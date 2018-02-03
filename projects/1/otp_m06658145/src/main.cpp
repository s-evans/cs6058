#include <stdlib.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>


// Define parameters
namespace PARAM
{

// Define supported operations
namespace OP
{

static const constexpr char* ENCRYPT = "enc";
static const constexpr char* DECRYPT = "dec";
static const constexpr char* KEYGEN  = "keygen";

} /* namespace OP */

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

static bool write_file ( const char* path, std::vector<char> const& data )
{
    // open file
    std::ofstream os( path, std::ofstream::out | std::ofstream::binary );

    // verify file was opened successfully
    if ( !os.is_open() ) {
        std::cerr << "ERROR: failed to open file '" << path << "'\n";
        return false;
    }

    // write to file
    os.write( data.data(), data.size() );

    // verify write was successful
    if ( !os ) {
        std::cerr << "ERROR: failed to write to file '" << path << "'\n";
        return false;
    }

    // close the file
    os.close();

    return true;
}

static std::pair<bool, std::vector<char>> read_file ( const char* path )
{
    // open file
    std::ifstream is( path, std::ifstream::in | std::ifstream::binary );

    // verify file was opened successfully
    if ( !is.is_open() ) {
        std::cerr << "ERROR: failed to open file '" << path << "'\n";
        return std::make_pair( false, std::vector<char>{} );
    }

    // get size of file
    is.seekg( 0, is.end );
    const size_t file_size = is.tellg();
    is.seekg( 0, is.beg );

    // read from file
    std::vector<char> data( file_size );
    is.read( data.data(), file_size );

    // verify read was successful
    if ( !is ) {
        std::cerr << "ERROR: failed to read from file '" << path << "'\n";
        return std::make_pair( false, std::vector<char>{} );
    }

    // close the file
    is.close();

    return std::make_pair( true, std::move( data ) );
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

    if ( op.compare( "--help" ) == 0 || op.compare( "-h" ) == 0 ) {

        print_help( argv[0] );

    } else if ( op.compare( PARAM::OP::DECRYPT ) == 0 ) {

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

        // read key data from file
        const auto key_file_data = read_file( key_file );

        // verify read was successful
        if ( !key_file_data.first ) {
            return EXIT_FAILURE;
        }

        // read ciphertext data from file
        const auto ciphertext_file_data = read_file( ciphertext_file );

        // verify read was successful
        if ( !ciphertext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // verify sizes of ciphertext and key are the same
        if ( ciphertext_file_data.second.size() != key_file_data.second.size() ) {
            std::cerr << "ERROR: ciphertext and key file sizes do not match ( "
                << ciphertext_file_data.second.size() << " != " << key_file_data.second.size() << " )\n";
            return EXIT_FAILURE;
        }

        std::vector<char> plaintext;

        // TODO: do decryption

        // write to output file
        if ( !write_file( plaintext_file, plaintext ) ) {
            return EXIT_FAILURE;
        }

    } else if ( op.compare( PARAM::OP::ENCRYPT ) == 0 ) {

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

        // read key data from file
        const auto key_file_data = read_file( key_file );

        // verify read was successful
        if ( !key_file_data.first ) {
            return EXIT_FAILURE;
        }

        // read plaintext data from file
        const auto plaintext_file_data = read_file( plaintext_file );

        // verify read was successful
        if ( !plaintext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // verify sizes of plaintext and key match
        if ( plaintext_file_data.second.size() != key_file_data.second.size() ) {
            std::cerr << "ERROR: plaintext and key file sizes do not match ( "
                << plaintext_file_data.second.size() << " != " << key_file_data.second.size() << " )\n";
            return EXIT_FAILURE;
        }

        std::vector<char> ciphertext;

        // TODO: do encryption

        // write to output file
        if ( !write_file( ciphertext_file, ciphertext ) ) {
            return EXIT_FAILURE;
        }

    } else if ( op.compare( PARAM::OP::KEYGEN ) == 0 ) {

        // verify argument count
        if ( argc != 4 ) {
            std::cerr << "ERROR: invalid argument count" << std::endl;
            print_help( argv[0] );
            return EXIT_FAILURE;
        }

        // get string pointers for arguments
        const char* const key_size = argv[2];
        const char* const key_file = argv[3];

        // convert arguments
        const unsigned int key_size_int = boost::lexical_cast<unsigned int>( key_size );

        std::vector<char> key_data( key_size_int );

        // TODO: do key generation

        // write to output file
        if ( !write_file( key_file, key_data ) ) {
            return EXIT_FAILURE;
        }

    } else {
        // no known operation was specified
        std::cerr << "ERROR: unknown operation '" << op << "' specified" << std::endl;
        print_help( argv[0] );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
