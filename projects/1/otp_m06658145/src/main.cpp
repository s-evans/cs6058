#include <stdlib.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <string>
#include <vector>

// Group together data for a CLI parameter
struct Parameter {
    static constexpr Parameter MakeSimple( const char* key, const char* desc ) noexcept
    {
        return Parameter{key, key, desc};
    }

    static constexpr Parameter Make( const char* key, const char* opt, const char* desc ) noexcept
    {
        return Parameter{key, opt, desc};
    }

    const char* const key;  // key used in the variables_map
    const char* const opt;  // cli parameter text used in registering the parameter
    const char* const desc; // option description used in printing help
};

// Define the types of operations possible in this program
enum class OPERATION {
    ENCRYPT,
    DECRYPT,
    KEYGEN
};

// Group together data for the 'operation' CLI parameter
struct Operation {
    const char* const name; // name of the operation used on the CLI
    const OPERATION val;    // enum value associated with the name specified on the CLI
};

// Define parameters
namespace PARAM
{

// Define supported parameters
static const constexpr Parameter HELP =
    Parameter::Make( "help", "help,h", "produce help message" );

static const constexpr Parameter OPERATION =
    Parameter::MakeSimple( "operation", "specify the operation to be performed" );

static const constexpr Parameter KEY_FILE =
    Parameter::MakeSimple( "key-file", "specify the path to the key file" );

static const constexpr Parameter CIPHERTEXT_FILE =
    Parameter::MakeSimple( "ciphertext-file", "specify the path to the ciphertext file" );

static const constexpr Parameter PLAINTEXT_FILE =
    Parameter::MakeSimple( "plaintext-file", "specify the path to the plaintext file" );

static const constexpr Parameter KEY_SIZE =
    Parameter::MakeSimple( "key-size", "specify the size of the key" );

// Define supported operations
namespace OP
{
static const constexpr Operation ENCRYPT = { "enc",    OPERATION::ENCRYPT };
static const constexpr Operation DECRYPT = { "dec",    OPERATION::DECRYPT };
static const constexpr Operation KEYGEN  = { "keygen", OPERATION::KEYGEN  };
} /* namespace OP */

// Define supported operations in an iterable fashion
static const Operation OPERATIONS[] = {OP::ENCRYPT, OP::DECRYPT, OP::KEYGEN};

} /* namespace PARAM */

static void print_help( const char* const exe )
{
    std::cerr << "\n";
    std::cerr << "Overview:\n";
    std::cerr << "\tperforms key generation, encryption, and decryption using one time pad encryption\n";
    std::cerr << "\n";

    std::cerr << "Synopsis:\n";
    std::cerr << "\t" << exe << " (-h|--help)\n";
    std::cerr << "\t" << exe << " enc <key_file_path> <plaintext_file_path> <ciphertext_file_path>\n";
    std::cerr << "\t" << exe << " dec <key_file_path> <ciphertext_file_path> <plaintext_file_path>\n";
    std::cerr << "\t" << exe << " keygen <key_size> <key_file_path>\n";
}

int main( int argc, const char* argv[] )
{
    // check that the operation is specified
    if ( argc < 2 ) {
        std::cerr << "ERROR: insufficient argument count" << std::endl;
        print_help( argv[0] );
        return EXIT_FAILURE;
    }

    // get the operation
    std::string const op = argv[1];

    // check if help was requested
    if ( op.compare( "--help" ) == 0 || op.compare( "-h" ) == 0 ) {
        print_help( argv[0] );
        return EXIT_SUCCESS;
    }

    // check for supported operations
    for ( auto const& OP : PARAM::OPERATIONS ) {
        if ( op.compare( OP.name ) == 0 ) {
            switch ( OP.val ) {
                case OPERATION::DECRYPT: {

                    // verify argument count
                    if ( argc != 5 ) {
                        std::cerr << "ERROR: invalid argument count" << std::endl;
                        print_help( argv[0] );
                        return EXIT_FAILURE;
                    }

                    // get string pointers for arguments
                    const char* const key_file = argv[2];
                    const char* const plaintext_file = argv[3];
                    const char* const ciphertext_file = argv[4];

                    // convert
                    const boost::filesystem::path key_file_path = key_file;
                    const boost::filesystem::path ciphertext_file_path = ciphertext_file;
                    const boost::filesystem::path plaintext_file_path = plaintext_file;

                    // TODO: implement

                    break;
                }

                case OPERATION::ENCRYPT: {

                    // verify argument count
                    if ( argc != 5 ) {
                        std::cerr << "ERROR: invalid argument count" << std::endl;
                        print_help( argv[0] );
                        return EXIT_FAILURE;
                    }

                    // get string pointers for arguments
                    const char* const key_file = argv[2];
                    const char* const ciphertext_file = argv[3];
                    const char* const plaintext_file = argv[4];

                    // convert
                    const boost::filesystem::path key_file_path = key_file;
                    const boost::filesystem::path ciphertext_file_path = ciphertext_file;
                    const boost::filesystem::path plaintext_file_path = plaintext_file;

                    // TODO: implement

                    break;
                }

                case OPERATION::KEYGEN: {

                    // verify argument count
                    if ( argc != 4 ) {
                        std::cerr << "ERROR: invalid argument count" << std::endl;
                        print_help( argv[0] );
                        return EXIT_FAILURE;
                    }

                    // get string pointers for arguments
                    const char* const key_size = argv[2];
                    const char* const key_file = argv[3];

                    // convert
                    const unsigned int key_size_int = boost::lexical_cast<unsigned int>( key_size );
                    const boost::filesystem::path key_file_path = key_file;

                    // TODO: implement

                    break;
                }

                default:
                    print_help( argv[0] );
                    return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }
    }

    // no known operation was specified
    std::cerr << "ERROR: unknown operation '" << op << "' specified" << std::endl;
    print_help( argv[0] );

    return EXIT_FAILURE;
}
