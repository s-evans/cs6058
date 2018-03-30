#ifndef INDEX_HPP
#define INDEX_HPP

#include <boost/filesystem.hpp>
#include <map>
#include <utility>
#include <vector>

using IndexType = std::multimap<std::array<unsigned char, 16>, boost::filesystem::path>;

/**
 * @brief Serialize the index data structure
 *
 * @param index index data structure
 *
 * @return serialized output
 */
inline std::vector<unsigned char> serialize( IndexType const& index )
{
    // serialize the index data to a string
    std::vector<unsigned char> index_string;

    // for each token
    for ( auto token_record = index.begin() ; token_record != index.end() ; ) {
        auto const& token = token_record->first;

        index_string.insert( index_string.end(), token.begin(), token.end() );

        // for each file
        do {
            auto const& file = token_record->second;
            auto const& filename_string = file.string();

            index_string.insert( index_string.end(), ' ' );
            index_string.insert( index_string.end(), filename_string.begin(), filename_string.end() );

            ++token_record;
        } while ( token_record != index.end() && token_record->first == token );

        index_string.insert( index_string.end(), '\n' );
    }

    return index_string;
}

/**
 * @brief Create an index data structure from serialized input
 *
 * @param index_string serialized input
 *
 * @return Index data structure
 */
inline IndexType deserialize( std::vector<unsigned char> const& index_string )
{
    IndexType index;

    // functor for determining if a character is a delimiter
    auto const is_delimiter = [&]( unsigned char const& v ){
        return v == ' ' || v == '\n';
    };

    // iterate over the vector
    for ( auto itr = index_string.begin() ; itr != index_string.end() ;  ) {

        // check for enough data
        if ( index_string.end() - itr < 16 ) {
            return index;
        }

        // grab 16 byte array as prf token
        std::array<unsigned char, 16> prf_token;
        std::copy( itr, itr + 16, prf_token.begin() );

        // jump over the prf token
        itr += 16;

        while ( itr != index_string.end() && *itr == ' ' ) {

            // jump over the space character
            ++itr;

            // find end of filename
            auto const end_itr = std::find_if( itr, index_string.end(), is_delimiter );
            if ( end_itr == index_string.end() ) {
                return index;
            }

            // add entry to the index
            index.emplace( std::make_pair( prf_token, boost::filesystem::path( (const char*) &*itr, (const char*) &*end_itr ) ) );

            // jump over the filename
            itr = end_itr;
        }

        // jump over the newline character
        ++itr;
    }

    return index;
}

#endif // INDEX_HPP
