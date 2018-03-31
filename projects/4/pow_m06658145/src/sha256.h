#ifndef SHA256_HPP
#define SHA256_HPP

#include <openssl/evp.h>
#include <array>

class sha256
{

public:

    inline sha256()
        : ctx()
    {
        if ( EVP_DigestInit_ex( &ctx, EVP_sha256(), nullptr ) != 1 ) {
            throw "EVP_DigestInit_ex() failed";
        }
    }

    inline ~sha256()
    {
        if ( EVP_MD_CTX_cleanup( &ctx ) != 1 ) { }
    }

    sha256( sha256 const& ) = delete;

    sha256& operator=( sha256 const& ) = delete;

    sha256( sha256&& ) = delete;

    sha256& operator=( sha256&& ) = delete;

    inline std::array<unsigned char, 16> hash( unsigned char const* const data, int const len )
    {
        if ( EVP_DigestUpdate( &ctx, data, len ) != 1 ) {
            throw "EVP_DigestUpdate() failed";
        }

        std::array<unsigned char, 16> hash;
        unsigned int hash_size = sizeof( hash );

        if ( EVP_DigestFinal_ex( &ctx, hash.data(), &hash_size ) != 1 ) {
            throw "EVP_DigestFinal_ex() failed";
        }

        if ( hash_size != sizeof( hash ) ) {
            throw "unexpected hash size";
        }

        return hash;
    }

private:

    EVP_MD_CTX ctx;

};

#endif // SHA256_HPP
