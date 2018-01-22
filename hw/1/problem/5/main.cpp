// Problem 5 (1 point).
//
// An adversary analyzes a sequence of (ciphertext) characters, which is encrypted
// by Vigenere Cipher, using Kasishi’s method. In addition, it knows the key
// length is at least 2. If it can find a sub-string with a length of 4 repeated
// twice in the sequence, and the distance between the two repeated sub-strings is
// 12, what are the possible key length of this Vigenere cipher?

// Solution
// key length = gcd(4, 12) = 4

#include <iostream>
#include <stdlib.h>

template<class T> T gcd( T x, T y ) {
    while ( y != 0 ) {
        T t = y;
        y = x % y;
        x = t;
    }

    return x;
}

int main( int argc, const char* argv[] )
{
    std::cout << "key length = " << gcd( 4, 12 ) << std::endl;

    return EXIT_SUCCESS;
}
