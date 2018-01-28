// Problem 3 (1 point).
//
// Assume the key of Vigenere Cipher is cats.
//
// What is the ciphertext of a message datasecurity encrypted by this key using
// Vigenere Cipher?
//
// What is the size of the key space for Vigenere Cipher if each key is a string
// of 4 characters?
//
// For easy calculation, a mapping table between characters (a, ..., z) and
// integers (0, ..., 25) is listed below.
//
// a  b  c  d  e  f  g  h  i  j  k   l   m   n   o   p   q   r   s   t   u   v   w   x   y   z
// 0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25

// Solution
// The ciphertext is 'famsuevmtimq'
// The key space for a Vigenere Cipher a 4 character string is 26 ^ 4 = 456976

#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <string>

template <size_t s> class Print;

int main( int argc, const char *argv[] )
{
    static const std::string key = "cats";

    static const std::string plain_text = "datasecurity";

    std::cout << "key is '" << key << "'" << std::endl;

    std::cout << "plain text is '" << plain_text << "'" << std::endl;

    std::string cipher_text = plain_text;

    size_t idx = 0;

    for ( size_t idx = 0 ; idx < plain_text.size() ; ++idx ) {
        cipher_text[idx] = 
            ( ( plain_text[idx] + ( key[idx % key.size()] - 'a' ) - 'a' ) % 26 ) + 'a';
    }

    std::cout << "cipher text is '" << cipher_text << "'" << std::endl;

    return EXIT_SUCCESS;
}
