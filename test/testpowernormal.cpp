
#include "abc.h"
#include "powernormal.h"
#include "constant.h"
#include "power.h"
#include "sum.h"
#include "product.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(PowerNormal)
{
    SymbolMap *map;
    const BasePtr undefined;
    BasePtr zeroByExpansion;
    BasePtr zeroByNormal;
    BasePtr abSum;
    BasePtr pi;

    void setup()
    {
        const BasePtr oneOverB = Power::oneOver(b);

        abSum = Sum::create(a, b);
        pi = Constant::createPi();

        /* a/b + (c - a)/b - c/b becomes 0 by normalization. */
        zeroByNormal = Sum::create(Product::create(a, oneOverB), Product::minus(c, oneOverB),
            Product::create(Sum::create(c, Product::minus(a)), oneOverB));
        /* a*b + a*c - a*(b + c) is zero after expansion. */
        zeroByExpansion = Sum::create(Product::create(a, b), Product::create(a, c),
                Product::minus(a, Sum::create(b, c)));

        map = new SymbolMap();
    }

    void teardown()
    {
        delete map;
    }
};

TEST(PowerNormal, unspecifiedInput)
{
    PowerNormal pn(*map);
    Fraction res;

    res = pn.normal();

    CHECK(res.num()->isUndefined());
    CHECK_EQUAL(one, res.denom());
}

TEST(PowerNormal, powerWithPosIntExp)
{
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(two);

    res = pn.normal();

    CHECK_EQUAL(Power::create(a, two), res.num());
    CHECK_EQUAL(one, res.denom());
}

TEST(PowerNormal, powerWithPosNegExp)
{
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(Numeric::create(-2));

    res = pn.normal();

    CHECK_EQUAL(one, res.num());
    CHECK_EQUAL(Power::create(a, two), res.denom());
}

TEST(PowerNormal, powerWithSymbolExp)
    /* (a + b)^c becomes tmp/1. */
{
    const BasePtr orig = Power::create(abSum, c);
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(Sum::create(a, b));
    pn.setExponent(c);

    res = pn.normal();

    CHECK_EQUAL(one, res.denom());
    CHECK(res.num()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    CHECK_EQUAL(orig, backReplaced);
}

TEST(PowerNormal, fractionBaseSymbolExp)
    /* (3/4)^a becomes tmp1/1. */
{
    const BasePtr base = Numeric::create(3, 4);
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(base);
    pn.setExponent(a);

    res = pn.normal();

    CHECK_EQUAL(one, res.denom());
    CHECK(res.num()->isSymbol());
}

TEST(PowerNormal, fractionBaseNumericallyEvaluableNegExp)
    /* (3/4)^(-sin(1)) = tmp1/tmp2 with tmp1 = 4^sin(1) and tmp2 = 3^sin(1). */
{
    const BasePtr sinOne = Trigonometric::createSin(one);
    const BasePtr expectedNum = Power::create(four, sinOne);
    const BasePtr expectedDenom = Power::create(three, sinOne);
    const BasePtr exp = Product::minus(sinOne);
    const BasePtr base = Numeric::create(3, 4);
    PowerNormal pn(*map);
    BasePtr backReplaced;
    Fraction res;

    pn.setBase(base);
    pn.setExponent(exp);

    res = pn.normal();

    CHECK(res.num()->isSymbol());
    CHECK(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    CHECK_EQUAL(expectedDenom, backReplaced);
}

TEST(PowerNormal, fractionBaseNumericallyEvaluablePosExp)
    /* (2/5)^sqrt(2) = tmp1/tmp2 with tmp1 = 2^sqrt(2) and tmp2 = 5^sqrt(2). */
{
    const BasePtr exp = Power::sqrt(two);
    const BasePtr expectedNum = Power::create(two, exp);
    const BasePtr expectedDenom = Power::create(five, exp);
    PowerNormal pn(*map);
    BasePtr backReplaced;
    Fraction res;

    pn.setBase(Numeric::create(2, 5));
    pn.setExponent(exp);

    res = pn.normal();

    CHECK(res.num()->isSymbol());
    CHECK(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    CHECK_EQUAL(expectedDenom, backReplaced);
}

TEST(PowerNormal, symbolicFractionBaseNumericallyEvaluablePosExp)
    /* (a/b)^sqrt(2) = tmp1/tmp2 with tmp1 = a^sqrt(2) and tmp2 = b^sqrt(2). */
{
    const BasePtr base = Product::create(a, Power::oneOver(b));
    const BasePtr exp = Power::sqrt(two);
    const BasePtr expectedNum = Power::create(a, exp);
    const BasePtr expectedDenom = Power::create(b, exp);
    PowerNormal pn(*map);
    BasePtr backReplaced;
    Fraction res;

    pn.setBase(base);
    pn.setExponent(exp);

    res = pn.normal();

    CHECK(res.num()->isSymbol());
    CHECK(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    CHECK_EQUAL(expectedDenom, backReplaced);
}

TEST(PowerNormal, powerWithMinusSymbolExp)
    /* (a + b)^(-c) becomes tmp/1, too. */
{
    const BasePtr minusC = Product::minus(c);
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(abSum);
    pn.setExponent(minusC);

    res = pn.normal();

    CHECK_EQUAL(one, res.denom());
    CHECK(res.num()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    CHECK_EQUAL(Power::create(abSum, minusC), backReplaced);
}

TEST(PowerNormal, powerWithPiExp)
    /* a^Pi becomes tmp/1 with tmp = a^Pi. */
{
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(pi);

    res = pn.normal();

    CHECK_EQUAL(one, res.denom());
    CHECK(res.num()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    CHECK_EQUAL(Power::create(a, pi), backReplaced);
}

TEST(PowerNormal, powerWithNegNumEvalExp)
    /* a^(-3*sqrt(2)*Pi) becomes 1/tmp with tmp = a^(3*sqrt(2)*Pi). */
{
    const BasePtr pos = Product::create(three, pi, Power::sqrt(two));
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(Product::minus(pos));

    res = pn.normal();

    CHECK_EQUAL(one, res.num());
    CHECK(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    CHECK_EQUAL(Power::create(a, pos), backReplaced);
}

TEST(PowerNormal, rationalBaseZero)
{
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(zeroByNormal);
    pn.setExponent(pi);

    res = pn.normal();
    backReplaced = map->replaceTmpSymbolsBackFrom(res.eval());

    CHECK_EQUAL(zero, backReplaced);
}

TEST(PowerNormal, rationalBaseOne)
{
    const BasePtr base = Sum::create(zeroByNormal, one);
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(base);
    pn.setExponent(pi);

    res = pn.normal();
    backReplaced = map->replaceTmpSymbolsBackFrom(res.eval());

    CHECK_EQUAL(one, backReplaced);
}

TEST(PowerNormal, rationalBaseUndefined)
{
    const BasePtr base = Power::oneOver(zeroByNormal);
    PowerNormal pn(*map);
    BasePtr res;

    pn.setBase(base);
    pn.setExponent(pi);

    res = pn.normal().eval();

    CHECK(res->isUndefined());
}

TEST(PowerNormal, rationalBaseUndefinedByExpansion)
{
    const BasePtr base = Power::oneOver(zeroByExpansion);
    PowerNormal pn(*map);
    BasePtr res;

    pn.setBase(base);
    pn.setExponent(a);

    res = pn.normal().eval();

    CHECK(res->isUndefined());
}
