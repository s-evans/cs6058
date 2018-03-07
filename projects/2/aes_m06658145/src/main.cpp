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
    std::basic_ofstream<unsigned char> os( path, std::ofstream::out | std::ofstream::binary );

    // verify file was opened successfully
    if ( !os.is_open() ) {
        std::cerr << "ERROR: failed to open file '" << path << "'" << std::endl;
        return false;
    }

    // write to file
    os.write( data.data(), data.size() );

    // verify write was successful
    if ( !os ) {
        std::cerr << "ERROR: failed to write to file '" << path << "'" << std::endl;
        return false;
    }

    // close the file
    os.close();

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
    std::basic_ifstream<unsigned char> is( path, std::ifstream::in | std::ifstream::binary );

    // verify file was opened successfully
    if ( !is.is_open() ) {
        std::cerr << "ERROR: failed to open file '" << path << "'" << std::endl;
        return std::make_pair( false, std::vector<unsigned char> {} );
    }

    // get size of file
    is.seekg( 0, is.end );
    size_t const file_size = is.tellg();
    is.seekg( 0, is.beg );

    // read from file
    std::vector<unsigned char> data( file_size );
    is.read( data.data(), file_size );

    // verify read was successful
    if ( !is ) {
        std::cerr << "ERROR: failed to read from file '" << path << "'" << std::endl;
        return std::make_pair( false, std::vector<unsigned char> {} );
    }

    // close the file
    is.close();

    return std::make_pair( true, std::move( data ) );
}

int main( int argc, char const* argv[] )
{
    if ( argc < 2 ) {
        std::cerr << "ERROR: insufficient argument count" << std::endl;
        print_help( argv[0] );
        return EXIT_FAILURE;
    }

    char const* const op = argv[1];

    if ( PARAM::HELP_LONG.compare( op ) == 0 || PARAM::HELP_SHORT.compare( op ) == 0 ) {
        print_help( argv[0] );
        return EXIT_SUCCESS;
    }

    auto const operation = get_op( op );

    if ( !operation.first ) {
        print_help( argv[0] );
        return EXIT_FAILURE;
    }

    switch ( operation.second ) {

    case OP::DECRYPT: {

        // verify argument count
        if ( argc != 6 ) {
            std::cerr << "ERROR: invalid argument count" << std::endl;
            print_help( argv[0] );
            return EXIT_FAILURE;
        }

        // get string pointers for arguments
        char const* const mode_string     = argv[2];
        char const* const key_file        = argv[3];
        char const* const ciphertext_file = argv[4];
        char const* const plaintext_file  = argv[5];

        auto const mode = get_mode( mode_string );

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

        // read ciphertext data from file
        auto const ciphertext_file_data = read_file( ciphertext_file );

        // verify read was successful
        if ( !ciphertext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the ciphertext data
        auto const& ciphertext = ciphertext_file_data.second;

        // TODO: salt?
        // TODO: iv?
        aes aes_ctx( get_evp_mode( mode.second ), key.data(), key.size(), (unsigned char*) "foo" );

        std::vector<unsigned char> plaintext( aes_ctx.decrypt( ciphertext.data(), ciphertext.size() ) );

        // write plaintext data to output file
        if ( !write_file( plaintext_file, plaintext ) ) {
            return EXIT_FAILURE;
        }

        break;
    }

    case OP::ENCRYPT: {

        // verify argument count
        if ( argc != 6 ) {
            std::cerr << "ERROR: invalid argument count" << std::endl;
            print_help( argv[0] );
            return EXIT_FAILURE;
        }

        // get string pointers for arguments
        char const* const mode_string     = argv[2];
        char const* const key_file        = argv[3];
        char const* const plaintext_file  = argv[4];
        char const* const ciphertext_file = argv[5];

        auto const mode = get_mode( mode_string );

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

        // read plaintext data from file
        auto const plaintext_file_data = read_file( plaintext_file );

        // verify read was successful
        if ( !plaintext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the plaintext data
        auto const& plaintext = plaintext_file_data.second;

        // TODO: salt?
        // TODO: IV
        aes aes_ctx( get_evp_mode( mode.second ), key.data(), key.size(), (unsigned char*) "foo" );
        std::vector<unsigned char> ciphertext( aes_ctx.encrypt( plaintext.data(), plaintext.size() ) );

        // write ciphertext to output file
        if ( !write_file( ciphertext_file, ciphertext ) ) {
            return EXIT_FAILURE;
        }

        break;
    }

    case OP::KEYGEN: {

        // verify argument count
        if ( argc != 4 ) {
            std::cerr << "ERROR: invalid argument count" << std::endl;
            print_help( argv[0] );
            return EXIT_FAILURE;
        }

        // get string pointers for arguments
        char const* const key_size = argv[2];
        char const* const key_file = argv[3];

        // convert argument from string to an unsigned integer
        unsigned int const key_size_int = boost::lexical_cast<unsigned int>( key_size );

        // TODO: update

        // verify user requested key length
        if ( key_size_int < 1 || key_size_int > 128 ) {
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
        std::cerr << "ERROR: Unknown operation type value (" << (int)operation.second << ")" << std::endl;
        return EXIT_FAILURE;
    }

    }

    return EXIT_SUCCESS;
}
