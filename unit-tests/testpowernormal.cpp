
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

BOOST_AUTO_TEST_CASE(powerWithPosIntExp)
{
    const PowerNormal pn(*a, *two, *map);
    const Fraction res = pn.normal();

    BOOST_CHECK_EQUAL(Power::create(a, two), res.num);
    BOOST_CHECK_EQUAL(one, res.denom);
}

BOOST_AUTO_TEST_CASE(powerWithPosNegExp)
{
    const PowerNormal pn(*a, *Numeric::create(-2), *map);
    const Fraction res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.num);
    BOOST_CHECK_EQUAL(Power::create(a, two), res.denom);
}

BOOST_AUTO_TEST_CASE(powerWithSymbolExp)
/* (a + b)^c becomes tmp/1. */
{
    const BasePtr orig = Power::create(abSum, c);
    const PowerNormal pn(*Sum::create(a, b), *c, *map);
    const Fraction res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom);
    BOOST_TEST(res.num->isSymbol());

    const BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(res.num);

    BOOST_CHECK_EQUAL(orig, backReplaced);
}

BOOST_AUTO_TEST_CASE(fractionBaseSymbolExp)
/* (3/4)^a becomes tmp1/1. */
{
    const BasePtr base = Numeric::create(3, 4);
    const PowerNormal pn(*base, *a, *map);
    const Fraction res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom);
    BOOST_TEST(res.num->isSymbol());
}

BOOST_AUTO_TEST_CASE(fractionBaseNumericallyEvaluableNegExp)
/* (3/4)^(-sin(1)) = tmp1/tmp2 with tmp1 = 4^sin(1) and tmp2 = 3^sin(1). */
{
    const BasePtr sinOne = Trigonometric::createSin(one);
    const BasePtr expectedNum = Power::create(four, sinOne);
    const BasePtr expectedDenom = Power::create(three, sinOne);
    const BasePtr exp = Product::minus(sinOne);
    const BasePtr base = Numeric::create(3, 4);
    const PowerNormal pn(*base, *exp, *map);
    const Fraction res = pn.normal();

    BOOST_TEST(res.num->isSymbol());
    BOOST_TEST(res.denom->isSymbol());

    BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(res.num);

    BOOST_CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom);

    BOOST_CHECK_EQUAL(expectedDenom, backReplaced);
}

BOOST_AUTO_TEST_CASE(fractionBaseNumericallyEvaluablePosExp)
/* (2/5)^sqrt(2) = tmp1/tmp2 with tmp1 = 2^sqrt(2) and tmp2 = 5^sqrt(2). */
{
    const BasePtr exp = Power::sqrt(two);
    const BasePtr expectedNum = Power::create(two, exp);
    const BasePtr expectedDenom = Power::create(five, exp);
    const PowerNormal pn(*Numeric::create(2, 5), *exp, *map);
    const Fraction res = pn.normal();

    BOOST_TEST(res.num->isSymbol());
    BOOST_TEST(res.denom->isSymbol());

    BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(res.num);

    BOOST_CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom);

    BOOST_CHECK_EQUAL(expectedDenom, backReplaced);
}

BOOST_AUTO_TEST_CASE(symbolicFractionBaseNumericallyEvaluablePosExp)
/* (a/b)^sqrt(2) = tmp1/tmp2 with tmp1 = a^sqrt(2) and tmp2 = b^sqrt(2). */
{
    const BasePtr base = Product::create(a, Power::oneOver(b));
    const BasePtr exp = Power::sqrt(two);
    const BasePtr expectedNum = Power::create(a, exp);
    const BasePtr expectedDenom = Power::create(b, exp);
    const PowerNormal pn(*base, *exp, *map);
    const Fraction res = pn.normal();

    BOOST_TEST(res.num->isSymbol());
    BOOST_TEST(res.denom->isSymbol());

    BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(res.num);

    BOOST_CHECK_EQUAL(expectedNum, backReplaced);

    backReplaced = map->replaceTmpSymbolsBackFrom(res.denom);

    BOOST_CHECK_EQUAL(expectedDenom, backReplaced);
}

BOOST_AUTO_TEST_CASE(powerWithMinusSymbolExp)
/* (a + b)^(-c) becomes tmp/1, too. */
{
    const BasePtr minusC = Product::minus(c);
    const PowerNormal pn(*abSum, *minusC, *map);
    const Fraction res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom);
    BOOST_TEST(res.num->isSymbol());

    BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(res.num);

    BOOST_CHECK_EQUAL(Power::create(abSum, minusC), backReplaced);
}

BOOST_AUTO_TEST_CASE(powerWithPiExp)
/* a^Pi becomes tmp/1 with tmp = a^Pi. */
{
    const PowerNormal pn(*a, *pi, *map);
    const Fraction res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.denom);
    BOOST_TEST(res.num->isSymbol());

    BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(res.num);

    BOOST_CHECK_EQUAL(Power::create(a, pi), backReplaced);
}

BOOST_AUTO_TEST_CASE(powerWithNegNumEvalExp)
/* a^(-3*sqrt(2)*Pi) becomes 1/tmp with tmp = a^(3*sqrt(2)*Pi). */
{
    const BasePtr pos = Product::create(three, pi, Power::sqrt(two));
    const PowerNormal pn(*a, *Product::minus(pos), *map);
    const Fraction res = pn.normal();

    BOOST_CHECK_EQUAL(one, res.num);
    BOOST_TEST(res.denom->isSymbol());

    BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(res.denom);

    BOOST_CHECK_EQUAL(Power::create(a, pos), backReplaced);
}

BOOST_AUTO_TEST_CASE(rationalBaseZero)
{
    const PowerNormal pn(*zeroByNormal, *pi, *map);
    const Fraction res = pn.normal();
    const BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(eval(res));

    BOOST_CHECK_EQUAL(zero, backReplaced);
}

BOOST_AUTO_TEST_CASE(rationalBaseOne)
{
    const BasePtr base = Sum::create(zeroByNormal, one);
    const PowerNormal pn(*base, *pi, *map);
    const Fraction res = pn.normal();
    const BasePtr backReplaced = map->replaceTmpSymbolsBackFrom(eval(res));

    BOOST_CHECK_EQUAL(one, backReplaced);
}

BOOST_AUTO_TEST_CASE(rationalBaseUndefined)
{
    const BasePtr base = Power::oneOver(zeroByNormal);
    const PowerNormal pn(*base, *pi, *map);
    const BasePtr res = eval(pn.normal());

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(rationalBaseUndefinedByExpansion)
{
    const BasePtr base = Power::oneOver(zeroByExpansion);
    const PowerNormal pn(*base, *a, *map);
    const BasePtr res = eval(pn.normal());

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_SUITE_END()
