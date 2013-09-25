/*
 * google_matrix.cpp
 * Date: 2013-09-25
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */


#include <iostream>
#include <fstream>
#include <unordered_map>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

struct stop
{
    size_t id;
    std::string name;
    double lat;
    double lon;
};

struct trip
{
};

struct stop_time
{
};

typedef std::unordered_map< size_t , stop > stop_db_type;


void open_stops( std::string const& filename , stop_db_type &stops )
{
    std::ifstream fin( filename );
    std::string line;
    std::getline( fin , line );
    while( std::getline( fin , line ) )
    {
        stop s;
        std::vector< std::string > splitted;
        boost::algorithm::split( splitted , line , boost::algorithm::is_any_of( "," ) );
        std::cout << line << std::endl;
        s.id = std::stoi( splitted[0] );
        s.name = std::string( splitted[2].begin() + 1 , splitted[2].end() - 1 );
        s.lat = std::stod( std::string( splitted[4].begin() + 1 , splitted[4].end() -1 ) );
        s.lon = std::stod( std::string( splitted[5].begin() + 1 , splitted[5].end() -1 ) );
        stops.insert( std::make_pair( s.id , s ) );
    }
}

using namespace std;

int main( int argc , char *argv[] )
{
    stop_db_type stops;
    open_stops( "../data/vbb_2011_2/stops.txt" , stops );
    return 0;
}
