#ifndef KEYGEN_TO_FILE_HPP
#define KEYGEN_TO_FILE_HPP

#include "aes.h"
#include "keygen.h"
#include "write_file.h"
#include <stdlib.h>

/**
 * @brief Generate an aes and prf key to output files
 *
 * @param aes_key_file path to output aes key file
 * @param prf_key_file path to output prf key file
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
inline int keygen_to_file(
    char const* const aes_key_file,
    char const* const prf_key_file )
{
    // generate an aes key
    auto const aes_key_data { keygen( KEY_SIZE ) };

    // write key data to output file
    if ( !write_file( aes_key_file, aes_key_data ) ) {
        return EXIT_FAILURE;
    }

    // generate a key for the PRF
    auto const prf_key_data { keygen( KEY_SIZE ) };

    // write key data to output file
    if ( !write_file( prf_key_file, prf_key_data ) ) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif // KEYGEN_TO_FILE_HPP
