/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#ifndef __NGX_HTTP_LOOKAROUND_MAP_H__
#define __NGX_HTTP_LOOKAROUND_MAP_H__


#include "ngx_http_lookaround_global.h"
#include "ngx_http_lookaround_strmap.h"


#define NLA_MAX_LON         180000000 
#define NLA_MIN_LON         -180000000
#define NLA_MAX_LAT         90000000
#define NLA_MIN_LAT         -90000000

#define NLA_EARTH_RADIUS    6371.229
#define NLA_LAT_DIST_FACTOR 8993

//map child node idx
#define NLA_MAP_CHILD_LT  0       //left top
#define NLA_MAP_CHILD_RT  1       //right top
#define NLA_MAP_CHILD_LB  2       //left bottom
#define NLA_MAP_CHILD_RB  3       //right bottom

#define NLA_MAP_CHILD_COUNT  4



#define NLA_DATA_ID_MAX_LEN     32
#define NLA_DATA_JSON_LEN       200



#define NLA_DEF_LOCK_SPIN       100


typedef struct NLA2DLoc
{
    NLADeg   lat ;
    NLADeg   lon ;
}
nla_2dloc ;

typedef struct NLARect
{
    nla_2dloc    lt ;    //left top
    nla_2dloc    rb ;    //right bottom
}
nla_rect ;


typedef struct NLADataItem
{
    char        ach_id[ NLA_DATA_ID_MAX_LEN ] ;
    int         i_status ;
    int         i_type ;
    nla_2dloc   loc ;
    time_t      last_update_time ;
}
nla_data_item ;

typedef struct NLADataNode
{
    nla_data_item   data ;
    struct NLADataNode *prev, *next ;
    void *  p_map_node ;
}
nla_data_node ;


typedef struct NLAMapNode
{
    nla_rect  rc ;

    struct NLAMapNode   *ap_childs[ 4 ] ;
    struct NLAMapNode   *p_parent ;

    int                 i_level ;

    nla_data_node       *p_data_node ;
    unsigned int        i_item_count ;
}
nla_map_node;


typedef struct NLACoreData
{
    nla_map_node    *p_map_root ;
    nla_data_node   *p_data_head ;

    struct StrMap   *p_id2data_map ;

    ngx_atomic_t    lock ;
}
nla_core_data;

NLA_EXTERN nla_core_data* g_pNLACoreData ;

NLA_EXTERN nla_2dloc fn_loc_GetRCCenter( const nla_rect* pRC ) ;
NLA_EXTERN BOOL fn_b_IsLocInRC( const nla_rect* pRC, const nla_2dloc* pLoc ) ;
NLA_EXTERN BOOL fn_b_IntersectRc( const nla_rect* pRC1, const nla_rect* pRC2, nla_rect* pNewRC ) ;
NLA_EXTERN double fn_d_GetDistance( const nla_2dloc* pLoc1, const nla_2dloc* pLoc2 ) ;
NLA_EXTERN void fn_v_InitCoreData( nla_core_data* pCoreData, int iHashSize ) ;
NLA_EXTERN void fn_v_InitRootRect( nla_rect* pRC ) ;
NLA_EXTERN void fn_v_InitRootMapNode( nla_map_node *pMapNode ) ;
NLA_EXTERN void fn_v_GetRectFromLocRadius( const nla_2dloc* pLoc, double dRadius, nla_rect* pRC ) ;


NLA_EXTERN int fn_i_GetChildRcIdx( const nla_rect* pRC,  const nla_2dloc* pLoc ) ;

NLA_EXTERN void fn_v_FormatDeg( NLADeg deg, char* pszBuf ) ; 
NLA_EXTERN void fn_v_FormatDataItem( nla_data_item *pData, char* pszBuf ) ; 
NLA_EXTERN NLADeg fn_x_LimitAdd( NLADeg deg, NLADeg dadd, NLADeg dmax, NLADeg dmin ) ;

#endif //__NGX_HTTP_LOOKAROUND_MAP_H__
