#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import inspect

sDir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
sSrcDir = sDir + "/src"

sConfigFile = sDir + "/config"
sConfigFileBK = sDir + "/config.bk"

try:
    os.remove( sConfigFileBK )
except:
    pass

try:
    os.rename( sConfigFile , sConfigFileBK ) 
except:
    pass


with open( sConfigFile, "w") as text_file:
    text_file.write("ngx_addon_name=ngx_http_lookaround_module\nHTTP_MODULES=\"$HTTP_MODULES ngx_http_lookaround_module\"\n")

    sSrcFiles = "NGX_ADDON_SRCS=\"$NGX_ADDON_SRCS"
    sDepFiles = "NGX_ADDON_DEPS=\"$NGX_ADDON_DEPS"
    #text_file.write("NGX_ADDON_SRCS=\"$NGX_ADDON_SRCS " )

    for r,d,files in os.walk(sSrcDir):
        for f in files:
            if f.endswith(".c"):
                sSrcFiles += " $ngx_addon_dir/src/" + f
            elif f.endswith(".h"):
                sDepFiles += " $ngx_addon_dir/src/" + f


    text_file.write( sSrcFiles + "\"\n" ) 
    text_file.write( sDepFiles + "\"\n" ) 
    text_file.write( "CORE_LIBS=\"$CORE_LIBS -lmm\"\nCFLAGS=\"$CFLAGS\"\n" )

