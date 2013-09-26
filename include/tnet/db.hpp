/*
 * tnet/db.hpp
 * Date: 2013-09-26
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#ifndef TNET_DB_HPP_INCLUDED
#define TNET_DB_HPP_INCLUDED

#include <tnet/parse_csv.hpp>

#include <ostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <stdexcept>

namespace tnet {

struct stop
{
    size_t id;
    std::string name;
    double lat;
    double lon;
};

inline std::ostream& operator<<( std::ostream &out , stop const & s )
{
    out << "Stop ( " << s.id << ", " << s.name << ", " << s.lon << ", " << s.lat << " )";
    return out;
}

typedef std::unordered_map< size_t , stop > stop_db_type;

struct trip
{
    size_t id;
    std::string head_sign;
    size_t service_id;
};

inline std::ostream& operator<<( std::ostream &out , trip const & t )
{
    out << "Trip ( " << t.id << ", " << t.head_sign << ", " << t.service_id << " )";
    return out;
}

typedef std::unordered_map< size_t , trip > trip_db_type;


struct stop_time
{
    size_t trip_id;
    size_t stop_id;
    size_t stop_sequence;
};

inline std::ostream& operator<<( std::ostream &out , stop_time const & st )
{
    out << "Stop time ( " << st.trip_id << ", " << st.stop_id << ", " << st.stop_sequence << " )";
    return out;
}

typedef std::vector< stop_time > stop_time_db_type;





inline void open_stops( std::string const& filename , stop_db_type &stops )
{
    std::ifstream fin( filename );
    if( !fin.good() ) throw std::runtime_error( std::string( "open_stops : Could not open " ) + filename );
    std::string line;
    std::getline( fin , line );
    while( std::getline( fin , line ) )
    {
        stop s;
        std::vector< std::string > splitted;
        parse_line( line , splitted );
        s.id = std::stoi( splitted[0] );
        s.name = splitted[2];
        s.lat = std::stod( splitted[4] );
        s.lon = std::stod( splitted[5] );
        stops.insert( std::make_pair( s.id , s ) );
    }
}

inline void open_trips( std::string const& filename , trip_db_type &trips )
{
    std::ifstream fin( filename );
    if( !fin.good() ) throw std::runtime_error( std::string( "open_trips : Could not open " ) + filename );
    std::string line;
    std::getline( fin , line );
    while( std::getline( fin , line ) )
    {
        trip t;
        std::vector< std::string > splitted;
        parse_line( line , splitted );
        t.id = std::stoi( splitted[2] );
        t.head_sign  = splitted[3];
        t.service_id  = std::stod( splitted[1] );
        trips.insert( std::make_pair( t.id , t ) );
    }
}

inline void open_stop_times( std::string const& filename , stop_time_db_type &stop_times )
{
    std::ifstream fin( filename );
    if( !fin.good() ) throw std::runtime_error( std::string( "open_stop_times : Could not open " ) + filename );
    std::string line;
    std::getline( fin , line );
    while( std::getline( fin , line ) )
    {
        stop_time s;
        std::vector< std::string > splitted;
        parse_line( line , splitted );
        s.trip_id = std::stoi( splitted[0] );
        s.stop_id = std::stoi( splitted[3] );
        s.stop_sequence = std::stoi( splitted[4] );
        stop_times.push_back( s );
    }
}



} // namespace tnet


#endif // TNET_DB_HPP_INCLUDED
