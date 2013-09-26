/*
 * google_matrix.cpp
 * Date: 2013-09-25
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#include <tnet/db.hpp>


#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>



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
    
    for( auto const& st : stop_times )
        cout << st << endl;
    
    return 0;
}
