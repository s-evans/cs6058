#ifndef AES_HPP
#define AES_HPP

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <vector>

class aes
{

public:

    aes() = delete;

    inline aes( EVP_CIPHER const* const mode, unsigned char const* const key_data, int const key_data_len, unsigned char const* const salt )
        : e_ctx()
        , d_ctx()
    {
        constexpr int const NROUNDS = 5;
        unsigned char iv[32];
        unsigned char key[32];

        // TODO: salt?
        // TODO: input IV?
        // TODO: keygen?
        int const i = EVP_BytesToKey( mode, EVP_sha1(), salt, key_data, key_data_len, NROUNDS, key, iv );

        if ( i != 32 ) {
            throw "EVP_BytesToKey() failed";
        }

        EVP_CIPHER_CTX_init( &e_ctx );

        if ( EVP_EncryptInit_ex( &e_ctx, mode, NULL, key, iv ) != 1 ) {
            EVP_CIPHER_CTX_cleanup( &e_ctx );
            throw "EVP_EncryptInit_ex() failed";
        }

        EVP_CIPHER_CTX_init( &d_ctx );

        if ( EVP_DecryptInit_ex( &d_ctx, mode, NULL, key, iv ) != 1 ) {
            EVP_CIPHER_CTX_cleanup( &e_ctx );
            EVP_CIPHER_CTX_cleanup( &d_ctx );
            throw "EVP_DecryptInit_ex() failed";
        }
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

    inline std::vector<unsigned char> decrypt( unsigned char const* const ciphertext, int const len )
    {

        if ( EVP_DecryptInit_ex( &d_ctx, NULL, NULL, NULL, NULL ) != 1 ) {
            throw "EVP_DecryptInit_ex() failed";
        }

        int p_len = len;
        int f_len = 0;
        std::vector<unsigned char> plaintext( p_len );

        EVP_DecryptUpdate( &d_ctx, plaintext.data(), &p_len, ciphertext, len );

        if ( EVP_DecryptFinal_ex( &d_ctx, plaintext.data() + p_len, &f_len ) != 1 ) {
            throw "EVP_DecryptFinal_ex() failed";
        }

        plaintext.resize( p_len + f_len );

        return plaintext;
    }

    inline std::vector<unsigned char> encrypt( unsigned char const* const plaintext, int const len )
    {

        if ( EVP_EncryptInit_ex( &e_ctx, NULL, NULL, NULL, NULL ) != 1 ) {
            throw "EVP_EncryptInit_ex() failed";
        }

        int c_len = len + AES_BLOCK_SIZE;
        int f_len = 0;
        std::vector<unsigned char> ciphertext( c_len );

        EVP_EncryptUpdate( &e_ctx, ciphertext.data(), &c_len, plaintext, len );

        if ( EVP_EncryptFinal_ex( &e_ctx, ciphertext.data() + c_len, &f_len ) != 1 ) {
            throw "EVP_EncryptFinal_ex() failed";
        }

        ciphertext.resize( c_len + f_len );

        return ciphertext;
    }

private:
    EVP_CIPHER_CTX e_ctx;
    EVP_CIPHER_CTX d_ctx;

};

#endif // AES_HPP
