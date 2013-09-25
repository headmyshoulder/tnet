#! /usr/bin/python

import os

def prepare_file( filename ):
    os.system( "iconv -c -f UTF-8 -t ISO_8859-1 " + filename + " > tmp.txt" )
    os.system( "mv tmp.txt " + filename )

prepare_file( "data/vbb_2011_2/agency.txt" )
prepare_file( "data/vbb_2011_2/calendar_dates.txt" )
prepare_file( "data/vbb_2011_2/calendar.txt" )
prepare_file( "data/vbb_2011_2/routes.txt" )
prepare_file( "data/vbb_2011_2/shapes.txt" )
prepare_file( "data/vbb_2011_2/stops.txt" )
prepare_file( "data/vbb_2011_2/stop_times.txt" )
prepare_file( "data/vbb_2011_2/transfers.txt" )
prepare_file( "data/vbb_2011_2/trips.txt" )