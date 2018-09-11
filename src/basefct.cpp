
#include "basefct.h"
#include "base.h"
#include "number.h"
#include "numberfct.h"

bool tsym::isInteger(const Base& expr)
{
    if (const auto num = expr.numericEval())
        return isInt(*num);

    return false;
}

namespace tsym {
    namespace {
        template <int n> bool isEqual(const Base& expr)
        {
            static const Number i{n};

            return expr.numericEval() == i;
        }
    }
}

bool tsym::isOne(const Base& expr)
{
    return expr.isNumeric() && isEqual<1>(expr);
}

bool tsym::isZero(const Base& expr)
{
    return expr.isNumeric() && isEqual<0>(expr);
}

bool tsym::isNumericPower(const Base& expr)
{
    if (expr.isPower())
        return expr.base()->isNumeric() && expr.exp()->isNumeric();

    return false;
}
