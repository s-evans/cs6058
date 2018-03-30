#ifndef PRF_HPP
#define PRF_HPP

#include "aes.h"
#include <array>
#include <algorithm>

/**
 * @brief Perform Psuedo Random Function on input data
 *
 * @param key prf key
 * @param data input data
 * @param size input data size
 *
 * @return prf output data
 */
inline std::array<unsigned char, 16> prf( unsigned char const* key, unsigned char const* data, size_t const size )
{
    // create an aes crypto context for prf
    aes aes_ctx( EVP_aes_256_ecb(), key, nullptr );

    // perform aes 256 ecb encryption as prf
    auto const prf_token( aes_ctx.encrypt( data, size ) );

    // truncate the prf token to a fixed length of 16 bytes
    std::array<unsigned char, 16> prf_token_truncated;
    std::copy( prf_token.begin(), prf_token.begin() + 16, prf_token_truncated.begin() );

    return prf_token_truncated;
}

#endif // PRF_HPP
