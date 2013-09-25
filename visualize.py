#! /usr/bin/python

import gtfs
import simplekml


sched = gtfs.Schedule( "vbb_2013.db" )
session = sched.session

kml = simplekml.Kml()
points = kml.newfolder( name = "Stops" )
routes = kml.newfolder( name = "Routen" )

pointStyle = simplekml.Style()
pointStyle.iconstyle.icon.href = "http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png"
pointStyle.iconstyle.color = simplekml.Color.red
pointStyle.iconstyle.scale = 0.3

lineStyle = simplekml.Style()
lineStyle.linestyle.color = simplekml.Color.red


indexed_stops = {}

for stop in sched.stops:
    indexed_stops[ stop.stop_id ] = stop
    # kml.newpoint( name = stop.stop_name , coords = [ ( stop.stop_lon , stop.stop_lat ) ] )
    pnt = points.newpoint( coords = [ ( stop.stop_lon , stop.stop_lat ) ] )
    pnt.style = pointStyle


for trip in sched.trips:
    track = []
    for t in session.query( gtfs.entity.StopTime ).filter( gtfs.entity.StopTime.trip_id == trip.trip_id ) :
        track.append( t )
    track = sorted( track , key = lambda t : t.stop_sequence )
    coords = []
    for t in track :
        coords.append( ( indexed_stops[ t.stop_id ].stop_lon , indexed_stops[ t.stop_id ].stop_lat ) )
    lst = routes.newlinestring( coords = coords )
    lst.style = lineStyle



kml.save( "vbb_2011.kml" )