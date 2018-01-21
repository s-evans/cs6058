// Problem 1 (1 point).
// Given a ciphertext JSSXFEPP encrypted by Shift Cipher,
// compute the key of shift cipher and the original message using brute-force
// attacks. In this problem, we assume the original message “makes sense” and is
// human-readable. The message space includes all the lower case characters, key.e.,
// M = { a , b , ..., z } , key space is K = { 0 , 1 , ..., 25 } and ciphertext
// space is C = { A , B , ..., Z } .

// Solution
// key = 4
// message = FOOTBALL

#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <string>

int main( int argc, const char *argv[] )
{
    const std::string cipher_text = "JSSXFEPP";

    std::cout << "brute forcing cipher text = '" << cipher_text << "' using shift cipher" << std::endl;

    for ( unsigned char key = 0  ; key < 26 ; ++key ) {
        std::string candidate_plain_text = cipher_text;

        std::cout << "trying key = '" << (int)key << "'" << std::endl;

        std::transform(
            cipher_text.begin(),
            cipher_text.end(),
            candidate_plain_text.begin(),
            [&]( auto const& cipher ){ return cipher - key; }
        );

        std::cout << "candidate plaintext = '" << candidate_plain_text << "'" << std::endl;

        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
