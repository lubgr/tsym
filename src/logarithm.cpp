
#include "logarithm.h"
#include <cmath>
#include "constant.h"
#include "fraction.h"
#include "logging.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbolmap.h"
#include "undefined.h"

tsym::Logarithm::Logarithm(const BasePtr& arg, Base::CtorKey&&)
    : Function({arg}, "log")
    , arg(ops.front())
{
    setDebugString();
}

tsym::BasePtr tsym::Logarithm::create(const BasePtr& arg)
{
    if (isInvalidArg(*arg))
        return Undefined::create();
    else if (arg->isOne())
        return Numeric::zero();
    else if (arg->isNumeric())
        return createNumerically(arg);
    else if (arg->isConstant())
        return createFromConstant(arg);
    else if (arg->isPower())
        return createFromPower(arg);
    else
        return createInstance(arg);
}

tsym::BasePtr tsym::Logarithm::createInstance(const BasePtr& arg)
{
    return std::make_shared<const Logarithm>(arg, Base::CtorKey{});
}

bool tsym::Logarithm::isInvalidArg(const Base& arg)
{
    bool invalid = false;

    if (arg.isUndefined())
        invalid = true;
    else if (arg.isZero())
        invalid = true;
    else if (arg.isPower() && arg.base()->isEqual(*Constant::createE()))
        /* Catches log(e^(-n)) with n being a large Numeric, which would numerically be evaluated to
         * zero and thus resulting in an Undefined return value. */
        ;
    else if (arg.isNegative())
        invalid = true;
    else if (arg.isNumericallyEvaluable())
        invalid = arg.numericEval() == 0;

    if (invalid)
        TSYM_WARNING("Logarithm: invalid argument %S", arg);

    return invalid;
}

tsym::BasePtr tsym::Logarithm::createNumerically(const BasePtr& arg)
{
    const Number nArg = *arg->numericEval();

    assert(nArg != 0 && nArg != 1);

    if (nArg.isRational())
        return createInstance(arg);
    else
        return Numeric::create(std::log(nArg.toDouble()));
}

tsym::BasePtr tsym::Logarithm::createFromConstant(const BasePtr& arg)
{
    if (arg->isEqual(*Constant::createE()))
        return Numeric::one();
    else
        return createInstance(arg);
}

tsym::BasePtr tsym::Logarithm::createFromPower(const BasePtr& arg)
{
    return Product::create(arg->exp(), create(arg->base()));
}

std::optional<tsym::Number> tsym::Logarithm::numericEval() const
{
    if (const auto nArg = arg->numericEval())
        return std::log(nArg->toDouble());
    else
        return std::nullopt;
}

tsym::Fraction tsym::Logarithm::normal(SymbolMap& map) const
{
    const BasePtr result(create(arg->normal()));
    const BasePtr replacement(map.getTmpSymbolAndStore(result));

    return Fraction{replacement};
}

tsym::BasePtr tsym::Logarithm::diffWrtSymbol(const Base& symbol) const
{
    return Product::create(Power::oneOver(arg), arg->diffWrtSymbol(symbol));
}

tsym::BasePtr tsym::Logarithm::subst(const Base& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return create(arg->subst(from, to));
}

bool tsym::Logarithm::isPositive() const
{
    return checkSign(&Base::isPositive);
}

bool tsym::Logarithm::checkSign(bool (Base::*method)() const) const
{
    const BasePtr argMinusOne(Sum::create(arg, Numeric::mOne()));

    return ((*argMinusOne).*method)();
}

bool tsym::Logarithm::isNegative() const
{
    return checkSign(&Base::isNegative);
}

unsigned tsym::Logarithm::complexity() const
{
    return 6 + arg->complexity();
}
