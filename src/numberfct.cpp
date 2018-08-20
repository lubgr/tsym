
#include "numberfct.h"
#include <cassert>
#include <cmath>
#include <limits>
#include "number.h"

bool tsym::isInt(const Number& n)
{
    return n.isRational() && n.denominator() == 1;
}

bool tsym::isFraction(const Number& n)
{
    return n.denominator() != 1;
}

tsym::Number tsym::abs(const Number& n)
{
    return n < 0 ? -n : n;
}

int tsym::sign(const Number& n)
{
    if (n < 0)
        return -1;
    else if (n > 0)
        return 1;

    assert(n == 0);

    return 0;
}
