#ifndef VERIFY_SOLUTION_HPP
#define VERIFY_SOLUTION_HPP

#include "read_file.h"
#include "sha256.h"
#include <algorithm>
#include <stdlib.h>

/**
 * @brief verify the solution
 *
 * @param input_file_path path to the input data file
 * @param target_file_path path to the target data file
 * @param solution_file_path path to the solution data file
 * @param output output stream
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
inline int verify_solution(
    char const* const input_file_path,
    char const* const target_file_path,
    char const* const solution_file_path,
    std::ostream& output )
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

    // verify target size
    if ( target.size() != 32 ) {
        std::cerr << "ERROR: invalid target" << std::endl;
        return EXIT_FAILURE;
    }

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

    // compare hash and target (target < hash)
    if ( std::lexicographical_compare( target.begin(), target.end(), hash.begin(), hash.end() ) ) {
        output << "0\n";
        return EXIT_FAILURE;
    }

    output << "1\n";

    return EXIT_SUCCESS;
}

#endif // VERIFY_SOLUTION_HPP
