/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#ifndef __NGX_HTTP_LOOKAROUND_MODULE_H__
#define __NGX_HTTP_LOOKAROUND_MODULE_H__

#include "ngx_http_lookaround_global.h"


extern ngx_module_t  ngx_http_lookaround_module;

typedef struct server_status
{
    struct timeval tServerStartTime;  
    unsigned long long ullcTotalReq;
}
la_server_status;

typedef struct {
    unsigned int nMaxLevel ;
    
} ngx_http_lookaround_loc_conf_t;



extern la_server_status     *g_server_status ;
extern MM                   *g_la_mm;


#endif //__NGX_HTTP_LOOKAROUND_MODULE_H__
