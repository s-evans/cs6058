#include "aes.h"
#include "keygen.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

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
    std::cerr << "\t" << exe << "keygen <prf_key_file_path> <aes_key_file_path>                                                          \n";
    std::cerr << "\t" << exe << "enc <prf_key_file_path> <aes_key_file_path> <index_file_path> <plaintext_dir_path> <ciphertext_dir_path>\n";
    std::cerr << "\t" << exe << "token <keyword> <prf_key_file_path> <token_file_path>                                                   \n";
    std::cerr << "\t" << exe << "search <index_file_path> <token_file_path> <ciphertext_dir_path> <aes_key_file_path>                    \n";
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
static std::pair<bool, std::vector<unsigned char>> read_file( char const* path )
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

    constexpr int const IV_SIZE = 16;
    constexpr int const KEY_SIZE = 32;

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

            // generate an aes key
            auto const aes_key_data { keygen( KEY_SIZE ) };

            // write key data to output file
            if ( !write_file( aes_key_file, aes_key_data ) ) {
                return EXIT_FAILURE;
            }

            // TODO: do key generation for prf
            // TODO: update write_file call below for prf key data

            // write key data to output file
            if ( !write_file( prf_key_file, aes_key_data ) ) {
                return EXIT_FAILURE;
            }

            break;
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

            // read key data from file
            auto const aes_key_file_data = read_file( aes_key_file );

            // verify read was successful
            if ( !aes_key_file_data.first ) {
                return EXIT_FAILURE;
            }

            // create an alias for the key data
            auto const& aes_key = aes_key_file_data.second;

            // verify size of the aes key
            if ( aes_key.size() != KEY_SIZE ) {
                std::cerr << "ERROR: invalid aes key size (" << aes_key.size() << " != 32)" << std::endl;
                return EXIT_FAILURE;
            }

            // read prf key data from file
            auto const prf_key_file_data = read_file( prf_key_file );

            // verify read was successful
            if ( !prf_key_file_data.first ) {
                return EXIT_FAILURE;
            }

            // create an alias for the key data
            auto const& prf_key = prf_key_file_data.second;

            // verify size of the prf key
            if ( prf_key.size() != KEY_SIZE ) {
                std::cerr << "ERROR: invalid prf key size (" << prf_key.size() << " != 32)" << std::endl;
                return EXIT_FAILURE;
            }

            // TODO: do something with the index data (perhaps this is a write-only operation, and not a read)

            // TODO: for each file in plaintext dir
            for ( int i = 0 ; i < 1 ; ++i ) {

                // read plaintext data from file
                auto const plaintext_file_data = read_file( plaintext_dir );

                // verify read was successful
                if ( !plaintext_file_data.first ) {
                    return EXIT_FAILURE;
                }

                // create an alias for the plaintext data
                auto const& plaintext = plaintext_file_data.second;

                // TODO: figure out what to do with the IV (single IV? multiple IVs? where the IV(s) should go?)

                // generate a random 128-bit initialization vector (IV)
                auto const iv { keygen( IV_SIZE ) };

                // TODO: tokenize the plaintext input file data (split tokens by whitespace)

                // TODO: for each token
                for ( int j = 0 ; j < 1 ; ++j ) {

                    // create an aes crypto context
                    aes aes_ctx( EVP_aes_256_cbc(), aes_key.data(), iv.data() );

                    // perform aes encryption
                    auto const ciphertext( aes_ctx.encrypt( plaintext.data(), plaintext.size() ) );

                    // TODO: add the ciphertext and file name to an index data structure
                }

                // TODO: create a ciphertext of the entire plaintext
                // TODO: concat ciphertext dir with a filename to create a file path

                // write ciphertext to output file
                if ( !write_file( ciphertext_dir, iv ) ) {
                    return EXIT_FAILURE;
                }
            }

            // TODO: output the index data to the index file

            break;
        }

        case OP::TOKEN: {
            // TODO: implement
            break;
        }

        case OP::SEARCH: {
            // TODO: implement
            break;
        }

        default: {
            std::cerr << "ERROR: Unknown operation type value (" << ( int )operation.second << ")" << std::endl;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
