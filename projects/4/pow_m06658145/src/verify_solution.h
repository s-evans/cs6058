#ifndef VERIFY_SOLUTION_HPP
#define VERIFY_SOLUTION_HPP

#include "read_file.h"
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
    auto const& input_data = input_file_data.second;

    // read target from file
    auto const target_file_data = read_file( target_file_path );

    // verify read operation
    if ( !target_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias the target data
    auto const& target_data = target_file_data.second;

    // read solution from file
    auto const solution_file_data = read_file( solution_file_path );

    // verify read operation
    if ( !solution_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias the solution data
    auto const& solution_data = solution_file_data.second;

    // TODO: implement

    return EXIT_SUCCESS;
}

#endif // VERIFY_SOLUTION_HPP
