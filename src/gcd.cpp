
#include "gcd.h"
#include <cassert>
#include <cmath>
#include "basefct.h"
#include "bplist.h"
#include "logging.h"
#include "numberfct.h"
#include "numeric.h"
#include "polyinfo.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "undefined.h"

tsym::BasePtr tsym::Gcd::compute(const BasePtr& u, const BasePtr& v) const
{
    if (polyinfo::isInputValid(*u, *v))
        return compute(u, v, polyinfo::listOfSymbols(*u, *v));

    TSYM_ERROR("Invalid gcd input: %S and %S", u, v);

    return Undefined::create();
}

tsym::BasePtr tsym::Gcd::compute(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const
{
    const BasePtr uExp(u->expand());
    const BasePtr vExp(v->expand());
    BasePtr result(Numeric::one());

    assert((L.empty() && uExp->isNumeric() && vExp->isNumeric()) || !L.empty());

    if (isOne(*uExp) || isOne(*vExp))
        ;
    else if (isZero(*uExp) && isZero(*vExp))
        return Undefined::create();
    else if (isZero(*uExp))
        result = vExp;
    else if (isZero(*vExp))
        result = uExp;
    else if (uExp->isEqual(*vExp))
        result = uExp;
    else if (uExp->isNumeric() && vExp->isNumeric())
        result = computeNumerics(uExp, vExp);
    else if (!haveCommonSymbol(u, v, L))
        result = integerContent(u, v);
    else
        result = gcdViaAlgo(uExp, vExp, L);

    return normalize(result, L);
}

tsym::BasePtr tsym::Gcd::computeNumerics(const BasePtr& u, const BasePtr& v) const
{
    const Number numU(u->numericEval().value_or(1));
    const Number numV(v->numericEval().value_or(1));
    Int intGcd(1);

    assert(numU.isRational() && numV.isRational());

    if (isInt(numU) && isInt(numV))
        intGcd = integerGcd(numU.numerator(), numV.numerator());

    return Numeric::create(intGcd);
}

tsym::Int tsym::Gcd::integerGcd(const Int& u, const Int& v) const
{
    if (v == 0)
        return u;

    return integerGcd(v, u % v);
}

bool tsym::Gcd::haveCommonSymbol(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const
{
    for (const auto& item : L)
        if (u->has(*item) && v->has(*item))
            return true;

    return false;
}

tsym::BasePtr tsym::Gcd::gcdViaAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const
{
    const BasePtr intContent(integerContent(u, v));
    const BasePtr factor(Power::oneOver(intContent));
    const BasePtr uReduced(Product::create(factor, u));
    const BasePtr vReduced(Product::create(factor, v));
    BasePtr gcd;

    gcd = gcdAlgo(uReduced, vReduced, L);

    return Product::create(intContent, gcd);
}

tsym::BasePtr tsym::Gcd::integerContent(const BasePtr& u, const BasePtr& v) const
{
    const Number uIntContent(integerContent(u));
    const Number vIntContent(integerContent(v));
    Int intGcd;

    if (!isInt(uIntContent) || !isInt(vIntContent))
        return Numeric::one();

    intGcd = integerGcd(uIntContent.numerator(), vIntContent.numerator());

    return Numeric::create(intGcd);
}

tsym::Number tsym::Gcd::integerContent(const BasePtr& poly) const
{
    Number result;

    if (poly->isSum())
        result = integerContentOfSum(poly->operands());
    else
        result = abs(poly->numericTerm()->numericEval().value_or(1));

    return isInt(result) ? result : 1;
}

tsym::Number tsym::Gcd::integerContentOfSum(const BasePtrList& summands) const
{
    Number intContent;
    Int result(0);

    for (const auto& summand : summands) {
        intContent = integerContent(summand);

        assert(isInt(intContent));

        result = integerGcd(result, intContent.numerator());
    }

    return Number(result);
}

tsym::BasePtr tsym::Gcd::normalize(const BasePtr& result, const BasePtrList& L) const
{
    BasePtrList symbolListCopy(L);
    Number factor;

    factor = normalizationFactor(result, symbolListCopy);

    return Product::create(Numeric::create(factor), result);
}

tsym::Number tsym::Gcd::normalizationFactor(const BasePtr& arg, BasePtrList& L) const
{
    BasePtr lCoeff;

    if (L.empty())
        lCoeff = arg;
    else {
        lCoeff = arg->leadingCoeff(*L.back());
        L.pop_back();
    }

    if (!lCoeff->isNumeric())
        return normalizationFactor(lCoeff, L);

    const Number fac = *lCoeff->numericEval();

    if (!fac.isRational())
        TSYM_ERROR("%S has a non-rational leading coefficient!", arg);
    else if (fac == 0)
        TSYM_ERROR("Gcd result %S has a zero leading coefficient.", arg);

    return sign(fac);
}
