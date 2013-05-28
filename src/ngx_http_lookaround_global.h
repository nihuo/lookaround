/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#ifndef __NGX_HTTP_LOOKAROUND_GLOBAL_H__
#define __NGX_HTTP_LOOKAROUND_GLOBAL_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>
#include <mm.h>
#include <time.h>


#define NLA_EXTERN extern
typedef int NLADeg ; //32 bit decimal fixed point representation, dividing by 1,000,000 as degree
typedef int BOOL ;

#define NLA_DEG_FIXPOINT        6
#define NLA_DEG_FIXPOINT_BASE   1000000
#define NLA_DEG_MAX_BUF_LEN     12


#ifndef TRUE
    #define TRUE    1
#endif //TRUE

#ifndef FALSE
    #define FALSE   0
#endif //FALSE



#define NLA_PI          3.14159265
#define NLA_MAX_PATH    256

#define MM_BUF_SIZE     ( 16 * 1024 * 1024 )
#define MM_BUF_ID       1


#define NLA_LOG_ERR_POS( log , msg )      ngx_log_error( NGX_LOG_ERR, log, 0, "[error] %s , in func: %s, file: %s line: %d date:%s time:%s" , msg , __FUNCTION__, __FILE__ ,  __LINE__ , __DATE__ , __TIME__ ) 
#define NLA_LOG_ERR( log , msg )          ngx_log_error( NGX_LOG_ERR, log, 0, "[error] %s" , msg ) 
#define NLA_LOG_DEBUG( log , msg )        ngx_log_debug1( NGX_LOG_DEBUG_HTTP, log, 0, "[debug] %s" , msg ) 

#define NLA_MM_CALLOC( mm , size )        mm_calloc( mm , 1 , size )

#define NLA_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define NLA_MIN(a,b)    (((a) < (b)) ? (a) : (b))

#define NLA_ARRAY_COUNT( a ) ( sizeof( a ) / sizeof( (a)[ 0 ] ) )




#endif //__NGX_HTTP_LOOKAROUND_GLOBAL_H__

