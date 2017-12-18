
#include <cassert>
#include "sum.h"
#include "sumsimpl.h"
#include "numeric.h"
#include "undefined.h"
#include "power.h"
#include "product.h"
#include "poly.h"

tsym::Sum::Sum(const BasePtrList& summands) :
    Base(summands)
{
    setDebugString();
}

tsym::Sum::~Sum() {}

tsym::BasePtr tsym::Sum::create(const BasePtr& s1, const BasePtr& s2)
{
    return create(BasePtrList(s1, s2));
}

tsym::BasePtr tsym::Sum::create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3)
{
    return create({ s1, s2, s3 });
}

tsym::BasePtr tsym::Sum::create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3,
        const BasePtr& s4)
{
    return create({ s1, s2, s3, s4 });
}

tsym::BasePtr tsym::Sum::create(const BasePtrList& summands)
{
    if (summands.hasUndefinedElements())
        return Undefined::create();
    else if (summands.size() == 1)
        return summands.front();
    else
        return createSimplifiedSum(summands);
}

tsym::BasePtr tsym::Sum::createSimplifiedSum(const BasePtrList& summands)
{
    SumSimpl simpl;
    BasePtrList res;

    res = simpl.simplify(summands);

    if (res.size() == 0)
        return Numeric::zero();
    else if (res.size() == 1)
        return res.front();
    else
        return instantiate([&res]() { return new Sum(res); });
}

bool tsym::Sum::isEqualDifferentBase(const BasePtr& other) const
{
    return isEqualByTypeAndOperands(other);
}

bool tsym::Sum::sameType(const BasePtr& other) const
{
    return other->isSum();
}

tsym::Number tsym::Sum::numericEval() const
    /* If one of the summands returns an undefined Number on numericEval(), the result will be
     * undefined, too. */
{
    Number res(0);

    for (const auto& summand : ops)
        res += summand->numericEval();

    return res;
}

tsym::Fraction tsym::Sum::normal(SymbolMap& map) const
{
    std::vector<Fraction> fractions;

    if (expand()->isZero())
        return Fraction(Numeric::zero());

    for (const auto& summand : ops)
        fractions.push_back(summand->normal(map));

    return toCommonDenom(fractions);
}

tsym::Fraction tsym::Sum::toCommonDenom(const std::vector<Fraction>& operands) const
{
    std::vector<Fraction>::const_iterator it(operands.begin());
    BasePtr denom(it->denom());
    BasePtr num(it->num());
    BasePtr oneOverGcd;
    BasePtr nextDenom;
    BasePtr nextNum;
    BasePtr lcm;

    for (++it; it != operands.end(); ++it) {
        nextNum = it->num();
        nextDenom = it->denom();

        if (denom->isEqual(nextDenom)) {
            num = create(num, nextNum);
            continue;
        }

        oneOverGcd = Power::oneOver(poly::gcd(denom, nextDenom));
        lcm = Product::create(nextDenom, oneOverGcd)->expand();

        num = create(Product::create(num, lcm)->expand(),
                Product::create(nextNum, denom, oneOverGcd)->expand());
        denom = Product::create(denom, lcm)->expand();
    }

    return Fraction(num, denom).cancel();
}

tsym::BasePtr tsym::Sum::diffWrtSymbol(const BasePtr& symbol) const
{
    BasePtrList derivedSummands;

    for (const auto& summand : ops)
        derivedSummands.push_back(summand->diff(symbol));

    return create(derivedSummands);
}

std::string tsym::Sum::typeStr() const
{
    return "Sum";
}

bool tsym::Sum::isPositive() const
{
    return isNumericallyEvaluable() ? numericEval() > 0 : sign() == 1;
}

bool tsym::Sum::isNegative() const
{
    return isNumericallyEvaluable() ? numericEval() < 0 : sign() == -1;
}

size_t tsym::Sum::hash() const
{
    return std::hash<BasePtrList>{}(ops);
}

unsigned tsym::Sum::complexity() const
{
    return 5 + ops.complexitySum();
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

    for (const auto& summand : ops)
        if (summand->isNumericallyEvaluable())
            numericPart += summand->numericEval();

    if (numericPart.isZero())
        return 0;
    else
        return numericPart > 0 ? 1 : -1;
}

int tsym::Sum::signOfSymbolicParts() const
{
    int symbolicSign = 0;

    for (const auto& summand : ops)
        if (summand->isNumericallyEvaluable())
            continue;
        else if (summand->isPositive() && symbolicSign >= 0)
            symbolicSign = 1;
        else if (summand->isNegative() && symbolicSign <= 0)
            symbolicSign = -1;
        else
            return 0;

    return symbolicSign;
}

bool tsym::Sum::isSum() const
{
    return true;
}

tsym::BasePtr tsym::Sum::expand() const
{
    BasePtrList expandedSummands;

    for (const auto& summand : ops)
        expandedSummands.push_back(summand->expand());

    return create(expandedSummands);
}

tsym::BasePtr tsym::Sum::subst(const tsym::BasePtr& from, const tsym::BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return create(ops.subst(from, to));
}

tsym::BasePtr tsym::Sum::coeff(const BasePtr& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (!has(variable) && exp == 0)
        return clone();
    else
        return coeffOverSummands(variable, exp);
}

tsym::BasePtr tsym::Sum::coeffOverSummands(const BasePtr& variable, int exp) const
{
    BasePtr coeffSum(Numeric::zero());

    for (const auto& summand : ops)
        coeffSum = create(coeffSum, summand->coeff(variable, exp));

    return coeffSum;
}

int tsym::Sum::degree(const tsym::BasePtr& variable) const
{
    auto it = ops.begin();
    int maxDegree;
    int deg;

    if (isEqual(variable))
        return 1;

    for (maxDegree = (*it)->degree(variable), ++it; it != ops.end(); ++it) {
        deg = (*it)->degree(variable);
        if (deg > maxDegree)
            maxDegree = deg;
    }

    return maxDegree;
}
