/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#include "ngx_http_lookaround_map.h"
#include "ngx_http_lookaround_module.h"

nla_core_data* g_pNLACoreData = NULL ;


nla_2dloc fn_loc_GetRCCenter( const nla_rect* pRC )
{
    nla_2dloc loc ;

    loc.lat = ( pRC->lt.lat + pRC->rb.lat ) / 2 ;
    loc.lon = ( pRC->lt.lon + pRC->rb.lon ) / 2 ;

    return loc ;
}

BOOL fn_b_IsLocInRC( const nla_rect* pRC, const nla_2dloc* pLoc )
{
    return pLoc->lat >= pRC->lt.lat
            && pLoc->lat < pRC->rb.lat
            && pLoc->lon >= pRC->lt.lon
            && pLoc->lon < pRC->rb.lon ;
}


BOOL fn_b_IntersectRc( const nla_rect* pRC1, const nla_rect* pRC2 , nla_rect* pNewRC )
{
    if( pRC1->lt.lat >= pRC2->rb.lat
            || pRC1->rb.lat <= pRC2->lt.lat
            || pRC1->rb.lon <= pRC2->lt.lon
            || pRC1->lt.lon >= pRC2->rb.lon )
        return FALSE ;

    pNewRC->lt.lat = NLA_MAX( pRC1->lt.lat , pRC2->lt.lat ) ;
    pNewRC->lt.lon = NLA_MAX( pRC1->lt.lon , pRC2->lt.lon ) ;
    pNewRC->rb.lat = NLA_MIN( pRC1->rb.lat , pRC2->rb.lat ) ;
    pNewRC->rb.lon = NLA_MIN( pRC1->rb.lon , pRC2->rb.lon ) ;

    return TRUE ;
}




void fn_v_InitCoreData( nla_core_data* pCoreData ) 
{
    pCoreData->p_map_root   = NULL ;
    pCoreData->p_data_head  = NULL ;


    pCoreData->p_map_root   = NLA_MM_CALLOC( g_pNLAMM , sizeof( nla_map_node ) ) ;
    fn_v_InitRootMapNode( pCoreData->p_map_root ) ;

}


void fn_v_InitRootRect( nla_rect* pRC ) 
{
    pRC->lt.lat = NLA_MIN_LAT ;
    pRC->lt.lon = NLA_MIN_LON ;
    pRC->rb.lat = NLA_MAX_LAT ;
    pRC->rb.lon = NLA_MAX_LON ;
}

void fn_v_InitRootMapNode( nla_map_node *pMapNode )
{
    fn_v_InitRootRect( &pMapNode->rc ) ;
}

int fn_i_GetChildRcIdx( const nla_rect* pRC ,  const nla_2dloc* pLoc ) 
{
    if( !fn_b_IsLocInRC( pRC , pLoc ) )
        return -1 ;

    nla_2dloc center_loc = fn_loc_GetRCCenter( pRC ) ;

    if( pLoc->lat >= center_loc.lat )
    {
        if( pLoc->lon >= center_loc.lon )
            return NLA_MAP_CHILD_RB ;
        else
            return NLA_MAP_CHILD_LB ;
    }
    else
    {
        if( pLoc->lon >= center_loc.lon )
            return NLA_MAP_CHILD_RT ;
        else
            return NLA_MAP_CHILD_LT ;
    }
}



void fn_v_FormatDeg( NLADeg deg , char* pszBuf ) 
{
    sprintf( pszBuf , "%d.%06d" , deg / NLA_DEG_FIXPOINT_BASE , deg % NLA_DEG_FIXPOINT_BASE ) ;
}


void fn_v_FormatDataItem( nla_data_item *pData , char* pszBuf ) 
{
    char achBufLat[ NLA_DEG_MAX_BUF_LEN ] , achBufLon[ NLA_DEG_MAX_BUF_LEN ] ;

    fn_v_FormatDeg( pData->loc.lat , achBufLat ) ;
    fn_v_FormatDeg( pData->loc.lon , achBufLon ) ;

    sprintf( pszBuf , "{\"id\"=\"%s\",\"status\"=\"0x%x\",\"type\"=\"%d\",\"lat\"=\"%s\",\"lon\"=\"%s\",\"uptime\":\"%lld\"}" , pData->ach_id , pData->i_status , pData->i_type , achBufLat , achBufLon , ( long long )pData->last_update_time ) ;
}
