#ifndef TARGET_GENERATION_HPP
#define TARGET_GENERATION_HPP

#include <iostream>
#include <stdlib.h>

/**
 * @brief TODO
 *
 * @param difficulty
 * @param target_file_path
 *
 * @return 
 */
inline int target_generation(
    unsigned int const difficulty,
    char const* const target_file_path )
{
    if ( difficulty > 256 ) {
        std::cerr << "ERROR: difficulty value specified out of range" << std::endl;
        return EXIT_FAILURE;
    }

    // TODO: implement

    return EXIT_SUCCESS;
}

#endif // TARGET_GENERATION_HPP
