#include "keygen.h"
#include <algorithm>
#include <chrono>
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

    std::vector<std::chrono::high_resolution_clock::duration> results( runs );

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

        // record the duration
        results[i] = duration;

    }

    std::chrono::high_resolution_clock::duration total_runtime =
        std::accumulate( results.begin(), results.end(), std::chrono::high_resolution_clock::duration{0}, [&](auto const& run, auto const& cur){ return run + cur; } );

    std::cout << "run time test result\n";
    std::cout << "security parameter = " << security_parameter_bytes * 8 << " bits\n";
    std::cout << "iterations = " << runs << "\n";
    std::cout << "min run time = " << std::min_element( results.begin(), results.end() )->count() << "\n";
    std::cout << "max run time = " << std::max_element( results.begin(), results.end() )->count() << "\n";
    std::cout << "average run time = " << total_runtime.count() / runs << "\n";
    std::cout << "total run time = " << total_runtime.count() << "\n";
}

int main( int argc, const char *argv[] )
{
    test_running_time();
    return EXIT_SUCCESS;
}
