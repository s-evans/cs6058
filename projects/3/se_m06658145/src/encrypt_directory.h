#ifndef ENCRYPT_DIRECTORY_HPP
#define ENCRYPT_DIRECTORY_HPP

#include "aes.h"
#include "index.h"
#include "keygen.h"
#include "prf.h"
#include "read_key_from_file.h"
#include "write_file.h"
#include <array>
#include <boost/filesystem.hpp>
#include <map>
#include <stdlib.h>
#include <utility>

/**
 * @brief Encrypt files in input directory to output directory
 *
 * @param prf_key_file path to prf key file
 * @param aes_key_file path to aes key file
 * @param index_file path to index file
 * @param plaintext_dir path to input directory
 * @param ciphertext_dir path to output directory
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
inline int encrypt_directory(
    char const* const prf_key_file,
    char const* const aes_key_file,
    char const* const index_file,
    char const* const plaintext_dir,
    char const* const ciphertext_dir )
{
    // read key data from file
    auto const aes_key_file_data = read_key_from_file( aes_key_file );

    // verify read was successful
    if ( !aes_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& aes_key = aes_key_file_data.second;

    // read prf key data from file
    auto const prf_key_file_data = read_key_from_file( prf_key_file );

    // verify read was successful
    if ( !prf_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& prf_key = prf_key_file_data.second;

    // verify plaintext direcctory is in fact a directory
    if ( !boost::filesystem::is_directory( boost::filesystem::status( plaintext_dir ) ) ) {
        std::cerr << "ERROR: '" << plaintext_dir << "' is not a directory" << std::endl;
        return EXIT_FAILURE;
    }

    // verify ciphertext direcctory is in fact a directory
    if ( !boost::filesystem::is_directory( boost::filesystem::status( ciphertext_dir ) ) ) {
        std::cerr << "ERROR: '" << ciphertext_dir << "' is not a directory" << std::endl;
        return EXIT_FAILURE;
    }

    std::multimap<std::array<unsigned char, 16>, boost::filesystem::path> index;

    // for each file in plaintext dir
    for ( auto file = boost::filesystem::directory_iterator( plaintext_dir ) ;
        file != boost::filesystem::directory_iterator() ;
        ++file ) {

        // create output file path
        auto const output_file_path = boost::filesystem::path( ciphertext_dir ) / file->path().filename();

        // skip non-regular files
        if ( !boost::filesystem::is_regular_file( file->status() ) ) {
            continue;
        }

        // read plaintext data from file
        auto const plaintext_file_data = read_file( file->path().c_str() );

        // verify read was successful
        if ( !plaintext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the plaintext data
        auto const& plaintext = plaintext_file_data.second;

        // create an array of whitespace delimiter characters
        constexpr std::array<unsigned char, 4> const delimiters{ { ' ', '\n', '\r', '\t' } };

        // create a function object for identifying a delimiter
        auto is_delimiter = [&]( unsigned char v ){
            return std::find( delimiters.begin(), delimiters.end(), v ) != delimiters.end();
        };

        // create a function object for identifying a non-delimiter
        auto is_not_delimiter = [&]( unsigned char v ){
            return !is_delimiter( v );
        };

        // for each token
        for (
            // find first non-delimiter character in plaintext
            auto token_begin = std::find_if( plaintext.begin(), plaintext.end(), is_not_delimiter ),
            // find first delimiter character after non-delimiter
            token_end = std::find_if( token_begin, plaintext.end(), is_delimiter );
            // search entire plaintext
            token_begin != plaintext.end();
            // find first non-delimiter character after previous token
            token_begin = std::find_if( token_end, plaintext.end(), is_not_delimiter ),
            // find first delimiter character after new token
            token_end = std::find_if( token_begin, plaintext.end(), is_delimiter )
        ) {
            // add the ciphertext and file name to an index data structure
            index.emplace(
                std::make_pair( prf( prf_key.data(), &*token_begin, token_end - token_begin  ), output_file_path ) );
        }

        // generate a random 128-bit initialization vector (IV)
        auto const iv { keygen( IV_SIZE ) };

        // create an aes crypto context
        aes aes_ctx( EVP_aes_256_cbc(), aes_key.data(), iv.data() );

        // perform aes encryption
        auto ciphertext( aes_ctx.encrypt( plaintext.data(), plaintext.size() ) );

        // prepend the iv to the ciphertext
        ciphertext.insert( ciphertext.begin(), iv.begin(), iv.end() );

        // write ciphertext to output file
        if ( !write_file( output_file_path.c_str(), ciphertext ) ) {
            return EXIT_FAILURE;
        }
    }

    // write serialized index string to file
    if ( !write_file( index_file, serialize( index ) ) ) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif // ENCRYPT_DIRECTORY_HPP
