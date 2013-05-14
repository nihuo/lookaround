/*
 *  Copyright (C) Cui Yingjie (nihuo)
 *    
 */

#include "ngx_http_lookaround_module.h"
#include "ngx_http_lookaround_handler.h"
#include "ngx_http_lookaround_util.h"


MM                   *g_la_mm = NULL ;
la_server_status     *g_server_status = NULL ;


static void *ngx_http_lookaround_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_lookaround_merge_loc_conf(ngx_conf_t *cf,
        void *parent, void *child);
static char *ngx_http_lookaround_setmaxlevel(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf);

static ngx_int_t ngx_http_lookaround_init();
static void ngx_http_lookaround_destroy();




static ngx_command_t  ngx_http_lookaround_commands[] = {
    { ngx_string("lookaround_setmaxlevel"),
      NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_1MORE,
      ngx_http_lookaround_setmaxlevel,
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
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    &ngx_http_lookaround_init,                   /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    &ngx_http_lookaround_destroy,                /* exit master */
    NGX_MODULE_V1_PADDING
};

static void *
ngx_http_lookaround_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_lookaround_loc_conf_t  *pConf;
    pConf = ngx_pcalloc(cf->pool, sizeof(ngx_http_lookaround_loc_conf_t));
    if (pConf == NULL) {
        return NULL;
    }

    pConf->nMaxLevel = 0 ;

    return pConf ; 
}


static char *
ngx_http_lookaround_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    return NGX_CONF_OK ;
}




static char *ngx_http_lookaround_setmaxlevel(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf)
{
    ngx_http_core_loc_conf_t    *clcf;

    clcf = ngx_http_conf_get_module_loc_conf( cf, ngx_http_core_module ) ;

    clcf->handler = ngx_http_lookaround_handler ;

    return NGX_CONF_OK ;    
}



static ngx_int_t ngx_http_lookaround_init()
{
    int ipid = getpid() ;

    g_la_mm = fn_p_create_mm( MM_BUF_SIZE , MM_BUF_ID , ipid ) ;

    g_server_status=N_MM_CALLOC (g_la_mm, sizeof (la_server_status)) ;

    gettimeofday(&(g_server_status->tServerStartTime),NULL) ;
    g_server_status->ullcTotalReq   = 0 ;

    return NGX_OK ;
}

static void ngx_http_lookaround_destroy()
{
    int ipid = getpid() ;
    fn_v_del_mm_file( MM_BUF_ID , ipid ) ;
    mm_destroy (g_la_mm) ;
}


