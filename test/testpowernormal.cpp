
#include <memory>
#include "constant.h"
#include "fixtures.h"
#include "power.h"
#include "powernormal.h"
#include "product.h"
#include "sum.h"
#include "symbolmap.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

struct PowerNormalFixture : public AbcFixture {
    const BasePtr oneOverB = Power::oneOver(b);
    const BasePtr abSum = Sum::create(a, b);
    /* a/b + (c - a)/b - c/b becomes 0 by normalization. */
    const BasePtr zeroByNormal = Sum::create(Product::create(a, oneOverB), Product::minus(c, oneOverB),
      Product::create(Sum::create(c, Product::minus(a)), oneOverB));
    /* a*b + a*c - a*(b + c) is zero after expansion. */
    const BasePtr zeroByExpansion =
      Sum::create(Product::create(a, b), Product::create(a, c), Product::minus(a, Sum::create(b, c)));
    std::unique_ptr<SymbolMap> map = std::make_unique<SymbolMap>();
};

BOOST_FIXTURE_TEST_SUITE(TestPowerNormal, PowerNormalFixture)

BOOST_AUTO_TEST_CASE(unspecifiedInput)
{
    PowerNormal pn(*map);
    Fraction res;

    res = pn.normal();

    BOOST_TEST(res.num()->isUndefined());
    BOOST_CHECK_EQUAL(one, res.denom());
}

BOOST_AUTO_TEST_CASE(powerWithPosIntExp)
{
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(two);

    res = pn.normal();

    BOOST_CHECK_EQUAL(Power::create(a, two), res.num());
    BOOST_CHECK_EQUAL(one, res.denom());
}

BOOST_AUTO_TEST_CASE(powerWithPosNegExp)
{
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(Numeric::create(-2));

    res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.num());
    BOOST_CHECK_EQUAL(Power::create(a, two), res.denom());
}

BOOST_AUTO_TEST_CASE(powerWithSymbolExp)
/* (a + b)^c becomes tmp/1. */
{
    const BasePtr orig = Power::create(abSum, c);
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(Sum::create(a, b));
    pn.setExponent(c);

    res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom());
    BOOST_TEST(res.num()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    BOOST_CHECK_EQUAL(orig, backReplaced);
}

BOOST_AUTO_TEST_CASE(fractionBaseSymbolExp)
/* (3/4)^a becomes tmp1/1. */
{
    const BasePtr base = Numeric::create(3, 4);
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(base);
    pn.setExponent(a);

    res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom());
    BOOST_TEST(res.num()->isSymbol());
}

BOOST_AUTO_TEST_CASE(fractionBaseNumericallyEvaluableNegExp)
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

    BOOST_TEST(res.num()->isSymbol());
    BOOST_TEST(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    BOOST_CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    BOOST_CHECK_EQUAL(expectedDenom, backReplaced);
}

BOOST_AUTO_TEST_CASE(fractionBaseNumericallyEvaluablePosExp)
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

    BOOST_TEST(res.num()->isSymbol());
    BOOST_TEST(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    BOOST_CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    BOOST_CHECK_EQUAL(expectedDenom, backReplaced);
}

BOOST_AUTO_TEST_CASE(symbolicFractionBaseNumericallyEvaluablePosExp)
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

    BOOST_TEST(res.num()->isSymbol());
    BOOST_TEST(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    BOOST_CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    BOOST_CHECK_EQUAL(expectedDenom, backReplaced);
}

BOOST_AUTO_TEST_CASE(powerWithMinusSymbolExp)
/* (a + b)^(-c) becomes tmp/1, too. */
{
    const BasePtr minusC = Product::minus(c);
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(abSum);
    pn.setExponent(minusC);

    res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom());
    BOOST_TEST(res.num()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    BOOST_CHECK_EQUAL(Power::create(abSum, minusC), backReplaced);
}

BOOST_AUTO_TEST_CASE(powerWithPiExp)
/* a^Pi becomes tmp/1 with tmp = a^Pi. */
{
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(pi);

    res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom());
    BOOST_TEST(res.num()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.num());

    BOOST_CHECK_EQUAL(Power::create(a, pi), backReplaced);
}

BOOST_AUTO_TEST_CASE(powerWithNegNumEvalExp)
/* a^(-3*sqrt(2)*Pi) becomes 1/tmp with tmp = a^(3*sqrt(2)*Pi). */
{
    const BasePtr pos = Product::create(three, pi, Power::sqrt(two));
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(a);
    pn.setExponent(Product::minus(pos));

    res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.num());
    BOOST_TEST(res.denom()->isSymbol());

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom());

    BOOST_CHECK_EQUAL(Power::create(a, pos), backReplaced);
}

BOOST_AUTO_TEST_CASE(rationalBaseZero)
{
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(zeroByNormal);
    pn.setExponent(pi);

    res = pn.normal();
    backReplaced = map->replaceTmpSymbolsBackFrom(res.eval());

    BOOST_CHECK_EQUAL(zero, backReplaced);
}

BOOST_AUTO_TEST_CASE(rationalBaseOne)
{
    const BasePtr base = Sum::create(zeroByNormal, one);
    BasePtr backReplaced;
    PowerNormal pn(*map);
    Fraction res;

    pn.setBase(base);
    pn.setExponent(pi);

    res = pn.normal();
    backReplaced = map->replaceTmpSymbolsBackFrom(res.eval());

    BOOST_CHECK_EQUAL(one, backReplaced);
}

BOOST_AUTO_TEST_CASE(rationalBaseUndefined)
{
    const BasePtr base = Power::oneOver(zeroByNormal);
    PowerNormal pn(*map);
    BasePtr res;

    pn.setBase(base);
    pn.setExponent(pi);

    res = pn.normal().eval();

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(rationalBaseUndefinedByExpansion)
{
    const BasePtr base = Power::oneOver(zeroByExpansion);
    PowerNormal pn(*map);
    BasePtr res;

    pn.setBase(base);
    pn.setExponent(a);

    res = pn.normal().eval();

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_SUITE_END()
