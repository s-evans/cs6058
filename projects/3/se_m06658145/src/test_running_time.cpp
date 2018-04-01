#include "add_token_to_file.h"
#include "encrypt_directory.h"
#include "keygen_to_file.h"
#include "search_token.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdlib.h>

/**
 * @brief calculate and output running time statistics
 *
 * @param results a vector of time durations
 */
static void output_stats( std::vector<std::chrono::high_resolution_clock::duration>& results )
{
    // get the total run time of all test iterations
    const std::chrono::high_resolution_clock::duration total_runtime = std::accumulate(
        results.begin(),
        results.end(),
        std::chrono::high_resolution_clock::duration{ 0 },
        [&]( auto const & run, auto const & cur ) {
            return run + cur;
        }
    );

    // sort the result data
    std::sort( results.begin(), results.end() );

    // calculate stats
    const auto min    = std::chrono::duration_cast<std::chrono::nanoseconds>( results[0] ).count();
    const auto max    = std::chrono::duration_cast<std::chrono::nanoseconds>( results[results.size() - 1] ).count();
    const auto total  = std::chrono::duration_cast<std::chrono::nanoseconds>( total_runtime ).count();
    const auto mean   = std::chrono::duration_cast<std::chrono::nanoseconds>( total_runtime / results.size() ).count();;

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
            [&]( const auto & run, const auto & cur ) {
                return run + ( cur - ( total_runtime / results.size() ) ) * 2;
            }
        )
    ).count();

    // output parameters to the cli
    std::cout << "run time test parameters\n";
    std::cout << " iterations         = " << results.size() << "\n";
    std::cout << "\n";

    // output results to the cli
    std::cout << "run time test results\n";
    std::cout << " min run time      = " << std::setw( 10 ) << min      << " ns\n";
    std::cout << " max run time      = " << std::setw( 10 ) << max      << " ns\n";
    std::cout << " mean run time     = " << std::setw( 10 ) << mean     << " ns\n";
    std::cout << " run time variance = " << std::setw( 10 ) << variance << " ns\n";
    std::cout << " median run time   = " << std::setw( 10 ) << median   << " ns\n";
    std::cout << " total run time    = " << std::setw( 10 ) << total    << " ns\n";
    std::cout << std::endl;
}

/**
 * @brief Runs a functor object the given number of iterations, tracks elapsed runtime, and outputs statistics
 *
 * @tparam T type of functor object
 * @param iterations number of iterations
 * @param f functor object to be timed
 */
template<class T>
static void test_running_time( unsigned int const iterations, T const& f )
{
    // allocate an array with an element for each test run
    std::vector<std::chrono::high_resolution_clock::duration> results( iterations );

    // run the test repeatedly
    for ( unsigned int i = 0 ; i < iterations ; ++i ) {

        // get the time point at the beginning of the test run
        const auto start_time = std::chrono::high_resolution_clock::now();

        // execute the timed function
        f();

        // get the time point at the end of the test run
        const auto end_time = std::chrono::high_resolution_clock::now();

        // calculate the elapsed time for the test run
        const auto duration = end_time - start_time;

        // record the duration
        results[i] = duration;

    }

    // calculate and output statistics for our test iterations
    output_stats( results );
}

void test_search_time(
    unsigned int const iterations,
    char const* const index_file,
    char const* const token_file,
    char const* const ciphertext_dir,
    char const* const aes_key_file )
{
    std::cout << "running encrypted index search timing test" << std::endl;

    // run test iterations
    test_running_time(
        iterations,
        [&]() {
            std::ostringstream oss;
            search_token(
                index_file,
                token_file,
                ciphertext_dir,
                aes_key_file,
                oss );
        }
    );
}

void test_encrypt_time(
    unsigned int const iterations,
    char const* const prf_key_file,
    char const* const aes_key_file,
    char const* const index_file,
    char const* const plaintext_dir,
    char const* const ciphertext_dir )
{
    std::cout << "running encrypted index generation timing test" << std::endl;

    // run test iterations
    test_running_time(
        iterations,
        [&]() {
            encrypt_directory(
                prf_key_file,
                aes_key_file,
                index_file,
                plaintext_dir,
                ciphertext_dir );
        }
    );
}

int main( int argc, const char* argv[] )
{
    // set the number of iterations
    constexpr unsigned int const ITERATIONS = 100;

    // set the parameters for the test
    char const aes_key_file[]   = "aes_key.bin";
    char const prf_key_file[]   = "prf_key.bin";
    char const index_file[]     = "index.bin";
    char const token_file[]     = "token.bin";
    char const plaintext_dir[]  = "../data/files/";
    char const ciphertext_dir[] = "../data/ciphertextfiles/";
    char const search_token[]   = "packers";

    // create keys
    if ( EXIT_SUCCESS != keygen_to_file( aes_key_file, prf_key_file ) ) {
        std::cerr << "failed to generate keys" << std::endl;
        return EXIT_FAILURE;
    }

    // perform input data encryption timing test
    test_encrypt_time( ITERATIONS, prf_key_file, aes_key_file, index_file, plaintext_dir, ciphertext_dir );

    // create token file
    std::ostringstream oss;
    if ( EXIT_SUCCESS != add_token_to_file( search_token, prf_key_file, token_file, oss ) ) {
        std::cerr << "failed to create token file" << std::endl;
        return EXIT_FAILURE;
    }

    // perform token search timing test
    test_search_time( ITERATIONS, index_file, token_file, ciphertext_dir, aes_key_file );

    return EXIT_SUCCESS;
}
