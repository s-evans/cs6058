#include "keygen.h"
#include <iostream>
#include <stdlib.h>

static void test_frequency_distribution( void )
{
    // security parameter is 3 bits
    const unsigned int security_parameter = 3;

    // get key size in bytes
    const unsigned int security_parameter_bytes = ( security_parameter + 8 - 1 ) / 8;

    // set the number of iterations
    const unsigned int runs = 5000;

    // allocate a histogram for byte values
    unsigned int histogram[256] = {0};

    // run the test repeatedly
    for ( unsigned int i = 0 ; i < runs ; ++i ) {

        // generate the key
        const auto key = keygen( security_parameter_bytes );

        // iterate over the bytes in the key
        for ( const auto byte : key ) {
            // add to the appropriate bucket in the histogram
            ++histogram[byte];
        }
    }

    std::cout << "key distribution test result data\n";

    // iterate over the buckets in the historgram
    for ( unsigned int i = 0 ; i < 256 ; ++i ) {
        // print byte value, count, and histogram graphic
        printf( "0x%02X = 0x%04X | %s\n", i, histogram[i], std::string( histogram[i], '*' ).c_str() );
    }
}

int main( int argc, const char *argv[] )
{
    test_frequency_distribution();
    return EXIT_SUCCESS;
}

