#include "keygen.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdlib.h>

static void test_running_time( void )
{
    // security parameter is 128 bits
    const unsigned int security_parameter = 128;

    // get key size in bytes
    const unsigned int security_parameter_bytes = ( security_parameter + 8 - 1 ) / 8;

    // set the number of iterations
    const unsigned int runs = 5000;

    // allocate an array with an element for each test run
    std::vector<std::chrono::high_resolution_clock::duration> results( runs );

    // generate a random key
    const auto key = keygen( security_parameter_bytes );

    // generate a random plaintext
    const auto plaintext = keygen( security_parameter_bytes );

    // run the test repeatedly
    for ( unsigned int i = 0 ; i < runs ; ++i ) {

        // get the time point at the beginning of the test run
        const auto start_time = std::chrono::high_resolution_clock::now();

        // allocate a buffer for the ciphertext
        std::vector<unsigned char> ciphertext( plaintext.size() );

        // combine the plaintext with the key to create the ciphertext
        for ( size_t i = 0 ; i < ciphertext.size() ; ++i ) {
            ciphertext[i] = plaintext[i] ^ key[i];
        }

        // get the time point at the end of the test run
        const auto end_time = std::chrono::high_resolution_clock::now();

        // calculate the elapsed time for the test run
        const auto duration = end_time - start_time;

        // record the duration
        results[i] = duration;

    }

    // get the total run time of all test runs
    const std::chrono::high_resolution_clock::duration total_runtime = std::accumulate(
        results.begin(),
        results.end(),
        std::chrono::high_resolution_clock::duration{ 0 },
        [&]( auto const& run, auto const& cur ) { return run + cur; }
    );

    // sort the result data
    std::sort( results.begin(), results.end() );

    // calculate stats
    const auto min    = std::chrono::duration_cast<std::chrono::nanoseconds>( results[0] ).count();
    const auto max    = std::chrono::duration_cast<std::chrono::nanoseconds>( results[results.size() - 1] ).count();
    const auto total  = std::chrono::duration_cast<std::chrono::nanoseconds>( total_runtime ).count();
    const auto mean   = std::chrono::duration_cast<std::chrono::nanoseconds>( total_runtime / runs ).count();;

    // calculate the median
    const auto median = std::chrono::duration_cast<std::chrono::nanoseconds>(
        ( results.size() % 1 ) ?
        results[results.size() / 2 + 1] :
        ( results[results.size() / 2] + results[results.size() / 2 + 1] ) / 2 ).count();

    // calculate the variance
    const auto variance = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::accumulate(
            results.begin(),
            results.end(),
            std::chrono::high_resolution_clock::duration{ 0 },
            [&]( const auto& run, const auto& cur ) { return run + ( cur - ( total_runtime / runs ) ) * 2; }
        )
    ).count();

    // output parameters to the cli
    std::cout << "run time test parameters\n";
    std::cout << " security parameter = " << security_parameter_bytes * 8 << " bits\n";
    std::cout << " iterations         = " << runs << "\n";
    std::cout << "\n";

    // output results to the cli
    std::cout << "run time test results\n";
    std::cout << " min run time      = " << std::setw( 10 ) << min      << " ns\n";
    std::cout << " max run time      = " << std::setw( 10 ) << max      << " ns\n";
    std::cout << " mean run time     = " << std::setw( 10 ) << mean     << " ns\n";
    std::cout << " run time variance = " << std::setw( 10 ) << variance << " ns\n";
    std::cout << " median run time   = " << std::setw( 10 ) << median   << " ns\n";
    std::cout << " total run time    = " << std::setw( 10 ) << total    << " ns\n";
}

int main( int argc, const char *argv[] )
{
    test_running_time();
    return EXIT_SUCCESS;
}
