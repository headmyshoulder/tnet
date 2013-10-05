/*
 * google_matrix.cpp
 * Date: 2013-09-25
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#include <tnet/db.hpp>

#include <Amboss/KML.h>

#include <boost/numeric/mtl/mtl.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_set>


using namespace std;

std::unordered_map< size_t , std::vector< tnet::stop_time > > get_stop_times_map( tnet::stop_time_db_type const & stop_times )
{
    std::unordered_map< size_t , std::vector< tnet::stop_time > > stop_times_map;
    for( auto const& st : stop_times )
    {
        stop_times_map[ st.trip_id ].push_back( st );
    }
    return stop_times_map;
}

void filter_stops( std::unordered_map< size_t , std::vector< tnet::stop_time > > & stop_times_map , tnet::stop_db_type const &stops ,
                   double lat_min , double lat_max , double lon_min , double lon_max , std::unordered_set< size_t > &stop_ids , 
                   std::unordered_set< size_t > &trip_ids )
{
    for( auto &st : stop_times_map )
    {
        std::sort( st.second.begin() , st.second.end() , []( tnet::stop_time const& s1 , tnet::stop_time const& s2 ) { return s1.stop_sequence < s2.stop_sequence; } );
        
        auto iter = std::find_if( st.second.begin() , st.second.end() , [&]( tnet::stop_time const& s ) {
            auto const& stop = stops.at( s.stop_id );
            return ( lat_min < stop.lat ) && ( stop.lat < lat_max ) && ( lon_min < stop.lon ) && ( stop.lon < lon_max ) ; } );
        if( iter != st.second.end() ) 
        {
            for( auto const s : st.second ) stop_ids.insert( s.stop_id );
            trip_ids.insert( st.first );
        }
    }
}

void get_stop_ids( std::unordered_map< size_t , std::vector< tnet::stop_time > > & stop_times_map , std::unordered_set< size_t > &stop_ids , 
                   std::unordered_set< size_t > &trip_ids )
{
    for( auto &st : stop_times_map )
    {
        std::sort( st.second.begin() , st.second.end() , []( tnet::stop_time const& s1 , tnet::stop_time const& s2 ) { return s1.stop_sequence < s2.stop_sequence; } );
        for( auto const s : st.second ) stop_ids.insert( s.stop_id );
        trip_ids.insert( st.first );
    }
}

std::unordered_map< size_t , size_t > get_stop_id_map( std::unordered_set< size_t > const & stop_ids )
{
    std::unordered_map< size_t , size_t > stop_id_map;
    size_t count = 0;
    for( auto id : stop_ids )
    {
        stop_id_map.insert( std::make_pair( id , count ) );
        ++count;
    }
    return stop_id_map;
}

mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::col_major > >
get_adjacency_matrix( std::unordered_set< size_t > const& trip_ids , std::unordered_set< size_t > const& stop_ids , 
                   std::unordered_map< size_t , std::vector< tnet::stop_time > > const &stop_times_map ,
                   std::unordered_map< size_t , size_t > const & stop_id_map )
{
    typedef mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::col_major > > matrix_type;
    
    size_t dim = stop_ids.size();
    matrix_type matrix( dim , dim );
    for( auto id : trip_ids )
    {
        auto r = stop_times_map.equal_range( id );
        for( auto iter = r.first ; iter != r.second ; ++iter )
        {
            auto const& v = iter->second;
            if( v.empty() ) continue;
            for( size_t i=0 ; i<v.size() - 1 ; ++i )
            {
                size_t id1 = stop_id_map.at( v[i].stop_id ) , id2 = stop_id_map.at( v[i+1].stop_id );
                matrix( id2 , id1 ) += 1.0;
            }
        }
    }
    return matrix;
}

mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::col_major > >
get_google_matrix( mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::col_major > > matrix , double d = 0.8 )
{
    typedef mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::col_major > > matrix_type;
    
    size_t dim = matrix.dim1();
    
    // normalize each row
    for( size_t i=0 ; i<dim ; ++i )
    {
        double sum = 0.0;
        for( size_t j=0 ; j<dim ; ++j ) sum += matrix( j , i );
        if( sum < 1.0e-10 )
        {
            for( size_t j=0 ; j<dim ; ++j ) matrix( j , i ) = 1.0 / double( dim );
        }
        else
        {
            for( size_t j=0 ; j<dim ; ++j ) matrix( j , i ) /= sum;
        }
    }
    
    matrix_type matrix2( dim , dim );
    matrix2 = matrix * d;
    matrix_type tmp3( dim , dim );
    tmp3 = ( 1.0 - d );
    matrix2 += tmp3;
    return matrix2;
}


mtl::dense_vector< double >
power_iterations( mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::col_major > > const & matrix , size_t iterations = 10000 )
{
    mtl::dense_vector< double > ev( matrix.dim2() );
    ev = 1.0 / double( matrix.dim2() );
    
    for( size_t i=0 ; i<10000 ; ++i )
    {
        double sum = std::accumulate( ev.begin() , ev.end() , 0.0 );
        cout << "Iteration " << i << " " << sum << endl;
        // cout << ev << endl;
        
        mtl::dense_vector< double > tmp;
        tmp = matrix * ev;
        ev = tmp;
    }
    return ev;
}


typedef boost::geometry::model::point< double , 2 , boost::geometry::cs::spherical_equatorial< boost::geometry::degree > > point_type;

int main( int argc , char *argv[] )
{
    std::cout.precision( 14 );
    
    tnet::stop_db_type stops;
    tnet::stop_time_db_type stop_times;
    tnet::trip_db_type trips;
    
    open_stops( "../data/vbb_2013/stops.txt" , stops );
    open_trips( "../data/vbb_2013/trips.txt" , trips );
    open_stop_times( "../data/vbb_2013/stop_times.txt" , stop_times );
    
//     power_iterations();
    
    auto stop_times_map = get_stop_times_map( stop_times );
    
    std::unordered_set< size_t > stop_ids , trip_ids;
        
    double lat_min = 52.339244 , lat_max = 52.425539 , lon_min = 12.976463 , lon_max = 13.092983; // grossraum potsdam
    // double lat_min = 52.373858 , lat_max = 52.412486 , lon_min = 13.030913 , lon_max = 13.082190; // potsdam innenstadt
    // double lat_min = 52.399662 , lat_max = 52.410564 , lon_min = 13.04558 , lon_max = 13.065008;  // potsdam noerdliche innenstadt
    // double lat_min = 52.4 , lat_max = 52.402 , lon_min = 13.072 , lon_max = 13.074;  // humboldtbruecke
    // filter_stops( stop_times_map , stops , lat_min , lat_max , lon_min , lon_max , stop_ids , trip_ids );
    get_stop_ids( stop_times_map , stop_ids , trip_ids );
    
    
    auto stop_id_map = get_stop_id_map( stop_ids );
    
    cout << stop_ids.size() << endl;
    cout << trip_ids.size() << endl;
    
    auto adjacency_matrix = get_adjacency_matrix( trip_ids , stop_ids , stop_times_map , stop_id_map );
    auto matrix = get_google_matrix( adjacency_matrix );
    auto ev = power_iterations( matrix );
    
    size_t dim = stop_ids.size();
    ofstream res_out( "result.dat" );
    res_out.precision( 14 );
    for( auto stop_id : stop_ids )
    {
        size_t id = stop_id_map.at( stop_id );
        double pagerank = ev[ id ];
        tnet::stop stop = stops.at( stop_id );
        res_out << stop_id << " " << id << " " << stop.lat << " " << stop.lon << " " << pagerank;
        for( auto stop_id2 : stop_ids )
        {
            size_t id2 = stop_id_map.at( stop_id2 );
            if( adjacency_matrix( id2 , id ) > 1.0e-10 )
                res_out << " " << stop_id2 << " " << adjacency_matrix( id2 , id );
        }
        res_out << "\n";
    }

    
    
    ofstream fout( "pagerank.dat" );
    fout.precision( 14 );
    std::vector< double > pr( ev.begin() , ev.end() );
    std::sort( pr.begin() , pr.end() );
    for( size_t i=0 ; i<matrix.dim1() ; ++i )
        fout << ev(i) << " " << pr[i] << "\n";
    
    
    auto scalePagerank = [&]( double p ) -> double {
        // return ( 0.1 * std::log10( p ) + 0.9 ) * 1.4 ;
        // return ( 50.0 * p + 0.1 );
        double min = pr[0] , max = pr[matrix.dim1()-1];
        double diff = max - min;
        double val = ( p - min ) / diff;
        return val + 0.2;
    };
    Amboss::KML::Document kml;
    for( auto stop_id : stop_ids )
    {
        double pagerank = ev[ stop_id_map.at( stop_id ) ];
        tnet::stop stop = stops.at( stop_id );
        point_type p = point_type( stop.lon , stop.lat );
        Amboss::KML::IconStyle style( Amboss::KML::Green , scalePagerank( pagerank ) , Amboss::KML::ShadedDot );
        kml.add( Amboss::KML::Placemark( p , style ) );
    }
    kml.write( "weighted_stops.kml" );
    
    
    
    
    
    
    return 0;
}
