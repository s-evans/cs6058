#ifndef TARGET_GENERATION_HPP
#define TARGET_GENERATION_HPP

#include "write_file.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <stdlib.h>

/**
 * @brief generate a target
 *
 * @param difficulty difficulty factor
 * @param target_file_path output target file path
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
inline int target_generation(
    int const difficulty,
    char const* const target_file_path )
{
    // verify input difficulty
    if ( 0 > difficulty || difficulty > 256 ) {
        std::cerr << "ERROR: difficulty value specified out of range" << std::endl;
        return EXIT_FAILURE;
    }

    // create a 256-bit bitset with bits set to zero
    std::array<unsigned char, 32> target{{0}};

    // populate trailing portion of bitset
    for ( int i = 255 ; i >= difficulty ; --i ) {
        target[i / 8] |= 1 << ( 7 - ( i % 8 ) );
    }

    // write the bitset to file in a binary format
    if ( !write_file( target_file_path, &target, sizeof( target ) ) ) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif // TARGET_GENERATION_HPP
