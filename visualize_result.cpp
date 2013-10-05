/*
 * visualize_result.cpp
 * Date: 2013-10-05
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#include <Amboss/KML.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

using namespace Amboss::KML;

struct stop
{
    size_t id;
    size_t index;
    double lat;
    double lon;
    double pagerank;
    std::vector< std::pair< size_t , double > > connections;
};

std::ostream& operator<<( std::ostream& out , stop const &s )
{
    out << s.id << " " << s.index << " " << s.lat << " " << s.lon << " " << s.pagerank;
    for( auto const& c : s.connections )
        out << " " << c.first << " " << c.second;
    return out;
}

typedef std::unordered_map< size_t , stop > stop_container;
typedef boost::geometry::model::point< double , 2 , boost::geometry::cs::spherical_equatorial< boost::geometry::degree > > point_type;
typedef boost::geometry::model::segment< point_type > segment_type;

stop read_stop( std::string const &line )
{
    std::istringstream str( line );
    stop s;
    str >> s.id >> s.index >> s.lat >> s.lon >> s.pagerank;
    while( !str.eof() )
    {
        size_t id;
        double weight;
        str >> id >> weight;
        s.connections.push_back( std::make_pair( id , weight ) );
    }
    return s;
}

Folder writeStops( stop_container const & stops )
{
    std::vector< double > pr;
    for( auto const &s : stops ) pr.push_back( s.second.pagerank );
    std::sort( pr.begin() , pr.end() );
    
    auto scalePagerank = [&]( double p ) -> double {
        // return ( 0.1 * std::log10( p ) + 0.9 ) * 1.4 ;
        // return ( 50.0 * p + 0.1 );
        double min = pr[0] , max = pr[pr.size()-1];
        double diff = max - min;
        double val = ( p - min ) / diff;
        return val + 0.2;
    };
    
    Folder folder( "Stops" );
    for( auto const& s : stops )
    {
        point_type p = point_type( s.second.lon , s.second.lat );
        IconStyle style( Green , scalePagerank( s.second.pagerank ) , ShadedDot );
        folder.add( Placemark( p , style ) );
    }
    return folder;
}

Folder writeStopsFrequency( stop_container const & stops )
{
    std::vector< double > pr;
    for( auto const &s : stops )
    {
        double f = 0.0;
        for( auto const &c : s.second.connections )
            f += c.second;
        pr.push_back( f );
    }
    std::sort( pr.begin() , pr.end() );
    
    auto scale = [&]( double p ) -> double {
        // return ( 0.1 * std::log10( p ) + 0.9 ) * 1.4 ;
        // return ( 50.0 * p + 0.1 );
        double min = pr[0] , max = pr[pr.size()-1];
        double diff = max - min;
        double val = ( p - min ) / diff;
        return val + 0.2;
    };
    
    Folder folder( "StopsFrequency" );
    for( auto const& s : stops )
    {
        double f = 0.0;
        for( auto const &c : s.second.connections )
            f += c.second;
        point_type p = point_type( s.second.lon , s.second.lat );
        IconStyle style( Red , scale( f ) , ShadedDot );
        folder.add( Placemark( p , style ) );
    }
    return folder;
}

Folder writeConnections( stop_container const & stops )
{
    Folder folder( "Connections" );
    double weight_min = 10000.0 , weight_max = -1.0;
    for( auto const& s : stops )
    {
        for( auto const &c : s.second.connections )
        {
            if( c.second > weight_max ) weight_max = c.second;
            if( c.second < weight_min ) weight_min = c.second;
        }
    }
    
    auto transparency = [&]( double weight ) -> int {
        double diff = weight_max - weight_min;
        double val = ( weight - weight_min ) / diff;
        return int( sqrt( val ) * 80.0 + 20.0 );
    };
    
    for( auto const& s : stops )
    {
        point_type p = point_type( s.second.lon , s.second.lat );
        for( auto const &c : s.second.connections )
        {
            auto s2 = stops.at( c.first );
            point_type p2 = point_type( s2.lon , s2.lat );
            Color color( 0 , 255 , 0 , transparency( c.second ) );
            LineStyle style( color , 3.0 );
            folder.add( Placemark( segment_type( p , p2 ) , Style( IconStyle() , style ) ) );
        }
    }
    return folder;
}





int main( int argc , char *argv[] )
{
    std::ifstream fin( "result.dat" );
    std::string line;
    stop_container stops;
    while( std::getline( fin , line ) )
    {
        stop s = read_stop( line );
        stops.insert( std::make_pair( s.id , s ) );
    }
    
//     for( auto const &s : stops )
//         std::cout << s.second << std::endl;

    for( auto const &s : stops )
    {
        double n = 0.0;
        for( size_t i=0 ; i<s.second.connections.size() ; ++i ) n += s.second.connections[i].second;
        std::cout << s.second.pagerank << " " << n << " " << s.second.connections.size() << "\n";
    }
    
    Document doc;
    doc.add( writeStops( stops ) );
    doc.add( writeConnections( stops ) );
    doc.add( writeStopsFrequency( stops ) );
    doc.write( "result.kml" );
    
    return 0;
}
