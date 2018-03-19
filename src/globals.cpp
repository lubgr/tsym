
#include "globals.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "constant.h"
#include "parser.h"
#include "logging.h"

tsym::Var tsym::sqrt(const Var& base)
{
    const Var half(1, 2);

    return pow(base, half);
}

tsym::Var tsym::pow(const Var& base, const Var& exp)
{
    return base.toThe(exp);
}

tsym::Var tsym::log(const Var& arg)
{
    return Var(Logarithm::create(arg.getBasePtr()));
}

tsym::Var tsym::sin(const Var& arg)
{
    return Var(Trigonometric::createSin(arg.getBasePtr()));
}

tsym::Var tsym::cos(const Var& arg)
{
    return Var(Trigonometric::createCos(arg.getBasePtr()));
}

tsym::Var tsym::tan(const Var& arg)
{
    return Var(Trigonometric::createTan(arg.getBasePtr()));
}

tsym::Var tsym::asin(const Var& arg)
{
    return Var(Trigonometric::createAsin(arg.getBasePtr()));
}

tsym::Var tsym::acos(const Var& arg)
{
    return Var(Trigonometric::createAcos(arg.getBasePtr()));
}

tsym::Var tsym::atan(const Var& arg)
{
    return Var(Trigonometric::createAtan(arg.getBasePtr()));
}

tsym::Var tsym::atan2(const Var& y, const Var& x)
{
    return Var(Trigonometric::createAtan2(y.getBasePtr(), x.getBasePtr()));
}

const tsym::Var& tsym::pi()
{
    static const tsym::Var instance(Constant::createPi());

    return instance;
}

const tsym::Var& tsym::euler()
{
    static const tsym::Var instance(Constant::createE());

    return instance;
}

tsym::Var tsym::parse(const std::string& str, bool *success)
{
    parser::Result result = parser::parse(str);

    TSYM_DEBUG("Parsed '%s' with result: %S", str.c_str(), result.value);

    if (success != nullptr)
        *success = result.success && result.matchedWholeString;

    return Var(result.value);
}
