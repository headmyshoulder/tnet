/*
 * tnet/parse_csv.hpp
 * Date: 2013-09-26
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#ifndef TNET_PARSE_CSV_HPP_INCLUDED
#define TNET_PARSE_CSV_HPP_INCLUDED

#include <string>
#include <vector>


namespace tnet {

    
inline void parse_line( std::string const& line_str , std::vector< std::string > & line )
{
    bool in_quote ( false );
    std::string field;
    
    std::string::const_iterator iter = line_str.begin();
    while ( iter != line_str.end() )
    {
        switch ( *iter )
        {
        case '"':
            in_quote = !in_quote;
            break;
        case ',':
            if ( in_quote == true )
            {
                field += *iter;
            }
            else
            {
                line.push_back ( field );
                field.clear();
            }
            break;
        case '\n':
        case '\r':
            if ( in_quote == true )
            {
                field += *iter;
            }
            else
            {
                line.push_back ( field );
            }

            break;
        default:
            field.push_back ( *iter );
            break;
        }
        iter++;
    }
    if( !field.empty() )
    line.push_back( field );
}



} // namespace tnet


#endif // TNET_PARSE_CSV_HPP_INCLUDED
