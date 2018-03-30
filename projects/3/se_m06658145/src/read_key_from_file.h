#ifndef READ_KEY_FROM_FILE_HPP
#define READ_KEY_FROM_FILE_HPP

#include "aes.h"
#include "read_file.h"
#include <utility>
#include <vector>

/**
 * @brief reads key data from a file and verifies key length
 *
 * @param path path to key file
 *
 * @return first = true if successful; second = key data;
 */
inline std::pair<bool, std::vector<unsigned char>> read_key_from_file( char const* const path )
{
    // read key data from file
    auto key_file_data = read_file( path );

    // verify read was successful
    if ( !key_file_data.first ) {
        return key_file_data;
    }

    // create an alias for the key data
    auto const& key = key_file_data.second;

    // verify size of the aes key
    if ( key.size() != KEY_SIZE ) {
        std::cerr << "ERROR: invalid key size (" << key.size() << " != 32)" << std::endl;
        key_file_data.first = false;
        return key_file_data;
    }

    return key_file_data;
}

#endif // READ_KEY_FROM_FILE_HPP
