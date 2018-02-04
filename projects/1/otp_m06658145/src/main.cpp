#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include "keygen.h"

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

static std::string encode( const std::vector<char>& val )
{
    // define a base 64 iterator type
    using b64_iterator =
        boost::archive::iterators::base64_from_binary<
            boost::archive::iterators::transform_width<
                std::vector<char>::const_iterator, 6, 8>>;

    // create a base64 encoded string from input binary data
    auto tmp = std::string( b64_iterator( std::begin( val ) ), b64_iterator( std::end( val ) ) );

    // append trailing equals signs for leftover bits
    return tmp.append( ( 3 - val.size() % 3 ) % 3, '=' );
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
        const char* const ciphertext_file = argv[3];
        const char* const plaintext_file = argv[4];

        // read key data from file
        const auto key_file_data = read_file( key_file );

        // verify read was successful
        if ( !key_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the key data
        const auto& key = key_file_data.second;

        // read ciphertext data from file
        const auto ciphertext_file_data = read_file( ciphertext_file );

        // verify read was successful
        if ( !ciphertext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the ciphertext data
        const auto& ciphertext = ciphertext_file_data.second;

        // verify sizes of ciphertext and key are the same
        if ( ciphertext.size() != key.size() ) {
            std::cerr << "ERROR: ciphertext and key file sizes do not match ( "
                << ciphertext.size() << " != " << key.size() << " )\n";
            return EXIT_FAILURE;
        }

        // copy the ciphertext data into the plaintext array
        std::vector<char> plaintext( ciphertext );

        // combine the ciphertext with the key to perform decryption and create the plaintext
        for ( size_t i = 0 ; i < plaintext.size() ; ++i ) {
            plaintext[i] ^= key[i];
        }

        // write plaintext data to output file
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
        const char* const plaintext_file = argv[3];
        const char* const ciphertext_file = argv[4];

        // read key data from file
        const auto key_file_data = read_file( key_file );

        // verify read was successful
        if ( !key_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the key data
        const auto& key = key_file_data.second;

        // read plaintext data from file
        const auto plaintext_file_data = read_file( plaintext_file );

        // verify read was successful
        if ( !plaintext_file_data.first ) {
            return EXIT_FAILURE;
        }

        // create an alias for the plaintext data
        const auto& plaintext = plaintext_file_data.second;

        // verify sizes of plaintext and key match
        if ( plaintext.size() != key.size() ) {
            std::cerr << "ERROR: plaintext and key file sizes do not match ( "
                << plaintext.size() << " != " << key.size() << " )\n";
            return EXIT_FAILURE;
        }

        // copy the plaintext into the ciphertext
        std::vector<char> ciphertext( plaintext );

        // combine the plaintext with the key to create the ciphertext
        for ( size_t i = 0 ; i < ciphertext.size() ; ++i ) {
            ciphertext[i] ^= key[i];
        }

        // write ciphertext to output file
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

        // convert argument from string to an unsigned integer
        const unsigned int key_size_int = boost::lexical_cast<unsigned int>( key_size );

        // verify user requested key length
        if ( key_size_int < 1 || key_size_int > 128 ) {
            std::cerr << "ERROR: invalid key size specified\n";
            return EXIT_FAILURE;
        }

        // get the size of the key in bytes
        const unsigned int key_bytes = ( key_size_int + 8 - 1 ) / 8;

        // generate a key
        const auto key_data { keygen( key_bytes ) };

        // convert binary key data to base64 and print to the terminal
        std::cout << "key = " << encode( key_data ) << "\n";

        // write key data to output file
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
