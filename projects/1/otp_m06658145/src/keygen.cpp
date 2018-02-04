#include "keygen.h"
#include <random>
#include <algorithm>

std::vector<char> keygen( unsigned int size )
{
    // allocate memory for key data
    std::vector<char> key_data( size );

    // get a random data source device
    std::random_device rd;

    // specify distribution to be used
    std::uniform_int_distribution<char> dist;

    // create the random key using the random source device and distribution
    std::generate( key_data.begin(), key_data.end(), [&](){ return dist( rd ); } );

    return key_data;
}

