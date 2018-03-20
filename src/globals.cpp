
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
    return Var(Logarithm::create(arg.get()));
}

tsym::Var tsym::sin(const Var& arg)
{
    return Var(Trigonometric::createSin(arg.get()));
}

tsym::Var tsym::cos(const Var& arg)
{
    return Var(Trigonometric::createCos(arg.get()));
}

tsym::Var tsym::tan(const Var& arg)
{
    return Var(Trigonometric::createTan(arg.get()));
}

tsym::Var tsym::asin(const Var& arg)
{
    return Var(Trigonometric::createAsin(arg.get()));
}

tsym::Var tsym::acos(const Var& arg)
{
    return Var(Trigonometric::createAcos(arg.get()));
}

tsym::Var tsym::atan(const Var& arg)
{
    return Var(Trigonometric::createAtan(arg.get()));
}

tsym::Var tsym::atan2(const Var& y, const Var& x)
{
    return Var(Trigonometric::createAtan2(y.get(), x.get()));
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
