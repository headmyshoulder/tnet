#! /usr/bin/python

import os

def prepare_file( filename ):
    os.system( "iconv -c -f UTF-8 -t ISO-8859-15 " + filename + " > tmp.txt" )
    os.system( "mv tmp.txt " + filename )

prepare_file( "data/15619/agency.txt" )
prepare_file( "data/15619/calendar_dates.txt" )
prepare_file( "data/15619/calendar.txt" )
prepare_file( "data/15619/routes.txt" )
prepare_file( "data/15619/stops.txt" )
prepare_file( "data/15619/stop_times.txt" )
prepare_file( "data/15619/transfers.txt" )
prepare_file( "data/15619/trips.txt" )