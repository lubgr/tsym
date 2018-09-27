
#include "basefct.h"
#include "baseptr.h"
#include "constant.h"
#include "fixtures.h"
#include "logarithm.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestSubst, AbcFixture)

BOOST_AUTO_TEST_CASE(undefinedToSymbol)
{
    const BasePtr res = undefined->subst(*undefined, a);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(symbolToNumeric)
{
    const BasePtr res = a->subst(*a, two);

    BOOST_CHECK_EQUAL(two, res);
}

BOOST_AUTO_TEST_CASE(symbolNotToNumeric)
{
    const BasePtr res = a->subst(*b, two);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(constantToSum)
{
    const BasePtr sum = Sum::create(a, b);
    BasePtr res;

    res = pi->subst(*pi, sum);

    BOOST_CHECK_EQUAL(sum, res);
}

BOOST_AUTO_TEST_CASE(constantNotToNumeric)
{
    const BasePtr res = pi->subst(*a, two);

    BOOST_CHECK_EQUAL(pi, res);
}

BOOST_AUTO_TEST_CASE(numericToSymbol)
{
    const BasePtr orig = Numeric::create(5, 17);
    BasePtr res;

    res = orig->subst(*orig, a);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(powerToSum)
{
    const BasePtr sum = Sum::create(a, b, c);
    const BasePtr orig = Power::create(a, two);
    BasePtr res;

    res = orig->subst(*orig, sum);

    BOOST_CHECK_EQUAL(sum, res);
}

BOOST_AUTO_TEST_CASE(productToPower)
{
    const BasePtr pow = Power::create(a, b);
    const BasePtr orig = Product::create(a, b);
    BasePtr res;

    res = orig->subst(*orig, pow);

    BOOST_CHECK_EQUAL(pow, res);
}

BOOST_AUTO_TEST_CASE(sumToSymbol)
{
    const BasePtr orig = Sum::create(a, b);
    BasePtr res;

    res = orig->subst(*orig, a);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(undefinedNoChange)
{
    const BasePtr result = undefined->subst(*a, b);

    BOOST_TEST(isUndefined(*result));
}

BOOST_AUTO_TEST_CASE(cosineToConstant)
{
    const BasePtr orig = Trigonometric::createCos(a);
    BasePtr res;

    res = orig->subst(*orig, pi);

    BOOST_CHECK_EQUAL(pi, res);
}

BOOST_AUTO_TEST_CASE(atanArgToConstant)
{
    const BasePtr orig = Trigonometric::createAtan(a);
    const BasePtr expected = Trigonometric::createAtan(pi);
    BasePtr res;

    res = orig->subst(*a, pi);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sinToZero)
/* Sin(a) = 0 for a = Pi. */
{
    const BasePtr orig = Trigonometric::createSin(a);
    const BasePtr res = orig->subst(*a, pi);

    BOOST_TEST(isZero(*res));
}

BOOST_AUTO_TEST_CASE(asinToUndefined)
/* Asin(a) = Undefined for |a| > 1. */
{
    const BasePtr orig = Trigonometric::createAsin(a);
    const BasePtr tmp = Numeric::create(1.23456789);
    BasePtr res;

    res = orig->subst(*a, tmp);

    BOOST_TEST(isUndefined(*res));
}

BOOST_AUTO_TEST_CASE(atan2ToPiFourth)
/* Atan(b, a) = Pi/4 for b = 2 and a = 2. */
{
    const BasePtr orig = Trigonometric::createAtan2(b, a);
    const BasePtr firstSubst = orig->subst(*b, two);
    const BasePtr expected = Product::create(Numeric::fourth(), pi);
    BasePtr secondSubst;

    BOOST_TEST(isFunction(*firstSubst));

    secondSubst = firstSubst->subst(*a, two);

    BOOST_CHECK_EQUAL(expected, secondSubst);
}

BOOST_AUTO_TEST_CASE(logToLog)
/* Log(a) = log(b) for a = b. */
{
    const BasePtr expected = Logarithm::create(b);
    const BasePtr orig = Logarithm::create(a);
    BasePtr res;

    res = orig->subst(*a, b);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(logToZero)
/* Log(a) = 0 for a = 1. */
{
    const BasePtr orig = Logarithm::create(a);
    BasePtr res;

    res = orig->subst(*a, one);

    BOOST_TEST(isZero(*res));
}

BOOST_AUTO_TEST_CASE(logEqualArg)
{
    const BasePtr orig = Logarithm::create(a);
    BasePtr res;

    res = orig->subst(*orig, a);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(powerToUndefined, noLogs())
/* a^(-2) = Undefined for a = 0. */
{
    const BasePtr orig = Power::create(a, Numeric::create(-2));
    BasePtr res;

    res = orig->subst(*a, zero);

    BOOST_TEST(isUndefined(*res));
}

BOOST_AUTO_TEST_CASE(sumToUndefined)
/* a + b + c + d = Undefined for c = Undefined. */
{
    const BasePtr orig = Sum::create(a, b, c, d);
    const BasePtr res = orig->subst(*c, undefined);

    BOOST_TEST(isUndefined(*res));
}

BOOST_AUTO_TEST_CASE(sumToOne)
/* a + b^a + a*(b + c) = 1 for a = 0. */
{
    const BasePtr orig = Sum::create(a, Power::create(b, a), Product::create(a, Sum::create(b, c)));
    BasePtr res;

    res = orig->subst(*a, zero);

    BOOST_CHECK_EQUAL(one, res);
}

BOOST_AUTO_TEST_CASE(noSubExpressionInSum)
/* a + b + c stays constant for a + b = 2. */
{
    const BasePtr abSum = Sum::create(a, b);
    const BasePtr orig = Sum::create(abSum, c);
    BasePtr res;

    res = orig->subst(*abSum, two);

    BOOST_CHECK_EQUAL(orig, res);
}

BOOST_AUTO_TEST_CASE(noSubExpressionInProduct)
/* 2*a*b stays constant for a*b = 3. */
{
    const BasePtr abProduct = Product::create(a, b);
    const BasePtr orig = Product::create(two, abProduct);
    BasePtr res;

    res = orig->subst(*abProduct, three);

    BOOST_CHECK_EQUAL(orig, res);
}

BOOST_AUTO_TEST_CASE(powerExpanedBySubst)
/* (a*b + c + d)^2= a^2*b^2 for c = d = 0. */
{
    const BasePtr expected = Product::create(Power::create(a, two), Power::create(b, two));
    const BasePtr abProduct = Product::create(a, b);
    const BasePtr base = Sum::create(abProduct, c, d);
    const BasePtr orig = Power::create(base, two);
    BasePtr res;

    res = orig->subst(*c, zero)->subst(*d, zero);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(numPowerSimplInSubstitution)
{
    const BasePtr orig = Product::create(two, a, Trigonometric::createCos(b));
    const BasePtr result = orig->subst(*b, Product::create(Numeric::create(5, 4), pi));
    const BasePtr expected = Product::minus(Power::sqrt(two), a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(mixedTerm)
/* 2*sqrt(3)*(a + b + sqrt(3))*(c + d)/a = 4*17^(1/3)*a + 2*17^(1/3)*b
 * for sqrt(3) = a and c + d = 17^(1/3). */
{
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr numPow = Power::create(Numeric::create(17), Numeric::third());
    const BasePtr expected = Sum::create(Product::create(four, numPow, a), Product::create(two, numPow, b));
    BasePtrList fac;
    BasePtr orig;
    BasePtr res;

    fac.push_back(two);
    fac.push_back(sqrtThree);
    fac.push_back(Sum::create(a, b, sqrtThree));
    fac.push_back(Sum::create(b, c));
    fac.push_back(Power::oneOver(a));

    orig = Product::create(fac);

    res = orig->subst(*sqrtThree, a)->subst(*Sum::create(b, c), numPow);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_SUITE_END()
