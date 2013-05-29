#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import string
import urllib
import urllib2
import inspect


class WebAPITest:
    def __init__( self ):
        self.host       = "localhost"
        self.webroot    = "/"


    def downloadURL( self , url ):
        try:
            request = urllib2.Request( url )
            response = urllib2.urlopen(request )
            return response.read()
        except Exception, e:
            print 'Error in WebAPITest::downloadURL %s , %s' % ( url , str( e ) )
            return None

    def generateURL( self , path ):
        return "http://" + self.host + self.webroot + path


    def testSetItem( self ):
        url = self.generateURL( "setitem?id=%s&status=%d&type=%d&lat=%f&lon=%f" % ( "d0e3f836c81011e28497f23c91aec05e" , 0 , 0 , 37.123456 , 110.123456 ) ) ;
        try:
            output = self.downloadURL( url )
            if output == "OK":
                print "passed"
            else:
                print "Error in setitem test" 
        except Exception, e:
            print "Exception catched. Error in setitem test: %s" % str( e )




    # run all function which function name is prefixed with 'test', case sensitive
    def Run( self ):
        methods = inspect.getmembers( WebAPITest , predicate=inspect.ismethod)

        for fn in methods:
            if fn[ 0 ].find( 'test' ) != 0:
                continue 

            fn[1]( self )
            

if __name__=="__main__":
    test = WebAPITest()

    test.Run() 
