#ifndef SEARCH_TOKEN_HPP
#define SEARCH_TOKEN_HPP

#include "aes.h"
#include "prf.h"
#include "read_file.h"
#include "read_key_from_file.h"
#include <algorithm>
#include <array>
#include <boost/filesystem.hpp>
#include <iostream>
#include <iterator>
#include <set>
#include <stdlib.h>
#include <utility>

/**
 * @brief Performs search function
 *
 * @param index_file path to index file
 * @param token_file path to token file
 * @param ciphertext_dir path to ciphertext directory
 * @param aes_key_file path to the aes key file
 * @param output output stream for search results
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
inline int search_token(
    char const* const index_file,
    char const* const token_file,
    char const* const ciphertext_dir,
    char const* const aes_key_file,
    std::ostream& output )
{
    // read key data from file
    auto const aes_key_file_data = read_key_from_file( aes_key_file );

    // verify read was successful
    if ( !aes_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& aes_key = aes_key_file_data.second;

    // read from index file
    auto const index_file_data = read_file( index_file );

    // verify read was successful
    if ( !index_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias for the index file data
    auto const& index_vector = index_file_data.second;

    // read from token file
    auto const token_file_data = read_file( token_file );

    // verify read was successful
    if ( !token_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias for the token file data
    auto const& token_data = token_file_data.second;

    // verify size of prf token read from file
    if ( token_data.size() != 16 ) {
        std::cerr << "ERROR: invalid token size (" << token_data.size() << " != 16)" << std::endl;
        return EXIT_FAILURE;
    }

    // deserialize the index data structure from the index data buffer
    auto const index = deserialize( index_vector );

    // create a container of distinct file paths
    std::set<boost::filesystem::path> matching_files;

    // get the prf token from the input data
    auto const prf_token = reinterpret_cast<std::array<unsigned char, 16> const*>( token_data.data() );

    // iterate over matching files
    for ( auto file = index.lower_bound( *prf_token ) ; file != index.upper_bound( *prf_token ) ; ++file ) {
        // add the matching files to the set
        matching_files.insert( file->second );
    }

    // output space delimited filenames on the cli
    std::copy(
        matching_files.begin(),
        matching_files.end(),
        std::ostream_iterator<boost::filesystem::path>( output, " " ) );
    output << "\n";

    // decrypt and output file data for those contain matching token

    // for each matching file
    for ( auto&& file : matching_files ) {

        // output the file name
        output << file << ": ";

        // read encrypted file
        auto const read_operation = read_file( file.c_str() );

        // verify file read status
        if ( !read_operation.first ) {
            continue;
        }

        // create an alias for the encrypted file data
        auto const& file_data = read_operation.second;

        // verify that the file data contains enough data for the iv
        if ( file_data.size() < IV_SIZE ) {
            output << "IV NOT FOUND" << std::endl;
            continue;
        }

        // create an aes crypto context
        aes ctx{ EVP_aes_256_cbc(), aes_key.data(), file_data.data() };

        // decrypt file data
        auto const decrypted_data{ ctx.decrypt( file_data.data() + IV_SIZE, file_data.size() - IV_SIZE ) };

        // output decrypted file to cout
        output << (char const*) decrypted_data.data() << "\n";
    }

    return EXIT_SUCCESS;
}

#endif // SEARCH_TOKEN_HPP
