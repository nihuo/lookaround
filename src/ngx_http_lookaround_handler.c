/*
 *  Copyright (C) Cui Yingjie (nihuo) 
 *    
 */

#include "ngx_http_lookaround_handler.h"
#include "ngx_http_lookaround_util.h"
#include "contenttype.h"

//api action
#define N_ACTION_SETITEM     "setitem"
#define N_ACTION_GETITEM     "getitem"
#define N_ACTION_LOOKAROUND  "lookaround"
#define N_ACTION_GETITEMS    "getitems"
#define N_ACTION_RESET       "reset"
#define N_ACTION_STAT        "stat"
#define N_ACTION_IMOK        "imok"
#define N_ACTION_ABOUT       "about"
#define N_ACTION_HELP        "help"

#define N_ACTION_MAXLEN     32


static ngx_int_t ngx_http_lookaround_setitem(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_getitem(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_lookaround(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_getitems(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_reset(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_stat(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_imok(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_about(ngx_http_request_t *r);
static ngx_int_t ngx_http_lookaround_help(ngx_http_request_t *r);


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

    u_char uachbuf[ N_ACTION_MAXLEN + 1 ] ;

    uachbuf[ N_ACTION_MAXLEN ] = 0 ;

    fn_i_get_action( r , uachbuf ) ;

    if( ngx_strcmp( uachbuf , N_ACTION_LOOKAROUND ) == 0 )
    {
        return ngx_http_lookaround_lookaround( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_SETITEM ) == 0 )
    {
        return ngx_http_lookaround_setitem( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_GETITEM ) == 0 )
    {
        return ngx_http_lookaround_getitem( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_GETITEMS ) == 0 )
    {
        return ngx_http_lookaround_getitems( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_RESET ) == 0 )
    {
        return ngx_http_lookaround_reset( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_STAT ) == 0 )
    {
        return ngx_http_lookaround_stat( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_ABOUT ) == 0 )
    {
        return ngx_http_lookaround_about( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_IMOK ) == 0 )
    {
        return ngx_http_lookaround_imok( r ) ;
    }
    else if( ngx_strcmp( uachbuf , N_ACTION_HELP ) == 0 )
    {
        return ngx_http_lookaround_help( r ) ;
    }
    else
        return NGX_HTTP_BAD_REQUEST ;

    return NGX_DONE;
}



ngx_int_t ngx_http_lookaround_imok(ngx_http_request_t *r)
{
    const char  *pcszOut = "I'm OK" ;
    ngx_buf_t   *b;
    ngx_chain_t out;
    ngx_str_t*  pstr;
    ngx_int_t   rc;

    if(!(r->method & NGX_HTTP_GET))
    {
        return NGX_HTTP_NOT_ALLOWED;
    }
 
    r->headers_out.content_type.len = sizeof(CT_TXT) - 1;
    r->headers_out.content_type.data = (u_char *) CT_TXT;
 
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = strlen(pcszOut);


    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if(b == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate response buffer.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    out.buf = b;
    out.next = NULL;
        
    pstr = fn_pns_get_ngxStr( r->pool , (const u_char*)pcszOut ) ;
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




ngx_int_t ngx_http_lookaround_setitem(ngx_http_request_t *r)
{
    return NGX_DONE ;
}

ngx_int_t ngx_http_lookaround_getitem(ngx_http_request_t *r)
{
    return NGX_DONE ;
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
    return NGX_DONE ;
}

ngx_int_t ngx_http_lookaround_help(ngx_http_request_t *r)
{
    return NGX_DONE ;
}


