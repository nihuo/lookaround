/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#include "ngx_http_lookaround_handler.h"
#include "ngx_http_lookaround_util.h"
#include "ngx_http_lookaround_contenttype.h"
#include "ngx_http_lookaround_map.h"

//api action
#define NLA_ACTION_SETITEM     "setitem"
#define NLA_ACTION_GETITEM     "getitem"
#define NLA_ACTION_LOOKAROUND  "lookaround"
#define NLA_ACTION_GETITEMS    "getitems"
#define NLA_ACTION_RESET       "reset"
#define NLA_ACTION_STAT        "stat"
#define NLA_ACTION_IMOK        "imok"
#define NLA_ACTION_ABOUT       "about"
#define NLA_ACTION_HELP        "help"

#define NLA_ACTION_MAXLEN     32

//param name
#define NLA_PN_ID               "id"
#define NLA_PN_LAT              "lat"
#define NLA_PN_LON              "lon"
#define NLA_PN_STATUS           "status"
#define NLA_PN_TYPE             "type"


static ngx_int_t ngx_http_lookaround_setitem(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_getitem(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_lookaround(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_getitems(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_reset(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_stat(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_imok(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_about(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_help(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_render_output(ngx_http_request_t *r, const char* pcszTxt , const char* pcszContentType );


static nla_data_node* ngx_http_lookaround_getdatanode( nla_core_data* pCoreData , const char* pcszID ) ; 
static nla_data_node* ngx_http_lookaround_setdataloc( nla_core_data* pCoreData , const char* pcszID , const nla_2dloc* pLoc , int iStatus , int iType , const ngx_http_lookaround_loc_conf *pConf ) ; 

static void ngx_http_lookaround_adddatanode( nla_map_node* pMapNode , nla_data_node* pDataNode , const ngx_http_lookaround_loc_conf *pConf ) ; 
static void ngx_http_lookaround_create_child_mapnode( nla_map_node* pMapNode ) ;
static void ngx_http_lookaround_remove_datanode_from_map( nla_map_node* pMapNode , nla_data_node* pDataNode , const ngx_http_lookaround_loc_conf *pConf ) ;

static const char* g_pcszIMOK       = "I'm OK" ;
static const char* g_pcszVersion    = "0.1" ;
static const char* g_pcszAbout      = "Version %s\nDeveloped by Cui Yingjie (nihuo)" ;
static const char* g_pcszHelp       = "Help" ;



ngx_int_t ngx_http_lookaround_handler(ngx_http_request_t *r)
{
    ngx_int_t   rc;
    
    if( !( r->method & ( NGX_HTTP_GET ) ) ) 
        return NGX_HTTP_NOT_ALLOWED;

    rc = ngx_http_discard_request_body(r);
    if( rc != NGX_OK && rc != NGX_AGAIN ) 
    {
        return rc;
    }

    if( ngx_http_set_content_type( r ) != NGX_OK ) 
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    u_char uachbuf[ NLA_ACTION_MAXLEN + 1 ] ;

    uachbuf[ NLA_ACTION_MAXLEN ] = 0 ;

    fn_i_get_action( r , uachbuf ) ;

    if( ngx_strcmp( uachbuf , NLA_ACTION_LOOKAROUND ) == 0 )
    {
        return ngx_http_lookaround_lookaround( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_SETITEM ) == 0 )
    {
        return ngx_http_lookaround_setitem( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_GETITEM ) == 0 )
    {
        return ngx_http_lookaround_getitem( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_GETITEMS ) == 0 )
    {
        return ngx_http_lookaround_getitems( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_RESET ) == 0 )
    {
        return ngx_http_lookaround_reset( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_STAT ) == 0 )
    {
        return ngx_http_lookaround_stat( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_ABOUT ) == 0 )
    {
        return ngx_http_lookaround_about( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_IMOK ) == 0 )
    {
        return ngx_http_lookaround_imok( r ) ;
    }
    else if( ngx_strcmp( uachbuf , NLA_ACTION_HELP ) == 0 )
    {
        return ngx_http_lookaround_help( r ) ;
    }
    else
        return NGX_HTTP_BAD_REQUEST ;

    return NGX_DONE;
}



ngx_int_t ngx_http_lookaround_render_output(ngx_http_request_t *r, const char* pcszTxt , const char* pcszContentType )
{
    ngx_buf_t   *b;
    ngx_chain_t out;
    ngx_str_t*  pstr;
    ngx_int_t   rc;

    if(!(r->method & NGX_HTTP_GET))
    {
        return NGX_HTTP_NOT_ALLOWED;
    }
 
    r->headers_out.content_type.len = strlen( pcszContentType ) - 1;
    r->headers_out.content_type.data = (u_char *)pcszContentType ;
 
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = strlen(pcszTxt);


    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if(b == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate response buffer.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    out.buf = b;
    out.next = NULL;
        
    pstr = fn_pns_get_ngxStr( r->pool , (const u_char*)pcszTxt ) ;
    if( pstr == NULL )
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    b->pos=pstr->data;
    b->last=pstr->data+pstr->len;
    b->memory = 1;
    b->last_buf = 1;
    rc = ngx_http_send_header(r);
    if(rc != NGX_OK)
    {
        return rc;
    }

    return ngx_http_output_filter(r, &out);
}


ngx_int_t ngx_http_lookaround_imok(ngx_http_request_t *r)
{
    return ngx_http_lookaround_render_output( r , g_pcszIMOK , CT_TXT ) ; 
}




ngx_int_t ngx_http_lookaround_setitem(ngx_http_request_t *r)
{
    ngx_http_lookaround_loc_conf    *pConf ;
    ngx_str_t       sID , sLat , sLon , sType , sStatus ;
    int             iType , iStatus ;
    nla_2dloc       loc ;


    pConf = ngx_http_get_module_loc_conf( r , ngx_http_lookaround_module ) ;

    if( ngx_http_arg( r, ( u_char* )NLA_PN_ID , sizeof( NLA_PN_ID ) - 1 , &sID ) != NGX_OK ) 
        return NGX_HTTP_BAD_REQUEST ;

    if( sID.len <= 0 || sID.len >= NLA_DATA_ID_MAX_LEN )
        return NGX_HTTP_BAD_REQUEST ;

    if( ngx_http_arg( r, ( u_char* )NLA_PN_LAT , sizeof( NLA_PN_LAT ) - 1 , &sLat ) != NGX_OK ) 
        return NGX_HTTP_BAD_REQUEST ;
    else
        loc.lat = ngx_atofp( sLat.data , sLat.len , NLA_DEG_FIXPOINT ) ; 

    if( ngx_http_arg( r, ( u_char* )NLA_PN_LON , sizeof( NLA_PN_LON ) - 1 , &sLon ) != NGX_OK ) 
        return NGX_HTTP_BAD_REQUEST ;
    else
        loc.lon = ngx_atofp( sLon.data , sLon.len , NLA_DEG_FIXPOINT ) ;

    if( ngx_http_arg( r, ( u_char* )NLA_PN_STATUS , sizeof( NLA_PN_STATUS ) - 1 , &sStatus ) != NGX_OK ) 
        iStatus = 0 ;
    else
        iStatus = ngx_atoi( sStatus.data , sStatus.len ) ;

    if( ngx_http_arg( r, ( u_char* )NLA_PN_TYPE , sizeof( NLA_PN_TYPE ) - 1 , &sType ) != NGX_OK ) 
        iType = 0 ;
    else
        iType = ngx_atoi( sType.data , sType.len ) ;
    
    char achIDBuf[ NLA_DATA_ID_MAX_LEN ] ;

    memcpy( achIDBuf , sID.data , sID.len ) ;
    achIDBuf[ sID.len ] = 0 ;

    ngx_http_lookaround_setdataloc( g_pNLACoreData , achIDBuf , &loc , iStatus , iType , pConf ) ; 

    return NGX_DONE ;
}

ngx_int_t ngx_http_lookaround_getitem(ngx_http_request_t *r)
{
    ngx_http_lookaround_loc_conf    *pConf ;
    ngx_str_t       sID ;
    nla_data_node   *pDataNode ;


    pConf = ngx_http_get_module_loc_conf( r , ngx_http_lookaround_module ) ;

    if( ngx_http_arg( r, ( u_char* )NLA_PN_ID , sizeof( NLA_PN_ID ) - 1 , &sID ) != NGX_OK ) 
        return NGX_HTTP_BAD_REQUEST ;

    if( sID.len <= 0 || sID.len >= NLA_DATA_ID_MAX_LEN )
        return NGX_HTTP_BAD_REQUEST ;

    char achIDBuf[ NLA_DATA_ID_MAX_LEN ] ;

    memcpy( achIDBuf , sID.data , sID.len ) ;
    achIDBuf[ sID.len ] = 0 ;

    pDataNode = ngx_http_lookaround_getdatanode( g_pNLACoreData , achIDBuf ) ;
    if( pDataNode == NULL )
    {
        return ngx_http_lookaround_render_output( r , "" , CT_JSON ) ;  
    }

    char achJsonBuf[ NLA_DATA_JSON_LEN ] ;

    fn_v_FormatDataItem( &pDataNode->data , achJsonBuf ) ;

    return ngx_http_lookaround_render_output( r , achJsonBuf , CT_JSON ) ;
}

ngx_int_t ngx_http_lookaround_lookaround(ngx_http_request_t *r)
{
    return NGX_DONE ;
}

ngx_int_t ngx_http_lookaround_getitems(ngx_http_request_t *r)
{
    return NGX_DONE ;
}

ngx_int_t ngx_http_lookaround_reset(ngx_http_request_t *r)
{
    return NGX_DONE ;
}

ngx_int_t ngx_http_lookaround_stat(ngx_http_request_t *r)
{
    return NGX_DONE ;
}

ngx_int_t ngx_http_lookaround_about(ngx_http_request_t *r)
{
    char achBuf[ 1000 ] ;

    sprintf( achBuf , g_pcszAbout , g_pcszVersion ) ;

    return ngx_http_lookaround_render_output( r , achBuf , CT_TXT ) ; 
}

ngx_int_t ngx_http_lookaround_help(ngx_http_request_t *r)
{
    return ngx_http_lookaround_render_output( r , g_pcszHelp , CT_TXT ) ; 
}




static nla_data_node* ngx_http_lookaround_getdatanode( nla_core_data* pCoreData , const char* pcszID ) 
{
    nla_data_node* pDataNode ;


    if( pCoreData == NULL )
        return NULL ;

    if( pCoreData->p_id2data_map == NULL || pCoreData->p_map_root == NULL || pCoreData->p_data_head == NULL )
        return NULL ;


    pDataNode = ( nla_data_node* )sm_get_ptrvalue( pCoreData->p_id2data_map , pcszID ) ;

    return pDataNode ;
}

static nla_data_node* ngx_http_lookaround_setdataloc( nla_core_data* pCoreData , const char* pcszID , const nla_2dloc* pLoc , int iStatus , int iType , const ngx_http_lookaround_loc_conf *pConf ) 
{
    nla_data_node *pDataNode ;

    pDataNode = ngx_http_lookaround_getdatanode( pCoreData , pcszID ) ;
    if( pDataNode == NULL )
    {
        pDataNode = NLA_MM_CALLOC( g_pNLAMM , sizeof( nla_data_node ) ) ;  
        strcpy( pDataNode->data.ach_id , pcszID ) ;

        pDataNode->data.i_status    = iStatus ;
        pDataNode->data.i_type      = iType ;

        pDataNode->data.loc         = *pLoc ;

        ngx_spinlock( &pCoreData->lock , 1 , NLA_DEF_LOCK_SPIN ) ;

        sm_put_ptrvalue( pCoreData->p_id2data_map , pcszID , pDataNode ) ;

        ngx_http_lookaround_adddatanode( pCoreData->p_map_root , pDataNode , pConf ) ;
        
        pCoreData->lock = 0 ;
    }
    else
    {
        nla_map_node* pMapNode = ( nla_map_node* )pDataNode->p_map_node ;

        if( fn_b_IsLocInRC( &pMapNode->rc , pLoc ) )
        {
            pDataNode->data.loc = *pLoc ;
        }
        else
        {
            ngx_spinlock( &pCoreData->lock , 1 , NLA_DEF_LOCK_SPIN ) ;
            
            ngx_http_lookaround_remove_datanode_from_map( pCoreData->p_map_root , pDataNode , pConf ) ;

            pDataNode->data.loc = *pLoc ; 
            ngx_http_lookaround_adddatanode( pCoreData->p_map_root , pDataNode , pConf ) ;

            pCoreData->lock = 0 ;
        }
    }

    pDataNode->data.last_update_time = time( NULL ) ;

    return NULL ;
}



static void ngx_http_lookaround_adddatanode( nla_map_node* pMapNode , nla_data_node* pDataNode , const ngx_http_lookaround_loc_conf *pConf ) 
{
    pMapNode->i_item_count++ ;

    if( pMapNode->i_level >= pConf->nMapMaxLevel )
    {
        pDataNode->next = pMapNode->p_data_node ;
        pDataNode->prev = NULL ;
        pDataNode->p_map_node = pMapNode ;

        if( pMapNode->p_data_node )
            pMapNode->p_data_node->prev = pDataNode ;

        pMapNode->p_data_node = pDataNode ;
    }
    else
    {
        if( pMapNode->ap_childs[ 0 ] == NULL )
        {
            ngx_http_lookaround_create_child_mapnode( pMapNode ) ;
        }

        int iChildIdx = fn_i_GetChildRcIdx( &pMapNode->rc , &pDataNode->data.loc ) ;
        
        if( iChildIdx < 0 || iChildIdx >= ( int )NLA_ARRAY_COUNT( pMapNode->ap_childs ) )
        {
            //error
            return ;
        }

        nla_map_node* pChildNode ;

        pChildNode = pMapNode->ap_childs[ iChildIdx ] ;

        ngx_http_lookaround_adddatanode( pChildNode , pDataNode , pConf ) ;
    }

}


static void ngx_http_lookaround_create_child_mapnode( nla_map_node* pMapNode ) 
{
    int         i ;
    nla_2dloc   center_loc ;

    if( pMapNode->ap_childs[ 0 ] != NULL )
      return ;

    center_loc = fn_loc_GetRCCenter( &pMapNode->rc ) ;
    for( i = 0 ; i < ( int )NLA_ARRAY_COUNT( pMapNode->ap_childs ) ; i++ )
    {
        pMapNode->ap_childs[ i ] = NLA_MM_CALLOC( g_pNLAMM , sizeof( nla_map_node ) ) ;
        pMapNode->ap_childs[ i ]->i_level = pMapNode->i_level + 1 ;
        pMapNode->ap_childs[ i ]->p_parent = pMapNode ;

        switch( i )
        {
        case NLA_MAP_CHILD_LT :
            pMapNode->ap_childs[ i ]->rc.lt = pMapNode->rc.lt ;
            pMapNode->ap_childs[ i ]->rc.rb = center_loc ;
            break ;
        case NLA_MAP_CHILD_RT :
            pMapNode->ap_childs[ i ]->rc.lt.lat = pMapNode->rc.lt.lat ;
            pMapNode->ap_childs[ i ]->rc.lt.lon = center_loc.lon ;
            pMapNode->ap_childs[ i ]->rc.rb.lat = center_loc.lat ;
            pMapNode->ap_childs[ i ]->rc.rb.lon = pMapNode->rc.rb.lon ;
            break ;
        case NLA_MAP_CHILD_LB :
            pMapNode->ap_childs[ i ]->rc.lt.lat = center_loc.lat ;
            pMapNode->ap_childs[ i ]->rc.lt.lon = pMapNode->rc.lt.lon ;
            pMapNode->ap_childs[ i ]->rc.rb.lat = pMapNode->rc.rb.lat ;
            pMapNode->ap_childs[ i ]->rc.rb.lon = center_loc.lon ;
            break ;
        case NLA_MAP_CHILD_RB :
        default:
            pMapNode->ap_childs[ i ]->rc.lt = center_loc ;
            pMapNode->ap_childs[ i ]->rc.rb = pMapNode->rc.rb ;
            break ;
        }
    }

}



static void ngx_http_lookaround_remove_datanode_from_map( nla_map_node* pMapNode , nla_data_node* pDataNode , const ngx_http_lookaround_loc_conf *pConf ) 
{
    if( pMapNode == NULL || pDataNode )
      return ;

    if( pMapNode->i_item_count <= 0 )
      return ;

    pMapNode->i_item_count-- ;

    if( pConf->nMapMaxLevel <= pMapNode->i_level )
    {
        if( pDataNode->prev )
            pDataNode->prev->next = pDataNode->next ;

        if( pDataNode->next )
            pDataNode->next->prev = pDataNode->prev ;

        if( pDataNode == pMapNode->p_data_node )
        {
            pMapNode->p_data_node = pDataNode->next ;

            if( pMapNode->p_data_node )
                pMapNode->p_data_node->prev = NULL ;
        }

        pDataNode->next = pDataNode->prev = NULL ;
    }
    else
    {
        int iChildIdx = fn_i_GetChildRcIdx( &pMapNode->rc , &pDataNode->data.loc ) ;
        
        if( iChildIdx < 0 || iChildIdx >= ( int )NLA_ARRAY_COUNT( pMapNode->ap_childs ) )
        {
            //error
            return ;
        }

        nla_map_node* pChildNode ;

        pChildNode = pMapNode->ap_childs[ iChildIdx ] ;

        ngx_http_lookaround_remove_datanode_from_map( pChildNode , pDataNode , pConf ) ;
    }
}
