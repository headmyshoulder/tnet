#! /usr/bin/python

import gtfs

sched = gtfs.Schedule( "vbb_2013.db" )
session = sched.session

count = 0
for trip in sched.trips:
    trackQuery = session.query( gtfs.entity.StopTime ).filter( gtfs.entity.StopTime.trip_id == trip.trip_id )
    track = []
    for t in trackQuery :
        track.append( t )
    print str( count ) + " " + str( trip.trip_id ) + " " + str( len( track ) ) 
    count = count + 1

