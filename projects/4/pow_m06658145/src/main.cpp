#include "generate_solution.h"
#include "target_generation.h"
#include "verify_solution.h"
#include <boost/lexical_cast.hpp>
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

static std::string const TARGET   = "target";
static std::string const SOLUTION = "sol";
static std::string const VERIFY   = "verify";

} /* namespace OP */

} /* namespace PARAM */

// Define constants for supported operations
enum class OP {
    TARGET,
    SOLUTION,
    VERIFY
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
    if ( PARAM::OP::TARGET.compare( op ) == 0 ) {
        return std::make_pair( true, OP::TARGET );
    }

    if ( PARAM::OP::SOLUTION.compare( op ) == 0 ) {
        return std::make_pair( true, OP::SOLUTION );
    }

    if ( PARAM::OP::VERIFY.compare( op ) == 0 ) {
        return std::make_pair( true, OP::VERIFY );
    }

    std::cerr << "ERROR: unknown operation '" << op << "' specified" << std::endl;

    return std::make_pair( false, OP::TARGET );
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
    std::cerr << "\t" << exe << " target d <target_file_path>\n";
    std::cerr << "\t" << exe << " sol <input_file_path> <target_file_path> <solution_file_path>\n";
    std::cerr << "\t" << exe << " verify <input_File_path> <target_file_path> <solution_file_path>\n";

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

        case OP::TARGET: {

            if ( argc != 4 ) {
                std::cerr << "ERROR: invalid argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            char const* const difficulty       = argv[2];
            char const* const target_file_path = argv[3];

            unsigned int difficulty_int = boost::lexical_cast<unsigned int>( difficulty );

            return target_generation( difficulty_int, target_file_path );
        }

        case OP::SOLUTION: {

            if ( argc != 5 ) {
                std::cerr << "ERROR: invalid argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            char const* const input_file_path    = argv[2];
            char const* const target_file_path   = argv[3];
            char const* const solution_file_path = argv[4];

            return generate_solution( input_file_path, target_file_path, solution_file_path );
        }

        case OP::VERIFY: {

            if ( argc != 5 ) {
                std::cerr << "ERROR: invalid argument count" << std::endl;
                print_help( argv[0] );
                return EXIT_FAILURE;
            }

            char const* const input_file_path    = argv[2];
            char const* const target_file_path   = argv[3];
            char const* const solution_file_path = argv[4];

            return verify_solution( input_file_path, target_file_path, solution_file_path );
        }

        default: {
            std::cerr << "ERROR: Unknown operation type value (" << ( int )operation.second << ")" << std::endl;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
