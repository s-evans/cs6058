// Problem 2 (1 point).
// Given an encryption key (i.e., a permutation) of Substitution Cipher presented
// below, compute the ciphertext of a message universityofcincinnati.
//
// a b c d e f g h i j k l m n o p q r s t u v w x y z
// E X A U N D K B M V O R Q C S F H Y G W Z L J I T P
//
// If the message space of Substitution Cipher has a number of 50 unique characters/symbols, what is the
// size of the key space? In other words, how many permutations in total?

// Solution
// The cipher text is 'ZCMLNYGMWTSDAMCAMCCEWM'.
// The key space for a message space of 50 is 50! (50 factorial).

#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <string>

int main( int argc, const char *argv[] )
{
    static const char* substitution_cipher = "EXAUNDKBMVORQCSFHYGWZLJITP";

    std::cout << "substitution cipher is '" << substitution_cipher << "'" << std::endl;

    static const std::string plain_text = "universityofcincinnati";

    std::cout << "plain text is '" << plain_text << "'" << std::endl;

    std::string cipher_text = plain_text;

    std::transform(
        plain_text.begin(),
        plain_text.end(),
        cipher_text.begin(),
        [&]( auto const& v ){ return substitution_cipher[v - 'a']; }
    );

    std::cout << "cipher text is '" << cipher_text << "'" << std::endl;

    return EXIT_SUCCESS;
}
