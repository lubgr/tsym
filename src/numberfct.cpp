
#include "numberfct.h"
#include <cassert>
#include <cmath>
#include <limits>
#include "number.h"

bool tsym::isInt(const Number& n)
{
    if (n.numerator() != 0 && n.denominator() == 1)
        return true;
    else if (n.numerator() == 0 && std::abs(n.toDouble()) < std::numeric_limits<double>::epsilon())
        return true;

    return false;
}

bool tsym::isFraction(const Number& n)
{
    return n.denominator() != 1;
}

bool tsym::isRational(const Number& n)
{
    return isFraction(n) || isInt(n);
}

bool tsym::isDouble(const Number& n)
{
    if (isRational(n))
        return false;

    assert(std::abs(n.toDouble()) > std::numeric_limits<double>::epsilon());

    return true;
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
