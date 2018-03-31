#ifndef KEYGEN_H
#define KEYGEN_H

#include <vector>

/**
 * @brief generate a random key
 *
 * @param size size of key in bytes
 *
 * @return vector of random bytes to be used as a key
 */
std::vector<unsigned char> keygen( unsigned int size );

#endif // KEYGEN_H
