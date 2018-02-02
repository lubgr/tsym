
#include <vector>
#include <numeric>
#include "product.h"
#include "undefined.h"
#include "numeric.h"
#include "productsimpl.h"
#include "fraction.h"
#include "power.h"
#include "sum.h"
#include "ctr.h"
#include "symbolmap.h"

tsym::Product::Product(const BasePtrCtr& factors, Base::CtorKey&&) :
    Base(factors)
{
    setDebugString();
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2)
{
    return create({ f1, f2 });
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3)
{
    return create({ f1, f2, f3 });
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3,
        const BasePtr& f4)
{
    return create({ f1, f2, f3, f4 });
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1)
{
    return create({ Numeric::mOne(), f1 });
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2)
{
    return create({ Numeric::mOne(), f1, f2 });
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3)
{
    return create({ Numeric::mOne(), f1, f2, f3 });
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3,
        const BasePtr& f4)
{
    return create({ Numeric::mOne(), f1, f2, f3, f4 });
}

tsym::BasePtr tsym::Product::create(const BasePtrCtr& factors)
{
    if (ctr::hasUndefinedElements(factors))
        return Undefined::create();
    else if (ctr::hasZeroElements(factors))
        return Numeric::zero();
    else if (factors.size() == 1)
        return factors.front();
    else
        return createSimplifiedProduct(factors);
}

tsym::BasePtr tsym::Product::createSimplifiedProduct(const BasePtrCtr& factors)
{
    ProductSimpl simpl;
    const BasePtrCtr res = simpl.simplify(factors);

    if (res.size() == 0)
        return Numeric::one();
    else if (res.size() == 1)
        return res.front();
    else if (needsExpansion(res))
        return ctr::expandAsProduct(res);
    else
        return std::make_shared<const Product>(res, Base::CtorKey{});
}

bool tsym::Product::needsExpansion(const BasePtrCtr& factors)
{
    const BasePtrCtr constFac(ctr::getConstElements(factors));
    const BasePtrCtr nonConstFac(ctr::getNonConstElements(factors));

    if (constFac.empty())
        return false;
    else if (ctr::hasSumElements(nonConstFac))
        /* Only expand one single non-const sum, e.g. 2*sqrt(2)*(a + b). */
        return nonConstFac.size() == 1;

    /* This catches (2 + sqrt(2))*a, but also trivial expressions like 2*a. Expanding them does no
     * harm, though. */
    return ctr::hasSumElements(constFac);
}

bool tsym::Product::isEqualDifferentBase(const BasePtr& other) const
{
    return isEqualByTypeAndOperands(other);
}

bool tsym::Product::sameType(const BasePtr& other) const
{
    return other->isProduct();
}

tsym::Number tsym::Product::numericEval() const
{
    Number res(1);

    for (const auto& factor : ops)
        if (factor->isNumericallyEvaluable())
            res *= factor->numericEval();
        else
            throw std::logic_error("Product isn't numerically evaluable");

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
    BasePtrCtr denominators;
    BasePtrCtr numerators;
    Fraction normalOperand;

    for (const auto& factor : ops) {
        normalOperand = factor->normal(map);
        numerators.push_back(normalOperand.num());
        denominators.push_back(normalOperand.denom());
    }

    return Fraction(create(numerators), create(denominators));
}

tsym::BasePtr tsym::Product::diffWrtSymbol(const BasePtr& symbol) const
{
    BasePtrCtr derivedSummands;
    BasePtrCtr factors;

    for (auto it1 = ops.begin(); it1 != ops.end(); ++it1) {
        factors.push_back((*it1)->diffWrtSymbol(symbol));

        for (auto it2 = ops.begin(); it2 != ops.end(); ++it2)
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

bool tsym::Product::isPositive() const
{
    return sign() == 1;
}

bool tsym::Product::isNegative() const
{
    return sign() == -1;
}

size_t tsym::Product::hash() const
{
    return std::hash<BasePtrCtr>{}(ops);
}

unsigned tsym::Product::complexity() const
{
    return 5 + ctr::complexitySum(ops);
}

int tsym::Product::sign() const
{
    int result = 1;

    for (const auto& factor : ops)
        if (factor->isPositive())
            continue;
        else if (factor->isNegative())
            result *= -1;
        else
            return 0;

    return result;
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
        return create(ctr::rest(ops));
    else
        return clone();
}

tsym::BasePtr tsym::Product::constTerm() const
{
    const BasePtrCtr constItems(ctr::getConstElements(ops));

    return constItems.empty() ? Numeric::one() : create(constItems);
}

tsym::BasePtr tsym::Product::nonConstTerm() const
{
    const BasePtrCtr nonConstItems(ctr::getNonConstElements(ops));

    return nonConstItems.empty() ? Numeric::one() : create(nonConstItems);
}

tsym::BasePtr tsym::Product::expand() const
{
    return ctr::expandAsProduct(ops);
}

tsym::BasePtr tsym::Product::subst(const BasePtr& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return create(ctr::subst(ops, from, to));
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

tsym::BasePtr tsym::Product::coeffFactorMatch(const BasePtr& variable, int exp) const
{
    const BasePtr pow(Power::create(variable, Numeric::create(exp)));
    auto matchingPower = std::find_if(cbegin(ops), cend(ops), [&pow](const auto& item){ return item->isEqual(pow); });

    if (matchingPower == cend(ops))
        return Numeric::zero();

    BasePtrCtr factors;

    factors.insert(cend(factors), cbegin(ops), matchingPower);

    if (matchingPower != cend(ops))
        factors.insert(cend(factors), ++matchingPower, cend(ops));

    return create(factors);
}

int tsym::Product::degree(const BasePtr& variable) const
{
    if (isEqual(variable))
        return 1;
    else
        return std::accumulate(cbegin(ops), cend(ops), 0,
                [&variable](int deg, const auto& factor){ return deg + factor->degree(variable); });
}
