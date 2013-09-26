/*
 * google_matrix.cpp
 * Date: 2013-09-25
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#include <tnet/db.hpp>

#include <boost/numeric/mtl/mtl.hpp>

#include <iostream>
#include <algorithm>
#include <unordered_set>

using namespace std;

int main( int argc , char *argv[] )
{
    std::cout.precision( 14 );
    
    tnet::stop_db_type stops;
    tnet::stop_time_db_type stop_times;
    tnet::trip_db_type trips;
    
    open_stops( "../data/vbb_2013/stops.txt" , stops );
    open_trips( "../data/vbb_2013/trips.txt" , trips );
    open_stop_times( "../data/vbb_2013/stop_times.txt" , stop_times );
    
    std::unordered_map< size_t , std::vector< tnet::stop_time > > stop_times_map;
    for( auto const& st : stop_times )
    {
        stop_times_map[ st.trip_id ].push_back( st );
    }
    
    
    std::unordered_set< size_t > filtered_stop_ids;
    std::unordered_set< size_t > filtered_trip_ids;
    
    // grossraum potsdam
    // double lat_min = 52.339244 , lat_max = 52.425539 , lon_min = 12.976463 , lon_max = 13.092983;
    
    // potsdam innenstadt
    double lat_min = 52.373858 , lat_max = 52.412486 , lon_min = 13.030913 , lon_max = 13.082190;
    
    for( auto &st : stop_times_map )
    {
        std::sort( st.second.begin() , st.second.end() , []( tnet::stop_time const& s1 , tnet::stop_time const& s2 ) { return s1.stop_sequence < s2.stop_sequence; } );
        
        auto iter = std::find_if( st.second.begin() , st.second.end() , [&]( tnet::stop_time const& s ) {
            auto const& stop = stops.at( s.stop_id );
            return ( lat_min < stop.lat ) && ( stop.lat < lat_max ) && ( lon_min < stop.lon ) && ( stop.lon < lon_max ) ; } );
        if( iter != st.second.end() ) 
        {
            for( auto const s : st.second ) filtered_stop_ids.insert( s.stop_id );
            filtered_trip_ids.insert( st.first );
        }
    }
    
    cout << filtered_stop_ids.size() << endl << endl;
    cout << filtered_trip_ids.size() << endl;
    
    
    mtl::compressed2D< double > matrix;
    {
        mtl::inserter< compressed2D< double > > ins( matrix );
        for( auto id : filtered_trip_ids )
        {
        }
    }

    
    
    
    
    
    
    return 0;
}
