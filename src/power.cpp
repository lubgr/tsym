
#include "power.h"
#include <cassert>
#include <cmath>
#include <limits>
#include "basefct.h"
#include "bplist.h"
#include "logarithm.h"
#include "logging.h"
#include "numberfct.h"
#include "numeric.h"
#include "powernormal.h"
#include "powersimpl.h"
#include "product.h"
#include "sum.h"
#include "undefined.h"

tsym::Power::Power(const BasePtr& base, const BasePtr& exponent, Base::CtorKey&&)
    : Base({base, exponent})
    , baseRef(ops.front())
    , expRef(ops.back())
{
    assert(ops.size() == 2);

    setDebugString();
}

tsym::BasePtr tsym::Power::create(const BasePtr& base, const BasePtr& exponent)
{
    if (exponent->isUndefined() || base->isUndefined())
        return Undefined::create();

    return createNotUndefined(base, exponent);
}

tsym::BasePtr tsym::Power::oneOver(const BasePtr& base)
{
    return create(base, Numeric::mOne());
}

tsym::BasePtr tsym::Power::sqrt(const BasePtr& base)
{
    return create(base, Numeric::half());
}

tsym::BasePtr tsym::Power::createNotUndefined(const BasePtr& base, const BasePtr& exponent)
{
    /* Handle trivial cases first. */
    if (isZero(*exponent) || isOne(*base))
        return Numeric::one();
    else if (isZero(*base) && exponent->isNegative()) {
        TSYM_WARNING("Division by zero during Power creation!");
        return Undefined::create();
    } else if (isZero(*base))
        return Numeric::zero();
    else if (isOne(*exponent))
        return base;

    return createNonTrivial(base, exponent);
}

tsym::BasePtr tsym::Power::createNonTrivial(const BasePtr& base, const BasePtr& exponent)
{
    const auto res = powersimpl::simplify(base, exponent);

    if (res.size() != 2) {
        TSYM_ERROR("Obtained wrong list from powersimpl: %S. Return Undefined", res);
        return Undefined::create();
    }

    /* Handle trivial cases again. */
    if (isOne(*res.back()))
        return res.front();
    else if (isOne(*res.front()))
        /* Will probably never be the case, just a security check. */
        return Numeric::one();

    return std::make_shared<const Power>(res.front(), res.back(), Base::CtorKey{});
}

bool tsym::Power::isEqualDifferentBase(const Base& other) const
{
    return isEqualByTypeAndOperands(other);
}

bool tsym::Power::sameType(const Base& other) const
{
    return other.isPower();
}

std::optional<tsym::Number> tsym::Power::numericEval() const
{
    const auto nExp = expRef->numericEval();
    const auto res = baseRef->numericEval();

    if (nExp && res)
        return res->toThe(*nExp);

    return std::nullopt;
}

tsym::Fraction tsym::Power::normal(SymbolMap& map) const
{
    PowerNormal pn(map);

    pn.setBase(*baseRef);
    pn.setExponent(*expRef);

    return pn.normal();
}

tsym::BasePtr tsym::Power::diffWrtSymbol(const Base& symbol) const
{
    const BasePtrList summands{Product::create(Logarithm::create(baseRef), expRef->diffWrtSymbol(symbol)),
      Product::create(expRef, oneOver(baseRef), baseRef->diffWrtSymbol(symbol))};

    return Product::create(clone(), Sum::create(summands));
}

std::string tsym::Power::typeStr() const
{
    return "Power";
}

bool tsym::Power::isPositive() const
{
    if (baseRef->isPositive())
        return true;
    else if (const auto num = expRef->numericEval())
        return num->isRational() && num->numerator() % 2 == 0;
    else
        return false;
}

bool tsym::Power::isNegative() const
{
    /* Currently, negative powers are always resolved as e.g. (-a)^(1/3) = (-1)*a^(1/3) for product
     * bases or (-2)^(1/3) = (-1)*2^(1/3) for numeric powers. */
    return false;
}

size_t tsym::Power::hash() const
{
    return std::hash<BasePtrList>{}(ops);
}

unsigned tsym::Power::complexity() const
{
    return 5 + baseRef->complexity() + 2 * expRef->complexity();
}

bool tsym::Power::isPower() const
{
    return true;
}

tsym::BasePtr tsym::Power::expand() const
{
    if (isInteger(*expRef))
        return expandIntegerExponent();
    else
        return clone();
}

bool tsym::Power::isInteger(const Base& arg) const
{
    if (const auto num = arg.numericEval())
        return isInt(*num);

    return false;
}

tsym::BasePtr tsym::Power::expandIntegerExponent() const
{
    if (baseRef->isSum())
        return expandSumBaseIntExp();
    else if (baseRef->isProduct())
        /* Should have been resolved during standard product simplification. */
        TSYM_ERROR("Illegal power expression, base: %S, exponent: %S.", baseRef, expRef);

    return Power::create(baseRef, expRef);
}

tsym::BasePtr tsym::Power::expandSumBaseIntExp() const
{
    const Int nExp(expRef->numericEval()->numerator());
    BasePtrList sums;

    for (Int i(0); i < integer::abs(nExp); ++i)
        sums.push_back(baseRef);

    auto res = bplist::expandAsProduct(sums);

    if (nExp < 0)
        res = Power::oneOver(res);

    return res;
}

tsym::BasePtr tsym::Power::subst(const Base& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return create(baseRef->subst(from, to), expRef->subst(from, to));
}

tsym::BasePtr tsym::Power::coeff(const Base& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (isEqual(*create(variable.clone(), Numeric::create(exp))))
        /* We won't get here for an exp < 2, otherwise *this wasn't a Power. */
        return Numeric::one();
    else if (!baseRef->isEqual(variable))
        return exp == 0 ? clone() : Numeric::zero();
    else
        return Numeric::zero();
}

int tsym::Power::degree(const Base& variable) const
{
    Int nExp;

    if (isEqual(variable))
        return 1;
    else if (isInteger(*expRef))
        nExp = expRef->numericEval()->numerator();

    const int baseDegree = baseRef->degree(variable);

    if (baseDegree == 0)
        return 0;
    else if (integer::fitsInto<int>(nExp)
      && static_cast<double>(nExp) < std::numeric_limits<int>::max() / static_cast<double>(baseDegree)
      && static_cast<double>(nExp) > std::numeric_limits<int>::min() / static_cast<double>(baseDegree))
        return static_cast<int>(nExp) * baseDegree;

    TSYM_ERROR("Degree of %S doens't fit into a primitive integer! Return 0 as degree.", clone());

    return 0;
}

tsym::BasePtr tsym::Power::base() const
{
    return baseRef;
}

tsym::BasePtr tsym::Power::exp() const
{
    return expRef;
}
