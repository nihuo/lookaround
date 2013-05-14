/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#ifndef __NGX_HTTP_LOOKAROUND_UTIL_H__
#define __NGX_HTTP_LOOKAROUND_UTIL_H__


#include "ngx_http_lookaround_global.h"

NEXTERN ngx_int_t fn_i_get_action(ngx_http_request_t *r, u_char* pszoBuf ) ;
NEXTERN ngx_str_t* fn_pns_get_ngxStr( ngx_pool_t* p, const u_char* pcuszStr ) ;

NEXTERN MM* fn_p_create_mm( size_t nsize , int iID , int iPID );
NEXTERN void fn_v_del_mm_file( int iID , int iPID );


#endif //__NGX_HTTP_LOOKAROUND_UTIL_H__

