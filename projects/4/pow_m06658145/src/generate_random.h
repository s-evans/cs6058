#ifndef GENERATE_RANDOM_HPP
#define GENERATE_RANDOM_HPP

#include <algorithm>
#include <random>
#include <vector>

/**
 * @brief generate a random string of bytes
 *
 * @param size size of random string in bytes
 *
 * @return vector of random bytes
 */
inline std::vector<unsigned char> generate_random( unsigned int size )
{
    // allocate memory for key data
    std::vector<unsigned char> key_data( size );

    // get a random data source device
    std::random_device rd;

    // specify distribution to be used
    std::uniform_int_distribution<unsigned char> dist;

    // create the random key using the random source device and distribution
    std::generate( key_data.begin(), key_data.end(), [&](){ return dist( rd ); } );

    return key_data;
}

#endif // GENERATE_RANDOM_HPP
