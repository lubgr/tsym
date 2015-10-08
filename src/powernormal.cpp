
#include "powernormal.h"
#include "numeric.h"
#include "power.h"
#include "product.h"

tsym::PowerNormal::PowerNormal(SymbolMap& map) :
    one(Numeric::one()),
    map(map)
{}

void tsym::PowerNormal::setBase(const BasePtr& base)
{
    rationalBase = base->normal(map);
}

void tsym::PowerNormal::setExponent(const BasePtr& exp)
{
    rationalExp = exp->normal();
}

tsym::Fraction tsym::PowerNormal::normal()
{
    if (isBaseOrExpUndefined())
        return Fraction(Undefined::create());
    else if (isRationalExpInteger())
        return normalIntegerExp();
    else
        return normalNonIntegerExp();
}

bool tsym::PowerNormal::isBaseOrExpUndefined() const
{
    return rationalBase.num()->isUndefined() || rationalBase.denom()->isUndefined() ||
        rationalExp->isUndefined();
}

bool tsym::PowerNormal::isRationalExpInteger() const
{
    return rationalExp->isNumeric() && rationalExp->numericEval().isInt();
}

tsym::Fraction tsym::PowerNormal::normalIntegerExp() const
    /* Does (a/b)^c = a^c/b^d or b^c/a^d, where c is an integer and d = abs(c). */
{
    const Number nExp(rationalExp->numericEval());
    const BasePtr absExp(Numeric::create(nExp.abs()));
    const BasePtr numPower(Power::create(rationalBase.num(), absExp));
    const BasePtr denomPower(Power::create(rationalBase.denom(), absExp));

    if (nExp > 0)
        return Fraction(numPower, denomPower);
    else if (nExp < 0)
        return Fraction(denomPower, numPower);

    /* The exponent became zero by normal(). */
    return Fraction(one);
}

tsym::Fraction tsym::PowerNormal::normalNonIntegerExp()
{
    if (rationalExp->isNumericallyEvaluable())
        return normalNumEvalExp();
    else
        return normalNonNumEvalExp();
}

tsym::Fraction tsym::PowerNormal::normalNumEvalExp()
{
    const Number nExp(rationalExp->numericEval());

    /* Should have been handled as integer exponent above. */
    assert(!nExp.isZero());

    if (nExp > 0)
        return normalNumEvalPosExp();
    else
        return normalNumEvalNegExp();
}

tsym::Fraction tsym::PowerNormal::normalNumEvalPosExp()
{
    const BasePtr num(evaluatePow(rationalBase.num(), rationalExp));
    const BasePtr denom(evaluatePow(rationalBase.denom(), rationalExp));

    return Fraction(num, denom);
}

tsym::BasePtr tsym::PowerNormal::evaluatePow(const BasePtr& base, const BasePtr& exp)
{
    const BasePtr evaluatedPow(Power::create(base, exp));

    return evaluatedPow->isOne() ? evaluatedPow : map.getTmpSymbolAndStore(evaluatedPow);
}

tsym::Fraction tsym::PowerNormal::normalNumEvalNegExp()
{
    const BasePtr positiveExp(Product::minus(rationalExp));
    const BasePtr num(evaluatePow(rationalBase.denom(), positiveExp));
    const BasePtr denom(evaluatePow(rationalBase.num(), positiveExp));

    return Fraction(num, denom);
}

tsym::Fraction tsym::PowerNormal::normalNonNumEvalExp()
{
    BasePtr replacement;
    BasePtr result;

    result = Power::create(rationalBase.eval(), rationalExp);
    replacement = map.getTmpSymbolAndStore(result);

    return Fraction(replacement, one);
}
