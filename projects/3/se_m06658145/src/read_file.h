#ifndef READ_FILE_HPP
#define READ_FILE_HPP

#include <utility>
#include <vector>
#include <stdio.h>
#include <iostream>

/**
 * @brief read entire binary file
 *
 * @param path path of file to be read
 *
 * @return true and vector of bytes if successful; false otherwise;
 */
inline std::pair<bool, std::vector<unsigned char>> read_file( char const* const path )
{
    // open file
    FILE* const is = fopen( path, "rb" );

    // verify file was opened successfully
    if ( !is ) {
        std::cerr << "ERROR: failed to open file '" << path << "'" << std::endl;
        return std::make_pair( false, std::vector<unsigned char> {} );
    }

    // get size of file
    fseek( is, 0, SEEK_END );
    size_t const file_size = ftell( is );
    fseek( is, 0, SEEK_SET );

    // create a vector to hold the file data
    std::vector<unsigned char> data( file_size );

    // read from file
    if ( 1 != fread( data.data(), file_size, 1, is ) ) {
        fclose( is );
        std::cerr << "ERROR: failed to read from file '" << path << "'" << std::endl;
        return std::make_pair( false, std::vector<unsigned char> {} );
    }

    // close the file
    fclose( is );

    return std::make_pair( true, std::move( data ) );
}

#endif // READ_FILE_HPP
