
#include "product.h"
#include <boost/range/algorithm.hpp>
#include <boost/range/numeric.hpp>
#include <vector>
#include "bplist.h"
#include "fraction.h"
#include "numeric.h"
#include "power.h"
#include "productsimpl.h"
#include "sum.h"
#include "symbolmap.h"
#include "undefined.h"

tsym::Product::Product(const BasePtrList& factors, Base::CtorKey&&)
    : Base(factors)
{
    setDebugString();
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2)
{
    return create({f1, f2});
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3)
{
    return create({f1, f2, f3});
}

tsym::BasePtr tsym::Product::create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3, const BasePtr& f4)
{
    return create({f1, f2, f3, f4});
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1)
{
    return create({Numeric::mOne(), f1});
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2)
{
    return create({Numeric::mOne(), f1, f2});
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3)
{
    return create({Numeric::mOne(), f1, f2, f3});
}

tsym::BasePtr tsym::Product::minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3, const BasePtr& f4)
{
    return create({Numeric::mOne(), f1, f2, f3, f4});
}

tsym::BasePtr tsym::Product::create(const BasePtrList& factors)
{
    if (bplist::hasUndefinedElements(factors))
        return Undefined::create();
    else if (bplist::hasZeroElements(factors))
        return Numeric::zero();
    else if (factors.size() == 1)
        return factors.front();
    else
        return createSimplifiedProduct(factors);
}

tsym::BasePtr tsym::Product::createSimplifiedProduct(const BasePtrList& factors)
{
    const BasePtrList res = productsimpl::simplify(factors);

    if (res.empty())
        return Numeric::one();
    else if (res.size() == 1)
        return res.front();
    else if (needsExpansion(res))
        return bplist::expandAsProduct(res);
    else
        return std::make_shared<const Product>(res, Base::CtorKey{});
}

bool tsym::Product::needsExpansion(const BasePtrList& factors)
{
    const BasePtrList constFac(bplist::getConstElements(factors));
    const BasePtrList nonConstFac(bplist::getNonConstElements(factors));

    if (constFac.empty())
        return false;
    else if (bplist::hasSumElements(nonConstFac))
        /* Only expand one single non-const sum, e.g. 2*sqrt(2)*(a + b). */
        return nonConstFac.size() == 1;

    /* This catches (2 + sqrt(2))*a, but also trivial expressions like 2*a. Expanding them does no
     * harm, though. */
    return bplist::hasSumElements(constFac);
}

bool tsym::Product::isEqualDifferentBase(const Base& other) const
{
    return isEqualByTypeAndOperands(other);
}

bool tsym::Product::sameType(const Base& other) const
{
    return other.isProduct();
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
    if (expand()->isZero())
        return {Numeric::zero()};
    else
        return cancel(normalAndSplitIntoFraction(map));
}

tsym::Fraction tsym::Product::normalAndSplitIntoFraction(SymbolMap& map) const
{
    BasePtrList denominators;
    BasePtrList numerators;

    for (const auto& factor : ops) {
        const auto normalOperand = factor->normal(map);
        numerators.push_back(normalOperand.num);
        denominators.push_back(normalOperand.denom);
    }

    return Fraction{create(numerators), create(denominators)};
}

tsym::BasePtr tsym::Product::diffWrtSymbol(const Base& symbol) const
{
    BasePtrList derivedSummands;
    BasePtrList factors;

    for (auto it1 = cbegin(ops); it1 != cend(ops); ++it1) {
        factors.push_back((*it1)->diffWrtSymbol(symbol));

        for (auto it2 = cbegin(ops); it2 != cend(ops); ++it2)
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
    return std::hash<BasePtrList>{}(ops);
}

unsigned tsym::Product::complexity() const
{
    return 5 + bplist::complexitySum(ops);
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
        return create(bplist::rest(ops));
    else
        return clone();
}

tsym::BasePtr tsym::Product::constTerm() const
{
    const BasePtrList constItems(bplist::getConstElements(ops));

    return constItems.empty() ? Numeric::one() : create(constItems);
}

tsym::BasePtr tsym::Product::nonConstTerm() const
{
    const BasePtrList nonConstItems(bplist::getNonConstElements(ops));

    return nonConstItems.empty() ? Numeric::one() : create(nonConstItems);
}

tsym::BasePtr tsym::Product::expand() const
{
    return bplist::expandAsProduct(ops);
}

tsym::BasePtr tsym::Product::subst(const Base& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return create(bplist::subst(ops, from, to));
}

tsym::BasePtr tsym::Product::coeff(const Base& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (!has(variable) && exp == 0)
        return clone();
    else
        return coeffFactorMatch(variable, exp);
}

tsym::BasePtr tsym::Product::coeffFactorMatch(const Base& variable, int exp) const
{
    const BasePtr pow(Power::create(variable.clone(), Numeric::create(exp)));
    auto matchingPower = boost::find_if(ops, [&pow](const auto& item) { return item->isEqual(*pow); });

    if (matchingPower == cend(ops))
        return Numeric::zero();

    BasePtrList factors;

    factors.insert(cend(factors), cbegin(ops), matchingPower);

    if (matchingPower != cend(ops))
        factors.insert(cend(factors), ++matchingPower, cend(ops));

    return create(factors);
}

int tsym::Product::degree(const Base& variable) const
{
    if (isEqual(variable))
        return 1;
    else
        return boost::accumulate(
          ops, 0, [&variable](int deg, const auto& factor) { return deg + factor->degree(variable); });
}
