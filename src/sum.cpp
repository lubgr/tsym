
#include "sum.h"
#include <boost/range/numeric.hpp>
#include <cassert>
#include <limits>
#include "basefct.h"
#include "baseptrlistfct.h"
#include "basetypestr.h"
#include "fraction.h"
#include "numberfct.h"
#include "numeric.h"
#include "poly.h"
#include "power.h"
#include "product.h"
#include "sumsimpl.h"
#include "undefined.h"

tsym::Sum::Sum(const BasePtrList& summands, Base::CtorKey&&)
    : Base(typestring::sum, summands)
{
    setDebugString();
}

tsym::BasePtr tsym::Sum::create(const BasePtrList& summands)
{
    if (hasUndefinedElements(summands))
        return Undefined::create();
    else if (summands.size() == 1)
        return summands.front();
    else
        return createSimplifiedSum(summands);
}

tsym::BasePtr tsym::Sum::createSimplifiedSum(const BasePtrList& summands)
{
    const auto res = simplifySum(summands);

    if (res.empty())
        return Numeric::zero();
    else if (res.size() == 1)
        return res.front();
    else
        return std::make_shared<const Sum>(res, Base::CtorKey{});
}

bool tsym::Sum::isEqualDifferentBase(const Base& other) const
{
    return isEqualByTypeAndOperands(other);
}

std::optional<tsym::Number> tsym::Sum::numericEval() const
{
    Number result(0);

    for (const auto& summand : ops)
        if (const auto numSummand = summand->numericEval())
            result += *numSummand;
        else
            return std::nullopt;

    return result;
}

tsym::Fraction tsym::Sum::normal(SymbolMap& map) const
{
    std::vector<Fraction> fractions;

    if (isZero(*expand()))
        return Fraction{Numeric::zero()};

    for (const auto& summand : ops)
        fractions.push_back(summand->normal(map));

    return toCommonDenom(fractions);
}

tsym::Fraction tsym::Sum::toCommonDenom(const std::vector<Fraction>& operands) const
{
    auto it(cbegin(operands));
    BasePtr denom(it->denom);
    BasePtr num(it->num);

    for (++it; it != cend(operands); ++it) {
        const BasePtr nextNum = it->num;
        const BasePtr nextDenom = it->denom;

        if (denom->isEqual(*nextDenom)) {
            num = create(num, nextNum);
            continue;
        }

        const BasePtr oneOverGcd = Power::oneOver(poly::gcd(denom, nextDenom));
        const BasePtr lcm = Product::create(nextDenom, oneOverGcd)->expand();

        num = create(Product::create(num, lcm)->expand(), Product::create(nextNum, denom, oneOverGcd)->expand());
        denom = Product::create(denom, lcm)->expand();
    }

    return cancel({num, denom});
}

tsym::BasePtr tsym::Sum::diffWrtSymbol(const Base& symbol) const
{
    BasePtrList derivedSummands;

    for (const auto& summand : ops)
        derivedSummands.push_back(summand->diff(symbol));

    return create(derivedSummands);
}

bool tsym::Sum::isPositive() const
{
    if (const auto num = numericEval())
        return num > 0;

    return sign() == 1;
}

bool tsym::Sum::isNegative() const
{
    if (const auto num = numericEval())
        return num < 0;

    return sign() == -1;
}

size_t tsym::Sum::hash() const
{
    return std::hash<BasePtrList>{}(ops);
}

unsigned tsym::Sum::complexity() const
{
    return 5 + complexitySum(ops);
}

int tsym::Sum::sign() const
{
    const int numericSign = signOfNumericParts();
    const int sumOfSigns = numericSign + signOfSymbolicParts();

    if (numericSign == 0)
        return sumOfSigns;
    else if (sumOfSigns > 1)
        return 1;
    else if (sumOfSigns < -1)
        return -1;
    else
        return 0;
}

int tsym::Sum::signOfNumericParts() const
{
    Number numericPart(0);
    using tsym::sign;

    for (const auto& summand : ops)
        if (const auto num = summand->numericEval())
            numericPart += *num;

    return sign(numericPart);
}

int tsym::Sum::signOfSymbolicParts() const
{
    int symbolicSign = 0;

    for (const auto& summand : ops)
        if (summand->numericEval())
            continue;
        else if (summand->isPositive() && symbolicSign >= 0)
            symbolicSign = 1;
        else if (summand->isNegative() && symbolicSign <= 0)
            symbolicSign = -1;
        else
            return 0;

    return symbolicSign;
}

tsym::BasePtr tsym::Sum::expand() const
{
    BasePtrList expandedSummands;

    for (const auto& summand : ops)
        expandedSummands.push_back(summand->expand());

    return create(expandedSummands);
}

tsym::BasePtr tsym::Sum::subst(const tsym::Base& from, const tsym::BasePtr& to) const
{
    using tsym::subst;

    if (isEqual(from))
        return to;
    else
        return create(subst(ops, from, to));
}

tsym::BasePtr tsym::Sum::coeff(const Base& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (!has(variable) && exp == 0)
        return clone();
    else
        return coeffOverSummands(variable, exp);
}

tsym::BasePtr tsym::Sum::coeffOverSummands(const Base& variable, int exp) const
{
    BasePtr coeffSum(Numeric::zero());

    for (const auto& summand : ops)
        coeffSum = create(coeffSum, summand->coeff(variable, exp));

    return coeffSum;
}

int tsym::Sum::degree(const Base& variable) const
{
    static auto minInt = std::numeric_limits<int>::min();

    if (isEqual(variable))
        return 1;
    else
        return boost::accumulate(
          ops, minInt, [&variable](int deg, const auto& summand) { return std::max(deg, summand->degree(variable)); });
}
