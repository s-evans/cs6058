#include "aes.h"
#include "index.h"
#include "keygen.h"
#include "prf.h"
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <iterator>

constexpr int const IV_SIZE = 16;
constexpr int const KEY_SIZE = 32;

// Define parameters
namespace PARAM
{

static std::string const HELP_LONG  = "--help";
static std::string const HELP_SHORT = "-h";

// Define supported operations
namespace OP
{

static std::string const KEYGEN  = "keygen";
static std::string const ENCRYPT = "enc";
static std::string const TOKEN   = "token";
static std::string const SEARCH  = "search";

} /* namespace OP */

} /* namespace PARAM */

// Define constants for supported operations
enum class OP {
    KEYGEN,
    ENCRYPT,
    TOKEN,
    SEARCH
};

/**
 * @brief convert from string to operation constant
 *
 * @param op operation in string form
 *
 * @return operation in enum form
 */
static std::pair<bool, OP> get_op( char const* const op )
{
    if ( PARAM::OP::KEYGEN.compare( op ) == 0 ) {
        return std::make_pair( true, OP::KEYGEN );
    }

    if ( PARAM::OP::ENCRYPT.compare( op ) == 0 ) {
        return std::make_pair( true, OP::ENCRYPT );
    }

    if ( PARAM::OP::TOKEN.compare( op ) == 0 ) {
        return std::make_pair( true, OP::TOKEN );
    }

    if ( PARAM::OP::SEARCH.compare( op ) == 0 ) {
        return std::make_pair( true, OP::SEARCH );
    }

    std::cerr << "ERROR: unknown operation '" << op << "' specified" << std::endl;

    return std::make_pair( false, OP::KEYGEN );
}

/**
 * @brief Print the help text for the program
 *
 * @param exe name of the binary
 */
static void print_help( char const* const exe )
{
    std::cerr << "\n";
    std::cerr << "Overview:\n";
    std::cerr << "\tperforms 256-bit AES key generation, encryption, and decryption\n";
    std::cerr << "\n";

    std::cerr << "Synopsis:\n";
    std::cerr << "\t" << exe << " (-h|--help)\n";
    std::cerr << "\t" << exe << " keygen <prf_key_file_path> <aes_key_file_path>                                                          \n";
    std::cerr << "\t" << exe << " enc <prf_key_file_path> <aes_key_file_path> <index_file_path> <plaintext_dir_path> <ciphertext_dir_path>\n";
    std::cerr << "\t" << exe << " token <keyword> <prf_key_file_path> <token_file_path>                                                   \n";
    std::cerr << "\t" << exe << " search <index_file_path> <token_file_path> <ciphertext_dir_path> <aes_key_file_path>                    \n";
    std::cerr << std::flush;
}

/**
 * @brief write an array of binary data to file
 *
 * @param path path to file to be written to
 * @param data array of binary data to be written
 *
 * @return true if successful; false otherwise;
 */
static bool write_file( const char* path, std::vector<unsigned char> const& data )
{
    // open file
    FILE* const os = fopen( path, "wb" );

    // verify file was opened successfully
    if ( !os ) {
        std::cerr << "ERROR: failed to open file '" << path << "'" << std::endl;
        return false;
    }

    // write to file
    if ( 1 != fwrite( data.data(), data.size(), 1, os ) ) {
        std::cerr << "ERROR: failed to write to file '" << path << "'" << std::endl;
        fclose( os );
        return false;
    }

    // close the file
    fclose( os );

    return true;
}

/**
 * @brief read entire binary file
 *
 * @param path path of file to be read
 *
 * @return true and vector of bytes if successful; false otherwise;
 */
static std::pair<bool, std::vector<unsigned char>> read_file( char const* const path )
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

/**
 * @brief reads key data from a file and verifies key length
 *
 * @param path path to key file
 *
 * @return first = true if successful; second = key data;
 */
static std::pair<bool, std::vector<unsigned char>> read_key_from_file( char const* const path )
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

/**
 * @brief Generate a an aes key to an output file
 *
 * @param aes_key_file path to output aes key file
 * @param prf_key_file path to output prf key file
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
static int keygen_to_file(
    char const* const aes_key_file,
    char const* const prf_key_file )
{
    // generate an aes key
    auto const aes_key_data { keygen( KEY_SIZE ) };

    // write key data to output file
    if ( !write_file( aes_key_file, aes_key_data ) ) {
        return EXIT_FAILURE;
    }

    // generate a key for the PRF
    auto const prf_key_data { keygen( KEY_SIZE ) };

    // write key data to output file
    if ( !write_file( prf_key_file, prf_key_data ) ) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Encrypt files in input directory to output directory
 *
 * @param prf_key_file path to prf key file
 * @param aes_key_file path to aes key file
 * @param index_file path to index file
 * @param plaintext_dir path to input directory
 * @param ciphertext_dir path to output directory
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int encrypt_directory(
    char const* const prf_key_file,
    char const* const aes_key_file,
    char const* const index_file,
    char const* const plaintext_dir,
    char const* const ciphertext_dir )
{
    // read key data from file
    auto const aes_key_file_data = read_key_from_file( aes_key_file );

    // verify read was successful
    if ( !aes_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& aes_key = aes_key_file_data.second;

    // read prf key data from file
    auto const prf_key_file_data = read_key_from_file( prf_key_file );

    // verify read was successful
    if ( !prf_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& prf_key = prf_key_file_data.second;

    // verify plaintext direcctory is in fact a directory
    if ( !boost::filesystem::is_directory( boost::filesystem::status( plaintext_dir ) ) ) {
        std::cerr << "ERROR: '" << plaintext_dir << "' is not a directory" << std::endl;
        return EXIT_FAILURE;
    }

    // verify ciphertext direcctory is in fact a directory
    if ( !boost::filesystem::is_directory( boost::filesystem::status( ciphertext_dir ) ) ) {
        std::cerr << "ERROR: '" << ciphertext_dir << "' is not a directory" << std::endl;
        return EXIT_FAILURE;
    }

    std::multimap<std::array<unsigned char, 16>, boost::filesystem::path> index;

    // for each file in plaintext dir
    for ( auto file = boost::filesystem::directory_iterator( plaintext_dir ) ;
        file != boost::filesystem::directory_iterator() ;
        ++file ) {

        // create output file path
        auto const output_file_path = boost::filesystem::path( ciphertext_dir ) / file->path().filename();

        // skip non-regular files
        if ( !boost::filesystem::is_regular_file( file->status() ) ) {
            continue;
        }

        // read plaintext data from file
        auto const plaintext_file_data = read_file( file->path().c_str() );

        // verify read was successful
        if ( !plaintext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the plaintext data
        auto const& plaintext = plaintext_file_data.second;

        // create an array of whitespace delimiter characters
        constexpr std::array<unsigned char, 4> const delimiters{ { ' ', '\n', '\r', '\t' } };

        // create a function object for identifying a delimiter
        auto is_delimiter = [&]( unsigned char v ){
            return std::find( delimiters.begin(), delimiters.end(), v ) != delimiters.end();
        };

        // create a function object for identifying a non-delimiter
        auto is_not_delimiter = [&]( unsigned char v ){
            return !is_delimiter( v );
        };

        // for each token
        for (
            // find first non-delimiter character in plaintext
            auto token_begin = std::find_if( plaintext.begin(), plaintext.end(), is_not_delimiter ),
            // find first delimiter character after non-delimiter
            token_end = std::find_if( token_begin, plaintext.end(), is_delimiter );
            // search entire plaintext
            token_begin != plaintext.end();
            // find first non-delimiter character after previous token
            token_begin = std::find_if( token_end, plaintext.end(), is_not_delimiter ),
            // find first delimiter character after new token
            token_end = std::find_if( token_begin, plaintext.end(), is_delimiter )
        ) {
            // add the ciphertext and file name to an index data structure
            index.emplace( std::make_pair( prf( prf_key.data(), &*token_begin, token_end - token_begin  ), output_file_path ) );
        }

        // generate a random 128-bit initialization vector (IV)
        auto const iv { keygen( IV_SIZE ) };

        // create an aes crypto context
        aes aes_ctx( EVP_aes_256_cbc(), aes_key.data(), iv.data() );

        // perform aes encryption
        auto ciphertext( aes_ctx.encrypt( plaintext.data(), plaintext.size() ) );

        // prepend the iv to the ciphertext
        ciphertext.insert( ciphertext.begin(), iv.begin(), iv.end() );

        // write ciphertext to output file
        if ( !write_file( output_file_path.c_str(), ciphertext ) ) {
            return EXIT_FAILURE;
        }
    }

    // write serialized index string to file
    if ( !write_file( index_file, serialize( index ) ) ) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Add the given token string to the token file
 *
 * @param token_keyword input token keyword to be added
 * @param prf_key_file path to input prf key
 * @param token_file path to output token file
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
static int add_token_to_file(
    char const* const token_keyword,
    char const* const prf_key_file,
    char const* const token_file )
{
    // read key data from file
    auto const prf_key_file_data = read_key_from_file( prf_key_file );

    // verify read was successful
    if ( !prf_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& prf_key = prf_key_file_data.second;

    // create prf token from input token
    auto const prf_token =
        prf( prf_key.data(), reinterpret_cast<const unsigned char*>( token_keyword ), strlen( token_keyword ) );

    // open file to append binary data
    FILE* const os = fopen( token_file, "ab" );

    // verify file was opened successfully
    if ( !os ) {
        std::cerr << "ERROR: failed to open file '" << token_file << "'" << std::endl;
        return EXIT_FAILURE;
    }

    // write to file
    if ( 1 != fwrite( prf_token.data(), prf_token.size(), 1, os ) ) {
        std::cerr << "ERROR: failed to write to file '" << token_file << "'" << std::endl;
        fclose( os );
        return EXIT_FAILURE;
    }

    // close the file
    fclose( os );

    return EXIT_SUCCESS;
}

/**
 * @brief Performs search function
 *
 * @param index_file path to index file
 * @param token_file path to token file
 * @param ciphertext_dir path to ciphertext directory
 * @param aes_key_file path to the aes key file
 *
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
static int search_token(
    char const* const index_file,
    char const* const token_file,
    char const* const ciphertext_dir,
    char const* const aes_key_file )
{
    // read key data from file
    auto const aes_key_file_data = read_key_from_file( aes_key_file );

    // verify read was successful
    if ( !aes_key_file_data.first ) {
        return EXIT_FAILURE;
    }

    // create an alias for the key data
    auto const& aes_key = aes_key_file_data.second;

    // read from index file
    auto const index_file_data = read_file( index_file );

    // verify read was successful
    if ( !index_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias for the index file data
    auto const& index_vector = index_file_data.second;

    // read from token file
    auto const token_file_data = read_file( token_file );

    // verify read was successful
    if ( !token_file_data.first ) {
        return EXIT_FAILURE;
    }

    // alias for the token file data
    auto const& token_data = token_file_data.second;

    // deserialize the index data structure from the index data buffer
    auto const index = deserialize( index_vector );

    std::set<boost::filesystem::path> matching_files;

    // iterate over input tokens
    for ( auto itr = token_data.begin() ; itr != token_data.end() ; itr += 16 ) {

        // verify enough space for a prf token
        if ( token_data.end() - itr < 16 ) {
            break;
        }

        // get the prf token from the input data
        std::array<unsigned char, 16> prf_token;
        std::copy( itr, itr + 16, prf_token.begin() );

        // iterate over matching files
        for ( auto file = index.lower_bound( prf_token ) ; file != index.upper_bound( prf_token ) ; ++file ) {
            // add the matching files to the set
            matching_files.insert( file->second );
        }
    }

    // output space delimited filenames on the cli
    std::copy( matching_files.begin(), matching_files.end(), std::ostream_iterator<boost::filesystem::path>( std::cout, " " ) );
    std::cout << std::endl;

    // for each matching file
    for ( auto&& file : matching_files ) {

        // output the file name
        std::cout << file << ": ";

        // read encrypted file
        auto const read_operation = read_file( file.c_str() );

        // verify file read status
        if ( !read_operation.first ) {
            continue;
        }

        // create an alias for the encrypted file data
        auto const& file_data = read_operation.second;

        // verify that the file data contains enough data for the iv
        if ( file_data.size() < IV_SIZE ) {
            std::cout << "IV NOT FOUND" << std::endl;
            continue;
        }

        // create an aes crypto context
        aes ctx{ EVP_aes_256_cbc(), aes_key.data(), file_data.data() };

        // decrypt file data
        auto const decrypted_data{ ctx.decrypt( file_data.data() + IV_SIZE, file_data.size() - IV_SIZE ) };

        // output decrypted file to cout
        std::cout << (char const*) decrypted_data.data() << std::endl;
    }

    return EXIT_SUCCESS;
}

int main( int argc, char const* argv[] )
{
    // verify minimum argument count
    if ( argc < 2 ) {
        std::cerr << "ERROR: insufficient argument count" << std::endl;
        print_help( argv[0] );
        return EXIT_FAILURE;
    }

    // create an alias for the operation
    char const* const op = argv[1];

    // check if the user asked for help
    if ( PARAM::HELP_LONG.compare( op ) == 0 || PARAM::HELP_SHORT.compare( op ) == 0 ) {
        print_help( argv[0] );
        return EXIT_SUCCESS;
    }

    // convert from the user specified operation string to the operation int value
    auto const operation = get_op( op );

    // verify result of conversion
    if ( !operation.first ) {
        print_help( argv[0] );
        return EXIT_FAILURE;
    }

    switch ( operation.second ) {

        case OP::KEYGEN: {

            // verify argument count
            if ( argc != 4 ) {
                std::cerr << "ERROR: insufficient argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            // get string pointers for arguments
            char const* const aes_key_file = argv[2];
            char const* const prf_key_file = argv[3];

            return keygen_to_file( aes_key_file, prf_key_file );
        }

        case OP::ENCRYPT: {

            // verify argument count
            if ( argc != 7 ) {
                std::cerr << "ERROR: insufficient argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            // get string pointers for arguments
            char const* const prf_key_file   = argv[2];
            char const* const aes_key_file   = argv[3];
            char const* const index_file     = argv[4];
            char const* const plaintext_dir  = argv[5];
            char const* const ciphertext_dir = argv[6];

            return encrypt_directory(
                prf_key_file,
                aes_key_file,
                index_file,
                plaintext_dir,
                ciphertext_dir );
        }

        case OP::TOKEN: {

            // verify argument count
            if ( argc != 5 ) {
                std::cerr << "ERROR: insufficient argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            // get string pointers for arguments
            char const* const token_keyword = argv[2];
            char const* const prf_key_file  = argv[3];
            char const* const token_file    = argv[4];

            return add_token_to_file( token_keyword, prf_key_file, token_file );
        }

        case OP::SEARCH: {

            // verify argument count
            if ( argc != 6 ) {
                std::cerr << "ERROR: insufficient argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            // get string pointers for arguments
            char const* const index_file     = argv[2];
            char const* const token_file     = argv[3];
            char const* const ciphertext_dir = argv[4];
            char const* const aes_key_file   = argv[5];

            return search_token( index_file, token_file, ciphertext_dir, aes_key_file );
        }

        default: {
            std::cerr << "ERROR: Unknown operation type value (" << ( int )operation.second << ")" << std::endl;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
