#ifndef ADD_TOKEN_TO_FILE_HPP
#define ADD_TOKEN_TO_FILE_HPP

#include "prf.h"
#include "read_key_from_file.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Add the given token string to the token file
 *
 * @param token_keyword input token keyword to be added
 * @param prf_key_file path to input prf key
 * @param token_file path to output token file
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
inline int add_token_to_file(
    char const* const token_keyword,
    char const* const prf_key_file,
    char const* const token_file )
{
    // read key data from file
    auto const prf_key_file_data = read_key_from_file( prf_key_file );

    // verify read was successful
    if ( !prf_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& prf_key = prf_key_file_data.second;

    // create prf token from input token
    auto const prf_token =
        prf( prf_key.data(), reinterpret_cast<const unsigned char*>( token_keyword ), strlen( token_keyword ) );

    // open file to append binary data
    FILE* const os = fopen( token_file, "ab" );

    // verify file was opened successfully
    if ( !os ) {
        std::cerr << "ERROR: failed to open file '" << token_file << "'" << std::endl;
        return EXIT_FAILURE;
    }

    // write to file
    if ( 1 != fwrite( prf_token.data(), prf_token.size(), 1, os ) ) {
        std::cerr << "ERROR: failed to write to file '" << token_file << "'" << std::endl;
        fclose( os );
        return EXIT_FAILURE;
    }

    // close the file
    fclose( os );

    return EXIT_SUCCESS;
}

#endif // ADD_TOKEN_TO_FILE_HPP
