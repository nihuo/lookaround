/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#include "ngx_http_lookaround_util.h"

#define NLA_URI_ROOT        "/"
#define NLA_ACTION_MAXLEN   32

#define NLA_MM_FILEPATH     "/tmp/nla"

ngx_int_t fn_i_get_action(ngx_http_request_t *r, u_char* pszoBuf )
{
    *pszoBuf = 0 ;

    const u_char    *puch ;
    u_char          *puchDst ;
    ngx_int_t       i ;

    if( r->uri.len < sizeof( NLA_URI_ROOT ) )
        return 0 ;

    if( memcmp( NLA_URI_ROOT , r->uri.data , sizeof( NLA_URI_ROOT ) - 1 ) != 0 )
        return 0 ;

    puch        = r->uri.data + sizeof( NLA_URI_ROOT ) - 1 ;
    puchDst     = pszoBuf ;

    for( i = 0 ; i < NLA_ACTION_MAXLEN && i < ( ngx_int_t )( r->uri.len - sizeof( NLA_URI_ROOT ) + 1 ) && *puch != 0 && *puch != '?' && *puch != '#' && *puch != '/' ; i++ , puch++ , puchDst++ )
    {
        *puchDst = *puch ;
    }

    *puchDst = 0 ;

    return i ;
}

ngx_str_t* fn_pns_get_ngxStr( ngx_pool_t* p, const u_char* pcuszStr )
{
    ngx_int_t   iLen = ngx_strlen( pcuszStr ) ;
    ngx_str_t   *psValue ;

    u_char* pszBuf = ngx_palloc( p , iLen + 1 ) ;

    ngx_memcpy( pszBuf , pcuszStr , iLen + 1 ) ;

    psValue = ngx_palloc( p , sizeof( ngx_str_t ) ) ; 

    psValue->data   = pszBuf ;
    psValue->len    = iLen ;

    return psValue ;
}

MM* fn_p_create_mm( size_t nsize , int iID , int iPID )
{
    MM *pMM ;
    char achbuf[ NLA_MAX_PATH ] ;

    sprintf( achbuf , "%s-%d-%d", NLA_MM_FILEPATH , iPID , iID ) ;
    
    pMM = mm_create (nsize, achbuf);

    if (pMM == NULL)
    {
        //log_notice("Failed to create shared memory segments");
        exit (1);
    }

    mm_permission (pMM, 0600, getuid(), iPID);
    
    return pMM ;
}

void fn_v_del_mm_file( int iID , int iPID )
{
    char achbuf[ NLA_MAX_PATH ] ;

    sprintf( achbuf , "%s-%d-%d" , NLA_MM_FILEPATH , iPID , iID ) ;
    
    unlink( achbuf ) ;
}

