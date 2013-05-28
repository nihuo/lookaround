/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#ifndef __NGX_HTTP_LOOKAROUND_MODULE_H__
#define __NGX_HTTP_LOOKAROUND_MODULE_H__

#include "ngx_http_lookaround_global.h"


NLA_EXTERN ngx_module_t  ngx_http_lookaround_module;

typedef struct server_status
{
    struct timeval tServerStartTime;  
    unsigned long long ullcTotalReq;
}
la_server_status;

typedef struct {
    unsigned int nMapMaxLevel ;
    unsigned int nMaxMemBufSize ;
    unsigned int nHashSize ;
    
} ngx_http_lookaround_loc_conf;



NLA_EXTERN la_server_status     *g_pNLAServerStatus ;
NLA_EXTERN MM                   *g_pNLAMM;


NLA_EXTERN ngx_str_t   *g_psNLAEmptyStr ;
NLA_EXTERN ngx_str_t   *g_psNLAZeroStr ;
NLA_EXTERN ngx_str_t   *g_psNLAOneStr ;


#endif //__NGX_HTTP_LOOKAROUND_MODULE_H__
