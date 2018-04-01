#include "add_token_to_file.h"
#include "encrypt_directory.h"
#include "keygen_to_file.h"
#include "search_token.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <utility>

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
    std::cerr << "\tperforms searchable encryption on files and provides encrypted file search functionality\n";
    std::cerr << "\n";

    std::cerr << "Synopsis:\n";
    std::cerr << "\t" << exe << " (-h|--help)\n";
    std::cerr << "\t" << exe << " keygen <prf_key_file_path> <aes_key_file_path>\n";
    std::cerr << "\t" << exe << " enc <prf_key_file_path> <aes_key_file_path> <index_file_path> <plaintext_dir_path> <ciphertext_dir_path>\n";
    std::cerr << "\t" << exe << " token <keyword> <prf_key_file_path> <token_file_path>\n";
    std::cerr << "\t" << exe << " search <index_file_path> <token_file_path> <ciphertext_dir_path> <aes_key_file_path>\n";

    std::cerr << std::flush;
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
            char const* const prf_key_file = argv[2];
            char const* const aes_key_file = argv[3];

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

            return add_token_to_file( token_keyword, prf_key_file, token_file, std::cout );
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

            return search_token( index_file, token_file, ciphertext_dir, aes_key_file, std::cout );
        }

        default: {
            std::cerr << "ERROR: Unknown operation type value (" << ( int )operation.second << ")" << std::endl;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
