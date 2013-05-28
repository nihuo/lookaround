/*
 *  Copyright (C) Cui Yingjie (nihuo)
 *    
 */

#include "ngx_http_lookaround_module.h"
#include "ngx_http_lookaround_handler.h"
#include "ngx_http_lookaround_util.h"
#include "ngx_http_lookaround_map.h"


MM                  *g_pNLAMM = NULL ;
la_server_status    *g_pNLAServerStatus = NULL ;


ngx_str_t   *g_psNLAEmptyStr ;
ngx_str_t   *g_psNLAZeroStr ;
ngx_str_t   *g_psNLAOneStr ;



static void *ngx_http_lookaround_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_lookaround_merge_loc_conf(ngx_conf_t *cf,
        void *parent, void *child);
static char *ngx_http_lookaround_setmaxmaplevel(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf);
static char *ngx_http_lookaround_setmembufsize(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf);
static char *ngx_http_lookaround_sethashsize(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf);

static ngx_int_t ngx_http_lookaround_init_master( ngx_log_t *log );
static ngx_int_t ngx_http_lookaround_init_module( ngx_cycle_t *cycle );
static void ngx_http_lookaround_destroy();




static ngx_command_t  ngx_http_lookaround_commands[] = {
    { ngx_string("lookaround_set_max_map_level"),
      NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_1MORE,
      ngx_http_lookaround_setmaxmaplevel,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
      
    { ngx_string("lookaround_set_mem_buf_size"),
      NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_1MORE,
      ngx_http_lookaround_setmembufsize,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
      
    { ngx_string("lookaround_set_hash_size"),
      NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_1MORE,
      ngx_http_lookaround_sethashsize,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
      
    ngx_null_command
};



static ngx_http_module_t  ngx_http_lookaround_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_lookaround_create_loc_conf,    /* create location configration */
    ngx_http_lookaround_merge_loc_conf      /* merge location configration */
};


ngx_module_t  ngx_http_lookaround_module = {
    NGX_MODULE_V1,
    &ngx_http_lookaround_module_ctx,        /* module context */
    ngx_http_lookaround_commands,           /* module directives */
    NGX_HTTP_MODULE,                        /* module type */
    ngx_http_lookaround_init_master,        /* init master */
    ngx_http_lookaround_init_module,       /* init module */
    NULL,                                   /* init process */
    NULL,                                   /* init thread */
    NULL,                                   /* exit thread */
    NULL,                                   /* exit process */
    &ngx_http_lookaround_destroy,           /* exit master */
    NGX_MODULE_V1_PADDING
};

static void *
ngx_http_lookaround_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_lookaround_loc_conf  *pConf;
    pConf = ngx_pcalloc(cf->pool, sizeof(ngx_http_lookaround_loc_conf));
    if (pConf == NULL) {
        return NULL;
    }

    pConf->nMapMaxLevel = 0 ;

    g_psNLAEmptyStr  = fn_pns_get_ngxStr( cf->pool, ( const u_char* )"" ) ;
    g_psNLAZeroStr    = fn_pns_get_ngxStr( cf->pool, ( const u_char* )"0" ) ;
    g_psNLAOneStr    = fn_pns_get_ngxStr( cf->pool, ( const u_char* )"1" ) ;


    return pConf ; 
}


static char *
ngx_http_lookaround_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    return NGX_CONF_OK ;
}




static char *ngx_http_lookaround_setmaxmaplevel(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf)
{
    ngx_http_core_loc_conf_t    *clcf;

    clcf = ngx_http_conf_get_module_loc_conf( cf, ngx_http_core_module ) ;

    clcf->handler = ngx_http_lookaround_handler ;

    ngx_http_lookaround_loc_conf    *p_la_conf = conf ;
    ngx_str_t                       *value ;

    value = cf->args->elts; 
    p_la_conf->nMapMaxLevel = ngx_atoi( value->data , value->len ) ;

    return NGX_CONF_OK ;    
}


static char *ngx_http_lookaround_setmembufsize(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf)
{
    ngx_http_lookaround_loc_conf    *p_la_conf = conf ;
    ngx_str_t                       *value ;

    value = cf->args->elts; 
    p_la_conf->nMaxMemBufSize = ngx_atoi( value->data , value->len ) ;
    
    return NGX_CONF_OK ;    
}


static char *ngx_http_lookaround_sethashsize(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf)

{
    ngx_http_lookaround_loc_conf    *p_la_conf = conf ;
    ngx_str_t                       *value ;

    value = cf->args->elts; 
    p_la_conf->nHashSize = ngx_atoi( value->data , value->len ) ;

    return NGX_CONF_OK ;    
}


static ngx_int_t ngx_http_lookaround_init_module( ngx_cycle_t *cycle )
{
    return NGX_OK ;
}


static ngx_int_t ngx_http_lookaround_init_master( ngx_log_t *log )
{
    int ipid = getpid() ;

    g_pNLAMM = fn_p_create_mm( MM_BUF_SIZE , MM_BUF_ID , ipid ) ;

    g_pNLAServerStatus  = NLA_MM_CALLOC (g_pNLAMM, sizeof (la_server_status)) ;
    g_pNLACoreData      = NLA_MM_CALLOC (g_pNLAMM, sizeof (nla_core_data)) ;

    fn_v_InitCoreData( g_pNLACoreData ) ;

    gettimeofday(&(g_pNLAServerStatus->tServerStartTime),NULL) ;
    g_pNLAServerStatus->ullcTotalReq   = 0 ;


    return NGX_OK ;
}

static void ngx_http_lookaround_destroy()
{
    int ipid = getpid() ;
    fn_v_del_mm_file( MM_BUF_ID , ipid ) ;
    mm_destroy (g_pNLAMM) ;
}


