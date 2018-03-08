#include "aes.h"
#include "keygen.h"
#include <algorithm>
#include <boost/lexical_cast.hpp>
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

static std::string const ENCRYPT = "enc";
static std::string const DECRYPT = "dec";
static std::string const KEYGEN  = "keygen";

} /* namespace OP */

// Define supported crypto modes
namespace MODE
{

static std::string const CBC = "cbc";
static std::string const ECB = "ecb";

} /* namespace MODE */

} /* namespace PARAM */

// Define constants for supported modes
enum class MODE {
    CBC,
    ECB
};

// Define constants for supported operations
enum class OP {
    KEYGEN,
    ENCRYPT,
    DECRYPT
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
    if ( PARAM::OP::DECRYPT.compare( op ) == 0 ) {
        return std::make_pair( true, OP::DECRYPT );
    }

    if ( PARAM::OP::ENCRYPT.compare( op ) == 0 ) {
        return std::make_pair( true, OP::ENCRYPT );
    }

    if ( PARAM::OP::KEYGEN.compare( op ) == 0 ) {
        return std::make_pair( true, OP::KEYGEN );
    }

    std::cerr << "ERROR: unknown operation '" << op << "' specified" << std::endl;

    return std::make_pair( false, OP::KEYGEN );
}

/**
 * @brief convert from string to mode constant
 *
 * @param mode mode in string form
 *
 * @return mode in enum form
 */
static std::pair<bool, MODE> get_mode( char const* const mode )
{
    if ( PARAM::MODE::ECB.compare( mode ) == 0 ) {
        return std::make_pair( true, MODE::ECB );
    }

    if ( PARAM::MODE::CBC.compare( mode ) == 0 ) {
        return std::make_pair( true, MODE::CBC );
    }

    std::cerr << "ERROR: invalid mode string '" << mode << "' specified" << std::endl;

    return std::make_pair( false, MODE::CBC );
}

/**
 * @brief get the openssl encryption type value from our mode value
 *
 * @param mode encryption mode
 *
 * @return openssl encryption mode
 */
static EVP_CIPHER const* get_evp_mode( MODE mode )
{
    switch ( mode ) {

        case MODE::CBC:
            return EVP_aes_256_cbc();

        case MODE::ECB:
            return EVP_aes_256_ecb();

        default:
            std::cerr << "ERROR: unknown mode type value (" << ( int )mode << ")" << std::endl;
            return NULL;
    }
}

/**
 * @brief get the size of the iv from the mode value
 *
 * @param mode encryption mode
 *
 * @return size of the iv in bytes
 */
static size_t get_iv_size( MODE mode )
{
    return ( mode == MODE::ECB ? 0 : 16 );
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
    std::cerr << "\t" << exe << " enc ecb <key_file_path> <plaintext_file_path> <ciphertext_file_path>\n";
    std::cerr << "\t" << exe << " dec ecb <key_file_path> <ciphertext_file_path> <plaintext_file_path>\n";
    std::cerr << "\t" << exe << " enc cbc <key_file_path> <plaintext_file_path> <ciphertext_file_path>\n";
    std::cerr << "\t" << exe << " dec cbc <key_file_path> <ciphertext_file_path> <plaintext_file_path>\n";
    std::cerr << "\t" << exe << " keygen <key_size> <key_file_path>\n";
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

    switch ( operation.second ) {

        case OP::DECRYPT: {

            // verify argument count
            if ( argc != 6 ) {
                std::cerr << "ERROR: insufficient argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            // get string pointers for arguments
            char const* const mode_string     = argv[2];
            char const* const key_file        = argv[3];
            char const* const ciphertext_file = argv[4];
            char const* const plaintext_file  = argv[5];

            // convert from the mode string to the mode int value
            auto const mode = get_mode( mode_string );

            // verify result of conversion
            if ( !mode.first ) {
                return EXIT_FAILURE;
            }

            // read key data from file
            auto const key_file_data = read_file( key_file );

            // verify read was successful
            if ( !key_file_data.first ) {
                return EXIT_FAILURE;
            }

            // create an alias for the key data
            auto const& key = key_file_data.second;

            // verify size of the key
            if ( key.size() != 32 ) {
                std::cerr << "ERROR: invalid key size (" << key.size() << " != 32)" << std::endl;
                return EXIT_FAILURE;
            }

            // read ciphertext data from file
            auto const ciphertext_file_data = read_file( ciphertext_file );

            // verify read was successful
            if ( !ciphertext_file_data.first ) {
                return EXIT_FAILURE;
            }

            // create an alias for the ciphertext data
            auto const& ciphertext = ciphertext_file_data.second;

            // get the size of the IV
            auto const iv_size = get_iv_size( mode.second );

            // ciphertext size validation
            if ( ciphertext.size() < iv_size ) {
                std::cerr << "ERROR: invalid ciphertext size (" << ciphertext.size() << " < " << iv_size << ")" << std::endl;
                return EXIT_FAILURE;
            }

            // create an aes context using the beginning of the ciphertext as the IV
            aes aes_ctx( get_evp_mode( mode.second ), key.data(), ciphertext.data() );

            // decrypt the ciphertext skipping the IV at its beginning
            std::vector<unsigned char> const plaintext( aes_ctx.decrypt( ciphertext.data() + iv_size, ciphertext.size() - iv_size ) );

            // write plaintext data to output file
            if ( !write_file( plaintext_file, plaintext ) ) {
                return EXIT_FAILURE;
            }

            break;
        }

        case OP::ENCRYPT: {

            // verify argument count
            if ( argc != 6 ) {
                std::cerr << "ERROR: insufficient argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            // get string pointers for arguments
            char const* const mode_string     = argv[2];
            char const* const key_file        = argv[3];
            char const* const plaintext_file  = argv[4];
            char const* const ciphertext_file = argv[5];

            // convert from mode string to mode int value
            auto const mode = get_mode( mode_string );

            // verify result of conversion
            if ( !mode.first ) {
                return EXIT_FAILURE;
            }

            // read key data from file
            auto const key_file_data = read_file( key_file );

            // verify read was successful
            if ( !key_file_data.first ) {
                return EXIT_FAILURE;
            }

            // create an alias for the key data
            auto const& key = key_file_data.second;

            // verify size of the key
            if ( key.size() != 32 ) {
                std::cerr << "ERROR: invalid key size (" << key.size() << " != 32)" << std::endl;
                return EXIT_FAILURE;
            }

            // read plaintext data from file
            auto const plaintext_file_data = read_file( plaintext_file );

            // verify read was successful
            if ( !plaintext_file_data.first ) {
                return EXIT_FAILURE;
            }

            // create an alias for the plaintext data
            auto const& plaintext = plaintext_file_data.second;

            // generate a random 128-bit initialization vector (IV) if necessary
            auto iv { keygen( get_iv_size( mode.second ) ) };

            // create an aes crypto context
            aes aes_ctx( get_evp_mode( mode.second ), key.data(), iv.data() );

            // perform aes encryption
            auto const ciphertext( aes_ctx.encrypt( plaintext.data(), plaintext.size() ) );

            // append ciphertext to the iv
            iv.insert( iv.end(), ciphertext.begin(), ciphertext.end() );

            // write iv and ciphertext to output file
            if ( !write_file( ciphertext_file, iv ) ) {
                return EXIT_FAILURE;
            }

            break;
        }

        case OP::KEYGEN: {

            // verify argument count
            if ( argc != 4 ) {
                std::cerr << "ERROR: insufficient argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            // get string pointers for arguments
            char const* const key_size = argv[2];
            char const* const key_file = argv[3];

            // convert argument from string to an unsigned integer
            unsigned int const key_size_int = boost::lexical_cast<unsigned int>( key_size );

            // verify user requested key length
            if ( key_size_int != 256 ) {
                std::cerr << "ERROR: invalid key size specified" << std::endl;
                return EXIT_FAILURE;
            }

            // get the size of the key in bytes
            unsigned int const key_bytes = ( key_size_int + 8 - 1 ) / 8;

            // generate a key
            auto const key_data { keygen( key_bytes ) };

            // write key data to output file
            if ( !write_file( key_file, key_data ) ) {
                return EXIT_FAILURE;
            }

            break;
        }

        default: {
            std::cerr << "ERROR: Unknown operation type value (" << ( int )operation.second << ")" << std::endl;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
