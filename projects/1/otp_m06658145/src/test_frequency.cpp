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
    unsigned int histogram[8] = {0};

    // run the test repeatedly
    for ( unsigned int i = 0 ; i < runs ; ++i ) {

        // generate the key
        const auto key = keygen( security_parameter_bytes );

        // add to the appropriate bucket in the histogram
        ++histogram[key[0] & 0x7];
    }

    std::cout << "key distribution test parameters\n";
    std::cout << " security parameter = " << security_parameter << "\n";
    std::cout << " iterations = " << runs << "\n";
    std::cout << "\n";

    std::cout << "key distribution test result data\n";
    std::cout << " key | count  | probability | histogram\n";
    std::cout << " --- | ------ | ----------- | ---------\n";

    // iterate over the buckets in the historgram
    for ( unsigned int i = 0 ; i < 8 ; ++i ) {
        // print byte value, count, probability, and histogram graphic
        printf( " 0x%01X | 0x%04X | %-11.3f | %s\n", i, histogram[i], histogram[i] / (double) runs, std::string( histogram[i] / 12, '*' ).c_str() );
    }
}

int main( int argc, const char *argv[] )
{
    test_frequency_distribution();
    return EXIT_SUCCESS;
}

