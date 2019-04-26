
#include <iostream>
#include "tsym/tsym.h"

int main(int, char**)
{
    const auto a = tsym::Var("a");
    const auto fraction = tsym::Var(-3, 4);
    const auto b = a + fraction;

    std::cout << b << std::endl;

    return 0;
}
