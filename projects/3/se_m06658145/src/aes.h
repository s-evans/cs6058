#ifndef AES_HPP
#define AES_HPP

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <vector>

class aes
{

public:

    aes() = delete;

    inline aes( EVP_CIPHER const* const mode, unsigned char const* const key, unsigned char const* const iv )
        : e_ctx()
        , d_ctx()
        , _key( key )
        , _iv( iv )
        , _mode( mode )
    {
        EVP_CIPHER_CTX_init( &e_ctx );
        EVP_CIPHER_CTX_init( &d_ctx );
    }

    inline ~aes()
    {
        if ( EVP_CIPHER_CTX_cleanup( &e_ctx ) != 1 ) { }
        if ( EVP_CIPHER_CTX_cleanup( &d_ctx ) != 1 ) { }
    }

    aes( aes const& ) = delete;

    aes& operator=( aes const& ) = delete;

    aes( aes&& ) = delete;

    aes& operator=( aes&& ) = delete;

    inline std::vector<unsigned char> decrypt( unsigned char const* const ciphertext, int const ciphertext_len )
    {

        if ( EVP_DecryptInit_ex( &d_ctx, _mode, NULL, _key, _iv ) != 1 ) {
            throw "EVP_DecryptInit_ex() failed";
        }

        EVP_CIPHER_CTX_set_padding( &d_ctx, 0 );

        std::vector<unsigned char> plaintext( ciphertext_len );

        int len;

        EVP_DecryptUpdate( &d_ctx, plaintext.data(), &len, ciphertext, ciphertext_len );

        int plaintext_len = len;

        if ( EVP_DecryptFinal_ex( &d_ctx, plaintext.data() + len, &len ) != 1 ) {
            throw "EVP_DecryptFinal_ex() failed";
        }

        plaintext_len += len;

        plaintext.resize( plaintext_len );

        return plaintext;
    }

    inline std::vector<unsigned char> encrypt( unsigned char const* const plaintext, int const len )
    {

        if ( EVP_EncryptInit_ex( &e_ctx, _mode, NULL, _key, _iv ) != 1 ) {
            throw "EVP_EncryptInit_ex() failed";
        }

        int c_len = len + AES_BLOCK_SIZE;

        std::vector<unsigned char> ciphertext( c_len );

        EVP_EncryptUpdate( &e_ctx, ciphertext.data(), &c_len, plaintext, len );

        int f_len = 0;

        if ( EVP_EncryptFinal_ex( &e_ctx, ciphertext.data() + c_len, &f_len ) != 1 ) {
            throw "EVP_EncryptFinal_ex() failed";
        }

        ciphertext.resize( c_len + f_len );

        return ciphertext;
    }

private:
    EVP_CIPHER_CTX e_ctx;
    EVP_CIPHER_CTX d_ctx;
    unsigned char const* const _key;
    unsigned char const* const _iv;
    EVP_CIPHER const* const _mode;

};

#endif // AES_HPP
