
#include <vector>
#include "base.h"
#include "product.h"
#include "undefined.h"
#include "numeric.h"
#include "productsimpl.h"
#include "power.h"
#include "sum.h"
#include "symbolmap.h"

tsym::Product::Product(const BasePtrList& factors) :
    Base(factors)
{}

tsym::Product::~Product() {}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2)
{
    return create(BasePtrList(f1, f2));
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3)
{
    BasePtrList factors;

    factors.push_back(f1);
    factors.push_back(f2);
    factors.push_back(f3);

    return create(factors);
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3,
        const BasePtr& f4)
{
    BasePtrList factors;

    factors.push_back(f1);
    factors.push_back(f2);
    factors.push_back(f3);
    factors.push_back(f4);

    return create(factors);
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1)
{
    return create(BasePtrList(Numeric::mOne(), f1));
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2)
{
    BasePtrList factors(Numeric::mOne());

    factors.push_back(f1);
    factors.push_back(f2);

    return create(factors);
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3)
{
    BasePtrList factors(Numeric::mOne());

    factors.push_back(f1);
    factors.push_back(f2);
    factors.push_back(f3);

    return create(factors);
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3,
        const BasePtr& f4)
{
    BasePtrList factors(Numeric::mOne());

    factors.push_back(f1);
    factors.push_back(f2);
    factors.push_back(f3);
    factors.push_back(f4);

    return create(factors);
}

tsym::BasePtr tsym::Product::create(const BasePtrList& factors)
{
    if (factors.hasUndefinedElements())
        return Undefined::create();
    else if (factors.hasZeroElements())
        return Numeric::zero();
    else if (factors.size() == 1)
        return factors.front();
    else
        return createSimplifiedProduct(factors);
}

tsym::BasePtr tsym::Product::createSimplifiedProduct(const BasePtrList& factors)
{
    ProductSimpl simpl;
    BasePtrList res;

    res = simpl.simplify(factors);

    if (res.size() == 0)
        return Numeric::one();
    else if (res.size() == 1)
        return res.front();
    else if (needsExpansion(res))
        return res.expandAsProduct();
    else
        return BasePtr(new Product(res));
}

bool tsym::Product::needsExpansion(const BasePtrList& factors)
{
    const BasePtrList constFac(factors.getConstElements());
    const BasePtrList nonConstFac(factors.getNonConstElements());

    if (constFac.empty())
        return false;
    else if (nonConstFac.hasSumElements())
        /* Only expand one single non-const sum, e.g. 2*sqrt(2)*(a + b). */
        return nonConstFac.size() == 1;

    /* This catches (2 + sqrt(2))*a, but also trivial expressions like 2*a. Expanding them does no
     * harm, though. */
    return constFac.hasSumElements();
}

bool tsym::Product::isEqual(const BasePtr& other) const
{
    return isEqualByTypeAndOperands(other);
}

bool tsym::Product::sameType(const BasePtr& other) const
{
    return other->isProduct();
}

tsym::Number tsym::Product::numericEval() const
    /* If a factor returns an undefined Number on numericEval(), the result is undefined, too. */
{
    BasePtrList::const_iterator it;
    Number res(1);

    for (it = ops.begin(); it != ops.end(); ++it)
        res *= (*it)->numericEval();

    return res;
}

tsym::Fraction tsym::Product::normal(SymbolMap& map) const
{
    Fraction unCanceled;

    if (expand()->isZero())
        return Fraction(Numeric::zero());

    unCanceled = normalAndSplitIntoFraction(map);

    return unCanceled.cancel();
}

tsym::Fraction tsym::Product::normalAndSplitIntoFraction(SymbolMap& map) const
{
    BasePtrList::const_iterator it;
    BasePtrList denominators;
    BasePtrList numerators;
    Fraction normalOperand;

    for (it = ops.begin(); it != ops.end(); ++it) {
        normalOperand = (*it)->normal(map);
        numerators.push_back(normalOperand.num());
        denominators.push_back(normalOperand.denom());
    }

    return Fraction(create(numerators), create(denominators));
}

tsym::BasePtr tsym::Product::diffWrtSymbol(const BasePtr& symbol) const
{
    BasePtrList::const_iterator it1;
    BasePtrList::const_iterator it2;
    BasePtrList derivedSummands;
    BasePtrList factors;

    for (it1 = ops.begin(); it1 != ops.end(); ++it1) {
        factors.push_back((*it1)->diffWrtSymbol(symbol));

        for (it2 = ops.begin(); it2 != ops.end(); ++it2)
            if (it1 != it2)
                factors.push_back(*it2);

        derivedSummands.push_back(create(factors));
        factors.clear();
    }

    return Sum::create(derivedSummands);
}

std::string tsym::Product::typeStr() const
{
    return "Product";
}

bool tsym::Product::isProduct() const
{
    return true;
}

tsym::BasePtr tsym::Product::numericTerm() const
{
    const BasePtr& first(ops.front());

    return first->isNumeric() ? first : Numeric::one();
}

tsym::BasePtr tsym::Product::nonNumericTerm() const
{
    if (ops.front()->isNumeric())
        /* We should to go through automatic simplification again, because the factor list could be
         * e.g. of size 1. */
        return create(ops.rest());
    else
        return clone();
}

tsym::BasePtr tsym::Product::constTerm() const
{
    const BasePtrList constItems(ops.getConstElements());

    return constItems.empty() ? Numeric::one() : create(constItems);
}

tsym::BasePtr tsym::Product::nonConstTerm() const
{
    const BasePtrList nonConstItems(ops.getNonConstElements());

    return nonConstItems.empty() ? Numeric::one() : create(nonConstItems);
}

tsym::BasePtr tsym::Product::expand() const
{
    return ops.expandAsProduct();
}

tsym::BasePtr tsym::Product::subst(const BasePtr& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return create(ops.subst(from, to));
}

tsym::BasePtr tsym::Product::coeff(const BasePtr& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (!has(variable) && exp == 0)
        return clone();
    else
        return coeffFactorMatch(variable, exp);
}

tsym::BasePtr tsym::Product::coeffFactorMatch(const BasePtr& variable , int exp) const
{
    const BasePtr pow(Power::create(variable, Numeric::create(exp)));
    BasePtrList resultFactors(ops);
    BasePtrList::iterator it;

    for (it = resultFactors.begin(); it != resultFactors.end(); ++it)
        if ((*it)->isEqual(pow))
            break;

    if (it == resultFactors.end())
        return Numeric::zero();

    resultFactors.erase(it);

    return create(resultFactors);
}

int tsym::Product::degree(const BasePtr& variable) const
{
    BasePtrList::const_iterator it;
    int degreeSum = 0;

    if (isEqual(variable))
        return 1;

    for (it = ops.begin(); it != ops.end(); ++it)
        degreeSum += (*it)->degree(variable);

    return degreeSum;
}
