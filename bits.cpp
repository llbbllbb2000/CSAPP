#include <iostream>

int main()
{
    int a = -2;
    std::cout << (a >> 1) << std::endl;  //arithmetic shift

    a = -1;
    std::cout << (a >> 1) << std::endl;  //arithmetic shift and it cannot get 2147483647

    a = 2147483647;
    std::cout << (a >> 1) << std::endl;
    std::cout << (a + 1) << std::endl;

    a = -2147483648;
    // -2^(w - 1) + -2^(w - 1) = -2^w, which will cause negative overflow
    // hence TMin_w + TMin_w = 0 (in two's-complement, w bits)
    std::cout << (-a) << std::endl;

    unsigned b = -1;
    std::cout << b << std::endl;
    std::cout << (b >> 1) << std::endl;  // logical shift

    std::cout << ((int) +1e20) << std::endl;
    std::cout << ((int) -1e20) << std::endl;
}