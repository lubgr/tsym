
#include <memory>
#include "basefct.h"
#include "constant.h"
#include "fixtures.h"
#include "fraction.h"
#include "logarithm.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbolmap.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct NormalFixture : public AbcFixture {
    const BasePtr denom = Power::oneOver(Sum::create(b, c));
    const BasePtr argToZero =
      Sum::create(a, Product::create(Numeric::mOne(), a, b, denom), Product::create(Numeric::mOne(), a, c, denom));

    std::unique_ptr<SymbolMap> map{std::make_unique<SymbolMap>()};
};

BOOST_FIXTURE_TEST_SUITE(TestNormal, NormalFixture)

BOOST_AUTO_TEST_CASE(numericInteger)
{
    BOOST_CHECK_EQUAL(two, two->normal());
}

BOOST_AUTO_TEST_CASE(numericDouble)
{
    const BasePtr d = Numeric::create(1.234567);

    BOOST_CHECK_EQUAL(d, d->normal());
}

BOOST_AUTO_TEST_CASE(numericFraction)
{
    const BasePtr frac = Numeric::third();

    BOOST_CHECK_EQUAL(frac, frac->normal());
}

BOOST_AUTO_TEST_CASE(symbol)
{
    BOOST_CHECK_EQUAL(a, a->normal());
}

BOOST_AUTO_TEST_CASE(constantPi)
{
    BOOST_CHECK_EQUAL(pi, pi->normal());
}

BOOST_AUTO_TEST_CASE(constantE)
{
    const BasePtr e = Constant::createE();

    BOOST_CHECK_EQUAL(e, e->normal());
}

BOOST_AUTO_TEST_CASE(undefinedToUndefined)
{
    BOOST_TEST(undefined->normal()->isUndefined());
}

BOOST_AUTO_TEST_CASE(powerWithPosIntExp)
{
    const BasePtr orig = Power::create(a, two);

    BOOST_CHECK_EQUAL(orig, orig->normal());
}

BOOST_AUTO_TEST_CASE(powerWithPosNegExp)
{
    const BasePtr orig = Power::create(a, Numeric::create(-2));

    BOOST_CHECK_EQUAL(orig, orig->normal());
}

BOOST_AUTO_TEST_CASE(powerWithSymbolExp)
{
    const BasePtr orig = Power::create(Sum::create(a, b), c);

    BOOST_CHECK_EQUAL(orig, orig->normal());
}

BOOST_AUTO_TEST_CASE(powerWithMinusSymbolExp)
{
    const BasePtr orig = Power::create(Sum::create(a, b), Product::minus(c));

    BOOST_CHECK_EQUAL(orig, orig->normal());
}

BOOST_AUTO_TEST_CASE(powerWithPiExp)
{
    const BasePtr orig = Power::create(a, pi);

    BOOST_CHECK_EQUAL(orig, orig->normal());
}

BOOST_AUTO_TEST_CASE(powerWithNegNumEvalExp)
{
    const BasePtr pos = Product::create(three, pi, Power::sqrt(two));
    const BasePtr orig = Power::create(a, Product::minus(pos));

    BOOST_CHECK_EQUAL(orig, orig->normal());
}

BOOST_AUTO_TEST_CASE(simpleProduct)
{
    const BasePtr orig = Product::create(a, Power::oneOver(b));
    const Fraction frac = orig->normal(*map);

    BOOST_CHECK_EQUAL(a, frac.num);
    BOOST_CHECK_EQUAL(b, frac.denom);
}

BOOST_AUTO_TEST_CASE(product)
/* (1/4)*(2*c + 2*b^2 + a*b^2 + a*c)(2*a + a^2)^(-1) becomes 1/4*a^(-1)*(b^2 + c). */
{
    const BasePtr num =
      Sum::create(Product::create(two, c), Product::create(two, b, b), Product::create(a, b, b), Product::create(a, c));
    const BasePtr denom = Sum::create(Product::create(two, a), Power::create(a, two));
    const BasePtr orig = Product::create(Numeric::fourth(), num, Power::oneOver(denom));
    const BasePtr expected =
      Product::create(Numeric::fourth(), Sum::create(Power::create(b, two), c), Power::oneOver(a));
    const BasePtr expectedNum = Sum::create(Power::create(b, two), c);
    const BasePtr expectedDenom = Product::create(four, a);
    Fraction frac;

    frac = orig->normal(*map);

    BOOST_CHECK_EQUAL(expectedNum, frac.num);
    BOOST_CHECK_EQUAL(expectedDenom, frac.denom);
    BOOST_CHECK_EQUAL(expected, orig->normal());
}

BOOST_AUTO_TEST_CASE(simpleSum)
/* a/b + 2/b becomes (2 + a)/b. */
{
    const BasePtr oneOverB = Power::oneOver(b);
    const BasePtr expected = Product::create(Sum::create(two, a), oneOverB);
    const BasePtr orig = Sum::create(Product::create(a, oneOverB), Product::create(two, oneOverB));
    const BasePtr result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleSumToZero)
{
    const BasePtr result = argToZero->normal();

    BOOST_TEST(isZero(*result));
}

BOOST_AUTO_TEST_CASE(simpleSumWithFractionCoeff)
/* a/b + 1/(5*b) becomes 1/5*(1 + 5*a)/b. */
{
    const BasePtr oneOverB = Power::oneOver(b);
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr expected = Product::create(oneFifth, oneOverB, Sum::create(one, Product::create(five, a)));
    const BasePtr orig = Sum::create(Product::create(a, oneOverB), Product::create(oneFifth, oneOverB));
    BasePtr result;

    result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(sum)
/* sqrt(2)*a/b + c/(sqrt(2)*d) - sin(a)/b becomes
 * (2*a*d + b*c - sqrt(2)*d*sin(a))/(sqrt(2)*b*d). */
{
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr oneOverB = Power::oneOver(b);
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr orig = Sum::create(Product::create(sqrtTwo, a, oneOverB),
      Product::create(c, Power::oneOver(sqrtTwo), Power::oneOver(d)), Product::minus(sinA, oneOverB));
    const BasePtr expected = Product::create(Power::oneOver(Product::create(sqrtTwo, b, d)),
      Sum::create(Product::create(two, a, d), Product::create(b, c), Product::minus(sqrtTwo, d, sinA)));
    BasePtr result;

    result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(longSimpleSum)
/* (1/a)* (a + b - b*(c - (a + b)*c/a + d)/(-b*c/a + d)) becomes 1. */
{
    const BasePtr oneOverA = Power::oneOver(a);
    const BasePtr tmp1 = Sum::create(d, Product::minus(b, c, oneOverA));
    const BasePtr tmp2 = Sum::create(c, Product::minus(c, oneOverA, Sum::create(a, b)), d);
    const BasePtr num = Sum::create(a, b, Product::minus(b, tmp2, Power::oneOver(tmp1)));
    const BasePtr orig = Product::create(oneOverA, num);
    BasePtr result;

    result = orig->normal();

    BOOST_CHECK_EQUAL(one, result);
}

BOOST_AUTO_TEST_CASE(replacementOfFunctionWithNumPowerArg)
/* sin(sqrt(3))^(-1) is rationalized to 1/tmp with tmp = sin(sqrt(3)). */
{
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sin = Trigonometric::createSin(sqrtThree);
    const BasePtr orig = Power::oneOver(sin);
    const Fraction frac = orig->normal(*map);
    BasePtr denom;

    BOOST_CHECK_EQUAL(one, frac.num);
    BOOST_TEST(frac.denom->isSymbol());

    denom = map->replaceTmpSymbolsBackFrom(frac.denom);

    BOOST_CHECK_EQUAL(sin, denom);
}

BOOST_AUTO_TEST_CASE(fractionsOfNumPowersAndFunctions01)
/* sqrt(2)/sin(sqrt(3)) + asin(a)/cos(b) becomes
 * (sqrt(2)*cos(b) + sin(sqrt(3))*asin(a))/(sin(sqrt3)*cos(b)). */
{
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sin = Trigonometric::createSin(Power::sqrt(three));
    const BasePtr cos = Trigonometric::createCos(b);
    const BasePtr asin = Trigonometric::createAsin(a);
    const BasePtr orig =
      Sum::create(Product::create(sqrtTwo, Power::oneOver(sin)), Product::create(asin, Power::oneOver(cos)));
    const BasePtr expectedNum = Sum::create(Product::create(sqrtTwo, cos), Product::create(sin, asin));
    const BasePtr expectedDenom = Product::create(sin, cos);
    const BasePtr expected = Product::create(expectedNum, Power::oneOver(expectedDenom));
    BasePtr result;

    result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(fractionsOfNumPowersAndFunctions02)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sqrtSix = Power::sqrt(six);
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr f1 = Power::oneOver(Product::create(three, sqrtTwo));
    const BasePtr f2 = Product::create(sinA, Power::oneOver(sqrtThree));
    const BasePtr f3 = Product::create(sqrtSix, Power::oneOver(aSquare));
    const BasePtr orig = Sum::create(f1, f2, f3);
    const BasePtr expectedNum = Sum::create(
      Product::create(sqrtThree, aSquare), Product::create(three, sqrtTwo, sinA, aSquare), Numeric::create(18));
    const BasePtr expectedDenom = Product::create(three, sqrtSix, aSquare);
    const BasePtr expected = Product::create(expectedNum, Power::oneOver(expectedDenom));
    BasePtr result;

    result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(fractionSumWithPiExp)
/* (a/b)^(-Pi) + 1/(a^Pi) becomes (1 + b^Pi)/a^Pi.*/
{
    const BasePtr f1 = Power::create(Product::create(a, Power::oneOver(b)), Product::minus(pi));
    const BasePtr f2 = Power::oneOver(Power::create(a, pi));
    const BasePtr orig = Sum::create(f1, f2);
    const BasePtr expected =
      Product::create(Sum::create(one, Power::create(b, pi)), Power::oneOver(Power::create(a, pi)));
    BasePtr result;

    result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(sumWithNumPowerOfFractions)
/* (3/4)^(-1/5) + 3^(1/5) becomes 2^(2/5)/3^(1/5) + 3^(1/5).*/
{
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr mOneFifth = Numeric::create(-1, 5);
    const BasePtr f1 = Power::create(Numeric::create(3, 4), mOneFifth);
    const BasePtr f2 = Power::create(three, oneFifth);
    const BasePtr orig = Sum::create(f1, f2);
    const BasePtr expected =
      Sum::create(Product::create(Power::create(two, Numeric::create(2, 5)), Power::create(three, mOneFifth)),
        Power::create(three, oneFifth));
    BasePtr result;

    result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2NormalizeFunctionArgs)
{
    const BasePtr orig = Trigonometric::createAtan2(Sum::create(b, argToZero), a);
    const BasePtr result = orig->normal();
    const Name expectedFctName{"atan2"};

    BOOST_TEST(result->isFunction());
    BOOST_CHECK_EQUAL(expectedFctName, result->name());

    BOOST_CHECK_EQUAL(b, result->operands().front());
    BOOST_CHECK_EQUAL(a, result->operands().back());
}

BOOST_AUTO_TEST_CASE(trigonometricFunctionArgToZero)
{
    const BasePtr orig = Trigonometric::createSin(argToZero);
    const BasePtr result = orig->normal();

    BOOST_TEST(isZero(*result));
}

BOOST_AUTO_TEST_CASE(logarithmicFunctionArgToZero, noLogs())
{
    const BasePtr orig = Logarithm::create(argToZero);
    BasePtr result;

    result = orig->normal();

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logarithmicFunction)
{
    const BasePtr orig = Logarithm::create(Product::create(two, a));
    const BasePtr result = orig->normal();

    BOOST_CHECK_EQUAL(orig, result);
}

BOOST_AUTO_TEST_CASE(trigoPowerSumComposite)
/* -2*a*sin(a^2)/sqrt(1 - cos(a^2)^2) doesn't change. */
{
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr cosASquare = Trigonometric::createCos(aSquare);
    const BasePtr orig = Product::minus(two, a, Trigonometric::createSin(aSquare),
      Power::create(Sum::create(one, Product::minus(cosASquare, cosASquare)), Numeric::create(-1, 2)));
    const BasePtr result = orig->normal();

    BOOST_CHECK_EQUAL(orig, result);
}

BOOST_AUTO_TEST_CASE(tanToSinOverCos)
/* 1/cos(a) + tan(a) becomes (1 + sin(a))/cos(a). */
{
    const BasePtr cosA = Trigonometric::createCos(a);
    const BasePtr orig = Sum::create(Power::oneOver(cosA), Trigonometric::createTan(a));
    const BasePtr result = orig->normal();
    const BasePtr expected = Product::create(Sum::create(one, Trigonometric::createSin(a)), Power::oneOver(cosA));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(tanInDenominator)
/* cos(a)/(12*b) + a*b/tan(a) becomes (12*a*b^2 + sin(a))/(12*b*tan(a)). */
{
    const BasePtr twelveB = Product::create(Numeric::create(12), b);
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr cosA = Trigonometric::createCos(a);
    const BasePtr tanA = Trigonometric::createTan(a);
    const BasePtr orig =
      Sum::create(Product::create(cosA, Power::oneOver(twelveB)), Product::create(a, b, Power::oneOver(tanA)));
    const BasePtr expected = Product::create(
      Sum::create(sinA, Product::create(twelveB, a, b)), Power::oneOver(Product::create(twelveB, tanA)));
    const BasePtr result = orig->normal();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_SUITE_END()
