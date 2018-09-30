
#include <iostream>
#include "tsym/all.h"

int main(int, char**)
{
    const auto sqrtTwo = tsym::sqrt(2);
    const auto atan2 = tsym::atan2(3, 7);

    std::cout << atan2 << std::endl;

    return 0;
}
