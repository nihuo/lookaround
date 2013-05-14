/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#ifndef __NGX_HTTP_LOOKAROUND_GLOBAL_H__
#define __NGX_HTTP_LOOKAROUND_GLOBAL_H__

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>
#include <mm.h>
#include <time.h>


#define NEXTERN extern

#define NLA_MAX_PATH    256

#define MM_BUF_SIZE     ( 16 * 1024 * 1024 )
#define MM_BUF_ID       1


#define N_LOG_ERR_POS( log , msg )      ngx_log_error( NGX_LOG_ERR, log, 0, "[error] %s , in func: %s, file: %s line: %d date:%s time:%s" , msg , __FUNCTION__, __FILE__ ,  __LINE__ , __DATE__ , __TIME__ ) 
#define N_LOG_ERR( log , msg )          ngx_log_error( NGX_LOG_ERR, log, 0, "[error] %s" , msg ) 
#define N_LOG_DEBUG( log , msg )        ngx_log_debug1( NGX_LOG_DEBUG_HTTP, log, 0, "[debug] %s" , msg ) 

#define N_MM_CALLOC( mm , size )        mm_calloc( mm , 1 , size )


#endif //__NGX_HTTP_LOOKAROUND_GLOBAL_H__

