
#include "powernormal.h"
#include "basefct.h"
#include "numberfct.h"
#include "power.h"
#include "product.h"
#include "symbolmap.h"
#include "undefined.h"

tsym::PowerNormal::PowerNormal(const Base& base, const Base& exp, SymbolMap& map)
    : map(map)
    , rationalBase{base.normal(this->map)}
    , rationalExp{exp.normal()}
{}

tsym::Fraction tsym::PowerNormal::normal() const
{
    if (isBaseOrExpUndefined())
        return {Undefined::create()};
    else if (isRationalExpInteger())
        return normalIntegerExp();
    else
        return normalNonIntegerExp();
}

bool tsym::PowerNormal::isBaseOrExpUndefined() const
{
    return isUndefined(*rationalBase.num) || isUndefined(*rationalBase.denom) || isZero(*rationalBase.denom)
      || isZero(*rationalBase.denom->expand()) || isUndefined(*rationalExp);
}

bool tsym::PowerNormal::isRationalExpInteger() const
{
    if (const auto num = rationalExp->numericEval())
        return isInt(*num);

    return false;
}

tsym::Fraction tsym::PowerNormal::normalIntegerExp() const
/* Does (a/b)^c = a^c/b^d or b^c/a^d, where c is an integer and d = abs(c). */
{
    const Number nExp(*rationalExp->numericEval());
    const BasePtr absExp(Numeric::create(abs(nExp)));
    const BasePtr numPower(Power::create(rationalBase.num, absExp));
    const BasePtr denomPower(Power::create(rationalBase.denom, absExp));

    if (nExp > 0)
        return {numPower, denomPower};
    else if (nExp < 0)
        return {denomPower, numPower};

    /* The exponent became zero by normal(). */
    return {one};
}

tsym::Fraction tsym::PowerNormal::normalNonIntegerExp() const
{
    if (rationalExp->numericEval())
        return normalNumEvalExp();
    else
        return normalNonNumEvalExp();
}

tsym::Fraction tsym::PowerNormal::normalNumEvalExp() const
{
    /* Should have been handled as integer exponent above. */
    assert(rationalExp->numericEval() != 0);

    if (rationalExp->isPositive())
        return normalNumEvalPosExp();
    else
        return normalNumEvalNegExp();
}

tsym::Fraction tsym::PowerNormal::normalNumEvalPosExp() const
{
    const BasePtr num(evaluatePow(rationalBase.num, rationalExp));
    const BasePtr denom(evaluatePow(rationalBase.denom, rationalExp));

    return {num, denom};
}

tsym::BasePtr tsym::PowerNormal::evaluatePow(const BasePtr& base, const BasePtr& exp) const
{
    const BasePtr evaluatedPow(Power::create(base, exp));

    return isOne(*evaluatedPow) ? evaluatedPow : map.getTmpSymbolAndStore(evaluatedPow);
}

tsym::Fraction tsym::PowerNormal::normalNumEvalNegExp() const
{
    const BasePtr positiveExp(Product::minus(rationalExp));
    const BasePtr num(evaluatePow(rationalBase.denom, positiveExp));
    const BasePtr denom(evaluatePow(rationalBase.num, positiveExp));

    return {num, denom};
}

tsym::Fraction tsym::PowerNormal::normalNonNumEvalExp() const
{
    const BasePtr result = Power::create(eval(rationalBase), rationalExp);
    const BasePtr replacement = map.getTmpSymbolAndStore(result);

    return {replacement, one};
}
