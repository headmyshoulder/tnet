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
#include <fstream>
#include <algorithm>
#include <unordered_set>


using namespace std;

void test_mtl( void )
{
    mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::row_major > > matrix( 2 , 2 );
    matrix( 0 , 0 ) = 4.0 ; matrix( 0 , 1 ) = 2.0;
    matrix( 1 , 0 ) = -0.5 ; matrix( 1 , 1 ) = 10.0;
    
    mtl::dense_vector< double > e1( 2 );
    e1( 0 ) = 1.0 ; e1( 1 ) = 0.0;
    
    mtl::dense_vector< double > e2( 2 );
    e2( 0 ) = 0.0 ; e2( 1 ) = 1.0;
    
    mtl::dense_vector< double > r1 , r2;
    r1 = matrix * e1;
    r2 = matrix * e2;
    
    cout << r1 << endl;
    cout << r2 << endl;
}

int main( int argc , char *argv[] )
{
    std::cout.precision( 14 );
    
    tnet::stop_db_type stops;
    tnet::stop_time_db_type stop_times;
    tnet::trip_db_type trips;
    
    open_stops( "../data/vbb_2013/stops.txt" , stops );
    open_trips( "../data/vbb_2013/trips.txt" , trips );
    open_stop_times( "../data/vbb_2013/stop_times.txt" , stop_times );
    
//     create_stop_times_map();
//     
//     filter_stops();
//     
//     create_google_matrix();
//     
//     power_iterations();
    
    std::unordered_map< size_t , std::vector< tnet::stop_time > > stop_times_map;
    for( auto const& st : stop_times )
    {
        stop_times_map[ st.trip_id ].push_back( st );
    }
    
    
    std::unordered_set< size_t > filtered_stop_ids;
    std::unordered_set< size_t > filtered_trip_ids;
    
    // grossraum potsdam
    double lat_min = 52.339244 , lat_max = 52.425539 , lon_min = 12.976463 , lon_max = 13.092983;
    
    // potsdam innenstadt
    // double lat_min = 52.373858 , lat_max = 52.412486 , lon_min = 13.030913 , lon_max = 13.082190;
    
    // potsdam noerdliche innenstadt
    // double lat_min = 52.399662 , lat_max = 52.410564 , lon_min = 13.04558 , lon_max = 13.065008;
    
    // humboldtbruecke
    // double lat_min = 52.4 , lat_max = 52.402 , lon_min = 13.072 , lon_max = 13.074;
    
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
    
    std::unordered_map< size_t , size_t > stop_id_map;
    size_t count = 0;
    for( auto id : filtered_stop_ids )
    {
        stop_id_map.insert( std::make_pair( id , count ) );
        ++count;
    }
    
    cout << filtered_stop_ids.size() << endl << endl;
    cout << filtered_trip_ids.size() << endl;
    
    
    typedef mtl::dense2D< double , mtl::matrix::parameters< mtl::tag::col_major > > matrix_type;
    // create google matrix
    matrix_type matrix( filtered_stop_ids.size() , filtered_stop_ids.size() );
    matrix = 0.0;
    {
        // mtl::inserter< compressed2D< double > > ins( matrix );
        for( auto id : filtered_trip_ids )
        {
            auto r = stop_times_map.equal_range( id );
            for( auto iter = r.first ; iter != r.second ; ++iter )
            {
                auto pred = []( tnet::stop_time const& t1 , tnet::stop_time const& t2 ) -> bool { return t1.stop_sequence < t2.stop_sequence; };
                auto &v = iter->second;
                if( ! std::is_sorted( v.begin() , v.end() , pred ) )
                   std::sort( v.begin() , v.end() , pred );
                if( v.empty() ) continue;
                for( size_t i=0 ; i<v.size() - 1 ; ++i )
                {
                    size_t id1 = stop_id_map.at( v[i].stop_id ) , id2 = stop_id_map.at( v[i+1].stop_id );
                    matrix( id2 , id1 ) += 1.0;
                }
            }
        }
    }
    
    for( size_t i=0 ; i<matrix.dim2() ; ++i )
    {
        for( size_t j=0 ; j<matrix.dim1() ; ++j )
        {
            double val = matrix( j , i );
            if( std::isnan( val ) )
                cout << i << " " << j << " " << matrix( j , i ) << "\n";
        }
    }
    
       
    // normalize each row
    for( size_t i=0 ; i<matrix.dim2() ; ++i )
    {
        double sum = 0.0;
        for( size_t j=0 ; j<matrix.dim1() ; ++j ) sum += matrix( j , i );
        if( sum < 1.0e-10 ) continue;
        for( size_t j=0 ; j<matrix.dim1() ; ++j ) matrix( j , i ) /= sum;
        
//         for( size_t j=0 ; j<matrix.dim1() ; ++j )
//         {
//             double val = matrix( j , i );
//             if( std::isnan( val ) )
//                 cout << i << " " << j << " " << matrix( j , i ) << " " << sum << "\n";
//         }
    }
    
//    return -1;
    
    
//    cout << matrix << endl;
//     
//    return -1;
    
    
    double d = 0.8;
    matrix_type matrix2( matrix.dim1() , matrix.dim2() );
    matrix2 = matrix * d;
    matrix_type tmp3( matrix.dim1() , matrix.dim2() );
    tmp3 = ( 1.0 - d );
    matrix2 += tmp3;
    
    for( size_t i=0 ; i<matrix.dim1() ; ++i )
    {
        double sum = 0.0;
        for( size_t j=0 ; j<matrix2.dim2() ; ++j ) sum += matrix2( j , i );
        cout << i << " " << sum << endl;
    }
    
    // return -1 ;
    // cout << matrix2 << endl << endl << endl;
    
    
    mtl::dense_vector< double > ev( matrix.dim2() );
    ev( 0 ) = 1.0;
    
    for( size_t i=0 ; i<100 ; ++i )
    {
        cout << "Iteration " << i << endl;
        
        double sum = 0.0 , sum_sq = 0.0;
        for( size_t i=0 ; i<matrix.dim1() ; ++i )
        {
            sum += ev( i );
            sum_sq += ev( i ) * ev( i );
            // cout << ev( i ) << " ";
        }
        // cout << sum << " " << sum_sq << endl;
        cout << ev << endl;
        
        mtl::dense_vector< double > tmp;
        tmp = matrix2 * ev;
        ev = tmp;
        
        
    }
    
    ofstream fout( "pagerank.dat" );
    fout.precision( 14 );
    std::vector< double > pr( ev.begin() , ev.end() );
    std::sort( pr.begin() , pr.end() );
    for( size_t i=0 ; i<matrix.dim1() ; ++i )
        fout << ev(i) << " " << pr[i] << "\n";
    
    
    
    // find page rank
    
    
    
    
    
    
    return 0;
}
