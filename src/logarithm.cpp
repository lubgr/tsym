
#include <cmath>
#include "logarithm.h"
#include "undefined.h"
#include "constant.h"
#include "sum.h"
#include "product.h"
#include "power.h"
#include "numeric.h"
#include "symbolmap.h"
#include "logging.h"

namespace tsym {
    namespace {
        const BasePtr& euler()
        {
            static const BasePtr e(Constant::createE());

            return e;
        }
    }
}

tsym::Logarithm::Logarithm(const BasePtr& arg) :
    Function(BasePtrList(arg), "log"),
    arg(ops.front())
{}

tsym::Logarithm::~Logarithm() {}

tsym::BasePtr tsym::Logarithm::create(const BasePtr& arg)
{
    if (isInvalidArg(arg))
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
        return BasePtr(new Logarithm(arg));
}

bool tsym::Logarithm::isInvalidArg(const BasePtr& arg)
{
    bool invalid = false;

    if (arg->isUndefined())
        invalid = true;
    else if(arg->isZero())
        invalid = true;
    else if (arg->isPower() && arg->base()->isEqual(euler()))
        /* Catches log(e^(-n)) with n being a large Numeric, which would numerically be evaluated to
         * zero and thus resulting in an Undefined return value. */
        ;
    else if (arg->isNegative())
        invalid = true;
    else if (arg->isNumericallyEvaluable())
        invalid = arg->numericEval().isZero();

    if (invalid)
        TSYM_WARNING("Logarithm: invalid argument ", arg);

    return invalid;
}

tsym::BasePtr tsym::Logarithm::createNumerically(const BasePtr& arg)
{
    const Number nArg(arg->numericEval());

    assert(nArg != 0 && nArg != 1 && !nArg.isUndefined());

    if (nArg.isRational())
        return BasePtr(new Logarithm(arg));
    else
        return Numeric::create(std::log(nArg.toDouble()));
}

tsym::BasePtr tsym::Logarithm::createFromConstant(const BasePtr& arg)
{
    if (arg->isEqual(euler()))
        return Numeric::one();
    else
        return BasePtr(new Logarithm(arg));
}

tsym::BasePtr tsym::Logarithm::createFromPower(const BasePtr& arg)
{
    return Product::create(arg->exp(), create(arg->base()));
}

tsym::Number tsym::Logarithm::numericEval() const
{
    if (!isNumericallyEvaluable())
        return Number::createUndefined();
    else
        return checkedNumericEval();
}

tsym::Number tsym::Logarithm::checkedNumericEval() const
{
    const double result = std::log(arg->numericEval().toDouble());

    return Number(result);
}

tsym::Fraction tsym::Logarithm::normal(SymbolMap& map) const
{
    const BasePtr result(create(arg->normal()));
    const BasePtr replacement(map.getTmpSymbolAndStore(result));

    return Fraction(replacement);
}

tsym::BasePtr tsym::Logarithm::diffWrtSymbol(const BasePtr& symbol) const
{
    return Product::create(Power::oneOver(arg), arg->diffWrtSymbol(symbol));
}

tsym::BasePtr tsym::Logarithm::subst(const BasePtr& from, const BasePtr& to) const
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
