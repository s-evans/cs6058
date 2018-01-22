// Problem 4 (1 point).
//
// Assume we have a sequence of 100 characters, the frequency distribution of
// different characters is listed below, compute the (approximated) index of
// coincidence (IC) of this sequence.
//
// char       a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s   t   u   v   w   x  y  z
// frequency  1  1  1  1  1  1  1  1  1  1  5  5  5  5  5  5  5  5  10  10  10  10  10  0  0  0

// Solution
// IC = 0.071

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stdlib.h>
#include <vector>

int main( int argc, const char* argv[] )
{
    static const std::vector<unsigned char> histogram = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 10, 10, 10, 10, 10, 0, 0, 0
    };

    std::cout << "Computing Index of Coincidence" << std::endl;

    std::cout << "Histogram = ";
    std::copy( histogram.begin(), histogram.end() - 1, std::ostream_iterator<size_t>( std::cout, ", " ) );
    std::copy( histogram.end() - 1, histogram.end(), std::ostream_iterator<size_t>( std::cout ) );
    std::cout << std::endl;

    const auto N = std::accumulate( histogram.begin(), histogram.end(), ( size_t ) 0 );

    std::cout << "N = " << N << std::endl;

    const auto IC = std::accumulate( histogram.begin(), histogram.end(), ( double ) 0.0,
        [&]( const auto & current_sum, const double & current_value ) {
            const auto p = ( current_value / N );
            const auto pp = p * p;
            return current_sum + pp;
        }
    );

    std::cout << "IC = " << IC << std::endl;

    return EXIT_SUCCESS;
}
