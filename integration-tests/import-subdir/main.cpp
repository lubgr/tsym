
#include <iostream>
#include "tsym/tsym.h"

int main(int, char**)
{
    const tsym::Var a("a");
    const tsym::Var b("b");
    const auto c = tsym::cos(a + b) + tsym::Var(3, 4);

    std::cout << c << std::endl;

    return 0;
}
