#! /usr/bin/python

import os
import glob
import csv


def make_db( filename , masterkey ):
    f = open( filename , "r" )
    reader = csv.DictReader( f )
    db = {}
    for row in reader:
        db[ row[ masterkey ] ] = row
    return db
        
        
agencies = make_db( "data/15619/agency.txt" , "agency_id" )
calendar_dates = make_db( "data/15619/calendar_dates.txt" )
calendar = make_db( "data/15619/calendar.txt" )
routes = make_db( "data/15619/routes.txt" )
stops = make_db( "data/15619/stops.txt" )
stop_times = make_db( "data/15619/stop_times.txt" )
transfers = make_db( "data/15619/transfers.txt" )
trips = make_db( "data/15619/trips.txt" )