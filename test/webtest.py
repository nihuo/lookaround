#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import string
import urllib
import urllib2
import inspect

try: 
    import simplejson as json
except ImportError: 
    import json



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
                print "setitem passed"
            else:
                print "Error in setitem test" 
        except Exception, e:
            print "Exception catched. Error in setitem test: %s" % str( e )


    def testGetItem( self ):
        try:
            #prepare data
            url = self.generateURL( "setitem?id=%s&status=%d&type=%d&lat=%f&lon=%f" % ( "d0e3f836c81011e28497f23c91aec05e" , 0 , 2 , 37.123456 , 110.123456 ) ) ;
            output = self.downloadURL( url )
            if output != "OK":
                return "Error in getitem test: setitem failed"

            url = self.generateURL( "getitem?id=%s" % "d0e3f836c81011e28497f23c91aec05e" ) 
            output = self.downloadURL( url )
            if output == None:
                print "Error in getitem test: getitem failed"
                return
            
            data = json.loads( output )
            if data == None or not "id" in data:
                print "Error in getitem test: parse json failed, %s" % output
                return

            if data[ "id" ] != "d0e3f836c81011e28497f23c91aec05e": 
                print "Error in getitem test: id is not right"
                return

            if data[ "type" ] != "2":
                print "Error in getitem test: type is not right"
                return

            print "getitem passed"
        except Exception, e:
            print "Exception catched. Error in setitem test: %s" % str( e )


    def testLookAround( self ):
        try:
            #prepare data
            url = self.generateURL( "setitem?id=%s&status=%d&type=%d&lat=%f&lon=%f" % ( "d0e3f836c81011e28497f23c91aec05e" , 0 , 2 , 37.123456 , 110.123456 ) ) ;
            output = self.downloadURL( url )
            if output != "OK":
                return "Error in lookaround test: setitem failed"

            url = self.generateURL( "lookaround?lat=%f&lon=%f&radius=%f" % ( 37.123456 , 110.123456 , 1 ) )
            output = self.downloadURL( url )
            if output == None:
                print "Error in lookaround test: lookaround failed"
                return
            
            data = json.loads( output )
            if data == None:
                print "Error in lookaround test: parse json failed, %s" % output
                return

            if len( data ) <= 0:
                print "Error in lookaround test: empty result"
                return 

            item = data[ 0 ] 
            if item[ "id" ] != "d0e3f836c81011e28497f23c91aec05e":
                print "Error in lookaround test: id is not right, %s" % output
                return

            url = self.generateURL( "lookaround?lat=%f&lon=%f&radius=%f" % ( 37.123456 , 111.123456 , 1 ) )
            output = self.downloadURL( url )
            if output == None:
                print "Error in lookaround test: lookaround failed"
                return
            
            data = json.loads( output )
            if data == None:
                print "Error in lookaround test: parse json failed, %s" % output
                return

            if len( data ) > 0:
                print "Error in lookaround test: empty result"
                return 

            print "lookaround passed"
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
