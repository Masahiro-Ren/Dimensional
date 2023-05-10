#include "./dimensional/dimensional.h"
#include <iostream>

int main()
{
    Dimensional<int, 2> d {
                                {1, 2, 3},
                                {4, 5, 6},
                                {7, 8, 9},
                            };

    std::cout << d(1, 2) << std::endl;
    d(1, 2) = 11;
    std::cout << d(1, 2) << std::endl;

    auto dr = d(1, slice(0, 2));

    //std::cout << dr(1, 1) << std::endl;
    //dr(1, 1) = 188;

    std::cout << dr(0, 0) << std::endl;
    std::cout << dr(0, 1) << std::endl;

    std::cout << d(1, 1) << std::endl;

    return 0;
}