#ifndef VERIFY_SOLUTION_HPP
#define VERIFY_SOLUTION_HPP

#include "read_file.h"
#include "sha256.h"
#include <array>
#include <stdlib.h>

/**
 * @brief verify the solution
 *
 * @param input_file_path path to the input data file
 * @param target_file_path path to the target data file
 * @param solution_file_path path to the solution data file
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
inline int verify_solution(
    char const* const input_file_path,
    char const* const target_file_path,
    char const* const solution_file_path )
{
    // read input from file
    auto const input_file_data = read_file( input_file_path );

    // verify read operation
    if ( !input_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias the input data
    auto const& input = input_file_data.second;

    // read target from file
    auto const target_file_data = read_file( target_file_path );

    // verify read operation
    if ( !target_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias the target data
    auto const& target = target_file_data.second;

    // read solution from file
    auto const solution_file_data = read_file( solution_file_path );

    // verify read operation
    if ( !solution_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias the solution data
    auto const& solution = solution_file_data.second;

    // concatenate input and candidate solution
    std::vector<unsigned char> subject{ input };
    subject.insert( subject.end(), solution.begin(), solution.end() );

    // create the hash
    auto const hash = sha256().hash( subject.data(), subject.size() );

//     // TODO: compare hash and target
//     if ( hash <= target ) {
//         std::cout << "1" << std::endl;
//         return EXIT_SUCCESS;
//     }

    std::cout << "0" << std::endl;

    return EXIT_FAILURE;
}

#endif // VERIFY_SOLUTION_HPP
