#include "keygen.h"
#include <stdlib.h>
#include <iostream>
#include <chrono>

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
    std::cout << "byte value = count\n";

    // iterate over the buckets in the historgram
    for ( unsigned int i = 0 ; i < 256 ; ++i ) {
        std::cout << i << " = " << histogram[i] << "\n";
    }
}

static void test_running_time( void )
{
    // security parameter is 128 bits
    const unsigned int security_parameter = 128;

    // get key size in bytes
    const unsigned int security_parameter_bytes = ( security_parameter + 8 - 1 ) / 8;

    // set the number of iterations
    const unsigned int runs = 5000;

    // run the test repeatedly
    for ( unsigned int i = 0 ; i < runs ; ++i ) {

        // get the time point at the beginning of the test run
        const auto start_time = std::chrono::high_resolution_clock::now();

        // generate the key
        const auto key = keygen( security_parameter_bytes );

        // get the time point at the end of the test run
        const auto end_time = std::chrono::high_resolution_clock::now();

        // calculate the elapsed time for the test run
        const auto duration = end_time - start_time;

        // TODO: calculate

    }
}

int main( int argc, const char *argv[] )
{
    test_frequency_distribution();
    test_running_time();
    return EXIT_SUCCESS;
}
