#ifndef GENERATE_SOLUTION_HPP
#define GENERATE_SOLUTION_HPP

#include "read_file.h"
#include "write_file.h"
#include <array>
#include <stdlib.h>

/**
 * @brief generate a solution
 *
 * @param input_file_path file path to the input data
 * @param target_file_path file path to the target data
 * @param solution_file_path path to the output solution file
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
inline int generate_solution(
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
    auto const& input_data = input_file_data.second;

    // read target from file
    auto const target_file_data = read_file( target_file_path );

    // verify read operation
    if ( !target_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias the target data
    auto const& target_data = target_file_data.second;

    std::array<unsigned char, 32> solution;

    // TODO: implement

    // write solution to file
    if ( !write_file( solution_file_path, solution.data(), solution.size() ) ) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif // GENERATE_SOLUTION_HPP
