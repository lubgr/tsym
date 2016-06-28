
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
{}

tsym::Sum::~Sum() {}

tsym::BasePtr tsym::Sum::create(const BasePtr& s1, const BasePtr& s2)
{
    return create(BasePtrList(s1, s2));
}

tsym::BasePtr tsym::Sum::create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3)
{
    BasePtrList summands;

    summands.push_back(s1);
    summands.push_back(s2);
    summands.push_back(s3);

    return create(summands);
}

tsym::BasePtr tsym::Sum::create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3,
        const BasePtr& s4)
{
    BasePtrList summands;

    summands.push_back(s1);
    summands.push_back(s2);
    summands.push_back(s3);
    summands.push_back(s4);

    return create(summands);
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
        return BasePtr(new Sum(res));
}

bool tsym::Sum::isEqual(const BasePtr& other) const
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
    BasePtrList::const_iterator it;
    Number res(0);

    for (it = ops.begin(); it != ops.end(); ++it)
        res += (*it)->numericEval();

    return res;
}

tsym::Fraction tsym::Sum::normal(SymbolMap& map) const
{
    std::vector<Fraction> fractions;

    if (expand()->isZero())
        return Fraction(Numeric::zero());

    for (BasePtrList::const_iterator it = ops.begin(); it != ops.end(); ++it)
        fractions.push_back((*it)->normal(map));

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
    BasePtrList::const_iterator it;
    BasePtrList derivedSummands;

    for (it = ops.begin(); it != ops.end(); ++it)
        derivedSummands.push_back((*it)->diff(symbol));

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

    for (BasePtrList::const_iterator it = ops.begin(); it != ops.end(); ++it)
        if ((*it)->isNumericallyEvaluable())
            numericPart += (*it)->numericEval();

    if (numericPart.isZero())
        return 0;
    else
        return numericPart > 0 ? 1 : -1;
}

int tsym::Sum::signOfSymbolicParts() const
{
    int symbolicSign = 0;

    for (BasePtrList::const_iterator it = ops.begin(); it != ops.end(); ++it)
        if ((*it)->isNumericallyEvaluable())
            continue;
        else if ((*it)->isPositive() && symbolicSign >= 0)
            symbolicSign = 1;
        else if ((*it)->isNegative() && symbolicSign <= 0)
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
    BasePtrList::const_iterator it;
    BasePtrList expandedSummands;

    for (it = ops.begin(); it != ops.end(); ++it)
        expandedSummands.push_back((*it)->expand());

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
    BasePtrList::const_iterator it;

    for (it = ops.begin(); it != ops.end(); ++it)
        coeffSum = create(coeffSum, (*it)->coeff(variable, exp));

    return coeffSum;
}

int tsym::Sum::degree(const tsym::BasePtr& variable) const
{
    BasePtrList::const_iterator it = ops.begin();
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
