#ifndef WRITE_FILE_HPP
#define WRITE_FILE_HPP

#include <utility>
#include <vector>
#include <stdio.h>
#include <iostream>

/**
 * @brief write an array of binary data to file
 *
 * @param path path to file to be written to
 * @param data pointer to binary data to be written
 * @param size size of binary data to be written
 *
 * @return true if successful; false otherwise;
 */
inline bool write_file( const char* path, void const* data, size_t size )
{
    // open file
    FILE* const os = fopen( path, "wb" );

    // verify file was opened successfully
    if ( !os ) {
        std::cerr << "ERROR: failed to open file '" << path << "'" << std::endl;
        return false;
    }

    // write to file
    if ( 1 != fwrite( data, size, 1, os ) ) {
        std::cerr << "ERROR: failed to write to file '" << path << "'" << std::endl;
        fclose( os );
        return false;
    }

    // close the file
    fclose( os );

    return true;
}

#endif // WRITE_FILE_HPP
