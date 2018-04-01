#ifndef ADD_TOKEN_TO_FILE_HPP
#define ADD_TOKEN_TO_FILE_HPP

#include "prf.h"
#include "read_key_from_file.h"
#include "write_file.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

/**
 * @brief output the given token string to the token file
 *
 * @param token_keyword input token keyword to be added
 * @param prf_key_file path to input prf key
 * @param token_file path to output token file
 * @param output output stream
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
inline int add_token_to_file(
    char const* const token_keyword,
    char const* const prf_key_file,
    char const* const token_file,
    std::ostream& output )
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
        prf( prf_key.data(), reinterpret_cast<unsigned char const*>( token_keyword ), strlen( token_keyword ) );

    // write prf token to file
    if ( !write_file( token_file, std::vector<unsigned char>{ prf_token.begin(), prf_token.end() } ) ) {
        return EXIT_FAILURE;
    }

    // output the prf token
    output << std::string( reinterpret_cast<char const*>( prf_token.data() ), prf_token.size() ) << "\n";

    return EXIT_SUCCESS;
}

#endif // ADD_TOKEN_TO_FILE_HPP
