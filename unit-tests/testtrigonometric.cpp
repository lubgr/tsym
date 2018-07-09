
#include <cmath>
#include "constant.h"
#include "fixtures.h"
#include "logarithm.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct TrigonometricFixture : public AbcFixture {
    const BasePtr minusHalf = Numeric::create(-1, 2);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr sinA = Trigonometric::createSin(a);
};

BOOST_FIXTURE_TEST_SUITE(TestTrigonometric, TrigonometricFixture)

BOOST_AUTO_TEST_CASE(typeRequest)
{
    BOOST_TEST(sinA->isFunction());
}

BOOST_AUTO_TEST_CASE(constRequest)
{
    const BasePtr symbolArg = Trigonometric::createCos(a);
    const BasePtr numericArg = Trigonometric::createCos(two);

    BOOST_TEST(!symbolArg->isConst());
    BOOST_TEST(!numericArg->isConst());
}

BOOST_AUTO_TEST_CASE(sinFunctionName)
{
    const Name expected("sin");

    BOOST_CHECK_EQUAL(expected, sinA->name());
}

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Function");

    BOOST_CHECK_EQUAL(expected, sinA->typeStr());
}

BOOST_AUTO_TEST_CASE(cosFunctionName)
{
    const BasePtr cos = Trigonometric::createCos(a);
    const Name expected("cos");

    BOOST_CHECK_EQUAL(expected, cos->name());
}

BOOST_AUTO_TEST_CASE(tanFunctionName)
{
    const BasePtr tan = Trigonometric::createTan(a);
    const Name expected("tan");

    BOOST_CHECK_EQUAL(expected, tan->name());
}

BOOST_AUTO_TEST_CASE(asinFunctionName)
{
    const BasePtr asin = Trigonometric::createAsin(a);
    const Name expected("asin");

    BOOST_CHECK_EQUAL(expected, asin->name());
}

BOOST_AUTO_TEST_CASE(acosFunctionName)
{
    const BasePtr acos = Trigonometric::createAcos(a);
    const Name expected("acos");

    BOOST_CHECK_EQUAL(expected, acos->name());
}

BOOST_AUTO_TEST_CASE(atanFunctionName)
{
    const BasePtr atan = Trigonometric::createAtan(a);
    const Name expected("atan");

    BOOST_CHECK_EQUAL(expected, atan->name());
}

BOOST_AUTO_TEST_CASE(atan2FunctionName)
{
    const BasePtr atan = Trigonometric::createAtan2(b, a);
    const Name expected("atan2");

    BOOST_CHECK_EQUAL(expected, atan->name());
}

BOOST_AUTO_TEST_CASE(trivialEquality)
{
    BOOST_TEST(sinA->isEqual(*sinA));
}

BOOST_AUTO_TEST_CASE(inequalityDifferentArg)
{
    const BasePtr sinB = Trigonometric::createSin(b);

    BOOST_TEST(sinA->isDifferent(*sinB));
}

BOOST_AUTO_TEST_CASE(inequalityDifferentTrigonometric)
{
    const BasePtr cosA = Trigonometric::createCos(a);

    BOOST_TEST(sinA->isDifferent(*cosA));
}

BOOST_AUTO_TEST_CASE(sinZero)
{
    const BasePtr res = Trigonometric::createSin(zero);

    BOOST_CHECK_EQUAL(zero, res);
}

BOOST_AUTO_TEST_CASE(cos345Degree)
/* Cos(23/12*pi) = (sqrt(6) + sqrt(2))/4. */
{
    const BasePtr arg = Product::create(Numeric::create(23, 12), pi);
    BasePtr res;
    const BasePtr expected = Product::create(Numeric::fourth(), Sum::create(Power::sqrt(six), sqrtTwo));

    res = Trigonometric::createCos(arg);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(tan240Degree)
/* Tan(4/3*Pi) = sqrt(3). */
{
    const BasePtr arg = Product::create(Numeric::create(4, 3), pi);
    const BasePtr res = Trigonometric::createTan(arg);

    BOOST_CHECK_EQUAL(sqrtThree, res);
}

BOOST_AUTO_TEST_CASE(atan2Order)
{
    const BasePtr atan2Ab = Trigonometric::createAtan2(a, b);
    const BasePtr atan2Ba = Trigonometric::createAtan2(b, a);

    BOOST_CHECK_EQUAL(a, atan2Ab->operands().front());
    BOOST_CHECK_EQUAL(a, atan2Ba->operands().back());

    BOOST_CHECK_EQUAL(b, atan2Ab->operands().back());
    BOOST_CHECK_EQUAL(b, atan2Ba->operands().front());
}

BOOST_AUTO_TEST_CASE(atan2XZeroNonEvaluable)
{
    const BasePtr res = Trigonometric::createAtan2(a, zero);

    BOOST_TEST(res->isFunction());
    BOOST_CHECK_EQUAL(a, res->operands().front());
    BOOST_CHECK_EQUAL(zero, res->operands().back());
}

BOOST_AUTO_TEST_CASE(atan2FirstQuadrantResolvable)
{
    const BasePtr arg = Product::create(Numeric::create(25, 180), pi);
    const BasePtr result = Trigonometric::createAtan2(Trigonometric::createSin(arg), Trigonometric::createCos(arg));

    BOOST_CHECK_EQUAL(arg, result);
}

BOOST_AUTO_TEST_CASE(atan2SecondQuadrantResolvable)
{
    const BasePtr result = Trigonometric::createAtan2(one, Numeric::mOne());
    const BasePtr expected = Product::create(pi, Numeric::create(3, 4));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2ThirdQuadrantResolvable)
{
    const BasePtr y = Product::create(Numeric::create(-3), Sum::create(one, sqrtTwo));
    const BasePtr result = Trigonometric::createAtan2(y, Numeric::create(-3));
    const BasePtr expected = Product::create(Numeric::create(11, 8), pi);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2FourthQuadrantNonResolvable)
{
    const BasePtr expected = Product::minus(Trigonometric::createAtan(Numeric::half()));
    const BasePtr result = Trigonometric::createAtan2(one, Numeric::create(-2));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2FourthQuadrantResolvable)
{
    const BasePtr sqrtFive = Power::sqrt(five);
    const BasePtr result = Trigonometric::createAtan2(Product::minus(sqrtFive), sqrtFive);
    const BasePtr expected = Product::create(pi, Numeric::create(7, 4));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2XZeroEvaluable)
{
    const BasePtr negativeX = Trigonometric::createAtan2(Product::minus(two, sqrtTwo), zero);
    const BasePtr positiveX = Trigonometric::createAtan2(sqrtTwo, zero);

    BOOST_CHECK_EQUAL(Product::create(Numeric::half(), pi), positiveX);
    BOOST_CHECK_EQUAL(Product::create(Numeric::create(3, 2), pi), negativeX);
}

BOOST_AUTO_TEST_CASE(pureNumericAtan2WithRangeCorretion)
{
    const double numX = -1.2345678;
    const double numY = -12.987654;
    const BasePtr x = Numeric::create(numX);
    const BasePtr y = Numeric::create(numY);
    const BasePtr result = Trigonometric::createAtan2(y, x);
    const BasePtr expected = Numeric::create(std::atan2(numY, numX) + 2.0 * M_PI);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(pureNumericAtan2NoRangeCorretion)
{
    const double numX = 1.2345678;
    const double numY = 7.6543456;
    const BasePtr x = Numeric::create(numX);
    const BasePtr y = Numeric::create(numY);
    const BasePtr result = Trigonometric::createAtan2(y, x);
    const BasePtr expected = Numeric::create(std::atan2(numY, numX));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(illegalAtan2, noLogs())
{
    BasePtr res;

    res = Trigonometric::createAtan2(zero, zero);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(undefinedArg)
{
    const BasePtr sin = Trigonometric::createSin(undefined);

    BOOST_TEST(sin->isUndefined());
}

BOOST_AUTO_TEST_CASE(undefinedArgAtan)
{
    BasePtr res = Trigonometric::createAtan2(undefined, a);

    BOOST_TEST(res->isUndefined());

    res = Trigonometric::createAtan2(a, undefined);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(sinOfAsin)
{
    const BasePtr asin = Trigonometric::createAsin(a);
    const BasePtr res = Trigonometric::createSin(asin);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(sinOfNegativeSum)
{
    const BasePtr arg = Sum::create(Product::minus(a, b, c), Product::create(Numeric::create(-7, 8), a),
      Product::minus(a, Trigonometric::createCos(b)));
    const BasePtr expected = Product::minus(Trigonometric::createSin(Product::minus(arg)));
    const BasePtr res = Trigonometric::createSin(arg);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sinOfNegativeProduct)
{
    const BasePtr arg = Product::create(Numeric::create(-1, 5), Power::create(two, Numeric::third()), a);
    const BasePtr res = Trigonometric::createSin(arg);
    const BasePtr expected = Product::minus(Trigonometric::createSin(Product::minus(arg)));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sinOfMinusAsin)
{
    const BasePtr mAsin = Product::minus(Trigonometric::createAsin(a));
    const BasePtr res = Trigonometric::createSin(mAsin);

    BOOST_CHECK_EQUAL(Product::minus(a), res);
}

BOOST_AUTO_TEST_CASE(cosOfAcos)
{
    const BasePtr acos = Trigonometric::createAcos(a);
    const BasePtr res = Trigonometric::createCos(acos);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(cosOfNegativeNumericArg)
{
    const BasePtr res = Trigonometric::createCos(Numeric::create(-2));
    const BasePtr expected = Trigonometric::createCos(two);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(cosOfNegativeProduct)
{
    const BasePtr arg = Product::minus(a, b, Trigonometric::createAtan(Sum::create(a, b)));
    const BasePtr res = Trigonometric::createCos(arg);
    const BasePtr expected = Trigonometric::createCos(Product::minus(arg));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(cosOfMinusAcos)
{
    const BasePtr arg = Sum::create(a, Product::create(two, b));
    const BasePtr mAcos = Product::minus(Trigonometric::createAcos(arg));
    const BasePtr res = Trigonometric::createCos(mAcos);

    BOOST_CHECK_EQUAL(arg, res);
}

BOOST_AUTO_TEST_CASE(tanOfAtan)
{
    const BasePtr atan = Trigonometric::createAtan(a);
    const BasePtr res = Trigonometric::createTan(atan);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(tanOfSimpleNegativeProduct)
{
    const BasePtr expected = Product::minus(Trigonometric::createTan(a));
    const BasePtr arg = Product::minus(a);
    const BasePtr res = Trigonometric::createTan(arg);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atanOfNegSimpleProduct)
{
    const BasePtr expected = Product::minus(Trigonometric::createAtan(a));
    const BasePtr res = Trigonometric::createAtan(Product::minus(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atanOfNegSum)
{
    const BasePtr arg = Sum::create(Product::minus(two, a, b, c), Product::create(Numeric::create(-7, 11), sqrtTwo),
      Product::minus(Logarithm::create(Sum::create(Numeric::create(-2, 3), a))));
    const BasePtr expected = Product::minus(Trigonometric::createAtan(Product::minus(arg)));
    const BasePtr res = Trigonometric::createAtan(arg);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(tanOfMinusAtan)
{
    const BasePtr mAtan = Product::minus(Trigonometric::createAtan(a));
    const BasePtr res = Trigonometric::createTan(mAtan);

    BOOST_CHECK_EQUAL(Product::minus(a), res);
}

BOOST_AUTO_TEST_CASE(tanOfMinusAsin)
/* Tan(-asin(a)) = -a/sqrt(1 - a^2). */
{
    const BasePtr expected = Product::minus(a, Power::create(Sum::create(one, Product::minus(a, a)), minusHalf));
    const BasePtr mAsin = Product::minus(Trigonometric::createAsin(a));
    const BasePtr res = Trigonometric::createTan(mAsin);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atanOfTan)
{
    const BasePtr tan = Trigonometric::createTan(a);
    const BasePtr atan = Trigonometric::createAtan(tan);
    const Name expected("atan");

    BOOST_TEST(atan->isFunction());
    BOOST_CHECK_EQUAL(expected, atan->name());
    BOOST_CHECK_EQUAL(tan, atan->operands().front());
}

BOOST_AUTO_TEST_CASE(sinOfCos)
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr res = Trigonometric::createCos(sin);
    BasePtr arg;

    BOOST_TEST(res->isFunction());

    arg = res->operands().front();

    BOOST_TEST(arg->isFunction());
    BOOST_CHECK_EQUAL(a, arg->operands().front());
}

BOOST_AUTO_TEST_CASE(asinOfSin)
/* Asin(sin(a)) isn't simplified. */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr res = Trigonometric::createAsin(sin);
    BasePtr arg;

    BOOST_TEST(res->isFunction());

    arg = res->operands().front();

    BOOST_TEST(arg->isFunction());
    BOOST_CHECK_EQUAL(a, arg->operands().front());
}

BOOST_AUTO_TEST_CASE(asinOfMinusSin)
/* Asin(-sin(a)) = -asin(sin(a)). */
{
    const BasePtr expected = Product::minus(Trigonometric::createAsin(Trigonometric::createSin(a)));
    const BasePtr minusSin = Product::minus(Trigonometric::createSin(a));
    const BasePtr res = Trigonometric::createAsin(minusSin);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(acosOfCos)
/* Acos(cos(a)) isn't simplified. */
{
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(a));

    BOOST_TEST(res->isFunction());
    BOOST_CHECK_EQUAL(1, res->operands().size());
    BOOST_CHECK_EQUAL(Trigonometric::createCos(a), res->operands().front());
}

BOOST_AUTO_TEST_CASE(acosOfMinusCos)
/* Acos(-cos(a)) = pi - acos(cos(a)). */
{
    const BasePtr expected = Sum::create(pi, Product::minus(Trigonometric::createAcos(Trigonometric::createCos(a))));
    const BasePtr minusCos = Product::minus(Trigonometric::createCos(a));
    const BasePtr res = Trigonometric::createAcos(minusCos);
    BasePtr arg;

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(asinOfSinOfNumEvalNoShift)
/* Asin(sin(-1/sqrt(5))) = -1/sqrt(5). */
{
    const BasePtr arg = Power::oneOver(Product::minus(Power::create(five, Numeric::half())));
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));

    BOOST_CHECK_EQUAL(arg, res);
}

BOOST_AUTO_TEST_CASE(asinOfSinOfNumEvalNoShift2)
/* Asin(sin(2/17)) = 2/17. */
{
    const BasePtr arg = Numeric::create(2, 17);
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr res = Trigonometric::createAsin(sin);

    BOOST_CHECK_EQUAL(arg, res);
}

BOOST_AUTO_TEST_CASE(asinOfSinOfNumEvalNegShift)
/*  Asin(sin(13/4*pi)) = -pi/4. */
{
    const BasePtr arg = Product::create(Numeric::create(13, 4), pi);
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));
    const BasePtr expected = Product::create(Numeric::create(-1, 4), pi);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(asinOfSinOfNumEvalNegShift2)
/* Asin(sin(12*sqrt(pi))) = 7*pi - 12*sqrt(pi). */
{
    const BasePtr arg = Product::create(Numeric::create(12), Power::sqrt(pi));
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr res = Trigonometric::createAsin(sin);
    const BasePtr expected = Sum::create(Product::create(seven, pi), Product::minus(arg));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(asinOfSinOfNumEvalPosShift)
/* Asin(sin(-79/7*pi + 2/15)) = 2/7*pi - 2/15. */
{
    const BasePtr arg = Sum::create(Numeric::create(2, 15), Product::create(Numeric::create(-79, 7), pi));
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));
    const BasePtr expected = Sum::create(Product::create(Numeric::create(2, 7), pi), Numeric::create(-2, 15));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(asinOfSinOfNumEvalPosShift2)
/* Asin(sin(-6/7*pi)) = -pi/7. */
{
    const BasePtr arg = Product::create(Numeric::create(-6, 7), pi);
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));
    const BasePtr expected = Product::create(Numeric::create(-1, 7), pi);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(asinOfMinusAsinNumEval)
/* Asin(-sin(1/2)) = -1/2. */
{
    const BasePtr mSin = Product::minus(Trigonometric::createSin(Numeric::half()));
    const BasePtr res = Trigonometric::createAsin(mSin);

    BOOST_CHECK_EQUAL(Numeric::create(-1, 2), res);
}

BOOST_AUTO_TEST_CASE(atanOfTanNumEvalNoShift)
/* Atan(tan(1/2)) = 1/2. */
{
    const BasePtr& half = Numeric::half();
    const BasePtr tan = Trigonometric::createTan(half);
    const BasePtr res = Trigonometric::createAtan(tan);

    BOOST_CHECK_EQUAL(half, res);
}

BOOST_AUTO_TEST_CASE(atanOfTanNumEvalWithNegShift)
/* Atan(tan(2)) = 2 - pi, after substitution. */
{
    const BasePtr expected = Sum::create(two, Product::minus(pi));
    const BasePtr tan = Trigonometric::createTan(a);
    const BasePtr res = Trigonometric::createAtan(tan);
    BasePtr replaced;

    BOOST_TEST(res->isFunction());
    BOOST_TEST(res->operands().front()->isFunction());

    replaced = res->subst(*a, two);

    BOOST_CHECK_EQUAL(expected, replaced);
}

BOOST_AUTO_TEST_CASE(atanOfTanNumEvalWithPosShift)
/* Atan(tan(-23/21*pi - sqrt(3))) = 19/21*pi - sqrt(3). */
{
    const BasePtr sqrt = Product::minus(Power::sqrt(three));
    const BasePtr arg = Sum::create(Product::create(Numeric::create(-23, 21), pi), sqrt);
    const BasePtr res = Trigonometric::createAtan(Trigonometric::createTan(arg));
    const BasePtr expected = Sum::create(Product::create(Numeric::create(19, 21), pi), sqrt);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atanOfMinusTanNumEval)
/* Atan(-tan(-1/2)) = 1/2. */
{
    const BasePtr mTan = Product::minus(Trigonometric::createTan(Numeric::create(-1, 2)));
    const BasePtr res = Trigonometric::createAtan(mTan);

    BOOST_CHECK_EQUAL(Numeric::half(), res);
}

BOOST_AUTO_TEST_CASE(acosOfCosNumEvalNoShift)
/* Acos(cos(1/2)) = 1/2. */
{
    const BasePtr& half = Numeric::half();
    const BasePtr cos = Trigonometric::createCos(half);
    const BasePtr res = Trigonometric::createAcos(cos);

    BOOST_CHECK_EQUAL(half, res);
}

BOOST_AUTO_TEST_CASE(acosOfCosNumEvalPosShift)
/* Acos(cos(-sqrt(21) - 3/8*pi)) = 13/8*pi - sqrt(21). */
{
    const BasePtr sqrt = Power::sqrt(Numeric::create(21));
    const BasePtr arg = Product::minus(Sum::create(sqrt, Product::create(Numeric::create(3, 8), pi)));
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr acos = Trigonometric::createAcos(cos);
    const BasePtr expected = Sum::create(Product::create(Numeric::create(13, 8), pi), Product::minus(sqrt));

    BOOST_CHECK_EQUAL(expected, acos);
}

BOOST_AUTO_TEST_CASE(acosOfCosNumEvalNegShift)
/* Acos(cos(11/7*pi + 2^(1/3) + 0.123456)) = 2^(1/3) + 0.123456 - 3/7*pi. */
{
    const BasePtr pow = Power::create(two, Numeric::third());
    const BasePtr sum = Sum::create(Numeric::create(0.123456), pow);
    const BasePtr arg = Sum::create(Product::create(Numeric::create(11, 7), pi), sum);
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(arg));
    const BasePtr expected = Sum::create(sum, Product::create(Numeric::create(-3, 7), pi));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(acosOfCosNumEvalNegShiftAndCorrection)
/* Acos(cos(37/11*pi)) = 7/11*pi. */
{
    const BasePtr arg = Product::create(Numeric::create(37, 11), pi);
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(arg));
    const BasePtr expected = Product::create(Numeric::create(7, 11), pi);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(acosOfCosNumEvalPosShiftAndCorrection)
/* Acos(cos(-sqrt(21) - pi)) = sqrt(21) - pi. */
{
    const BasePtr sqrt = Power::sqrt(Numeric::create(21));
    const BasePtr arg = Product::minus(Sum::create(sqrt, Product::minus(pi)));
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(arg));
    const BasePtr expected = Sum::create(sqrt, Product::minus(pi));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(acosOfNegativeCosNumEvalWithShift)
/* Acos(-cos(1/2)) = pi - 1/2. */
{
    const BasePtr arg = Product::minus(Trigonometric::createCos(Numeric::half()));
    const BasePtr res = Trigonometric::createAcos(arg);

    BOOST_CHECK_EQUAL(Sum::create(pi, Numeric::create(-1, 2)), res);
}

BOOST_AUTO_TEST_CASE(sinOfAcos)
/* Sin(acos(a)) = sqrt(1 - a^2). */
{
    const BasePtr expected = Power::sqrt(Sum::create(one, Product::minus(aSquare)));
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAcos(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(cosOfAsin)
/* Cos(asin(a)) = sqrt(1 - a^2). */
{
    const BasePtr expected = Power::sqrt(Sum::create(one, Product::minus(aSquare)));
    const BasePtr res = Trigonometric::createCos(Trigonometric::createAsin(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sinOfAtan)
/* Sin(atan(a)) = a/sqrt(a^2 + 1). */
{
    const BasePtr expected = Product::create(a, Power::create(Sum::create(aSquare, one), minusHalf));
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAtan(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(cosOfAtan)
/* Cos(atan(a)) = 1/sqrt(a^2 + 1). */
{
    const BasePtr expected = Power::create(Sum::create(Power::create(a, two), one), minusHalf);
    const BasePtr res = Trigonometric::createCos(Trigonometric::createAtan(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(tanOfAsin)
/* Tan(asin(a)) = a/sqrt(1 - a^2). */
{
    const BasePtr expected =
      Product::create(a, Power::create(Sum::create(one, Product::minus(Power::create(a, two))), minusHalf));
    const BasePtr res = Trigonometric::createTan(Trigonometric::createAsin(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(tanOfAcos)
/* Tan(asin(a)) = sqrt(1 - a^2)/a. */
{
    const BasePtr expected = Product::create(Power::sqrt(Sum::create(one, Product::minus(aSquare))), Power::oneOver(a));
    const BasePtr res = Trigonometric::createTan(Trigonometric::createAcos(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atan2OfSinCos)
/* Atan2(sin(a), cos(a)) isn't simplified. */
{
    const BasePtr cosA = Trigonometric::createCos(a);
    const BasePtr res = Trigonometric::createAtan2(sinA, cosA);

    BOOST_TEST(res->isFunction());
    BOOST_CHECK_EQUAL(sinA, res->operands().front());
    BOOST_CHECK_EQUAL(cosA, res->operands().back());
}

BOOST_AUTO_TEST_CASE(atan2OfNumericsNotResolvableArg)
/* Atan2(2/3, 7/11) = atan(22/21). */
{
    const BasePtr res = Trigonometric::createAtan2(Numeric::create(2, 3), Numeric::create(7, 11));
    const BasePtr expected = Trigonometric::createAtan(Numeric::create(22, 21));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atan2OfNumEvaluableNotResolvable)
{
    const BasePtr y = Logarithm::create(Trigonometric::createTan(Numeric::half()));
    const BasePtr x = Sum::create(Product::create(sqrtTwo, Constant::createE()), five, Trigonometric::createCos(two));
    const BasePtr res = Trigonometric::createAtan2(y, x);
    const BasePtr expected = Trigonometric::createAtan(Product::create(y, Power::oneOver(x)));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atan2OfNegNumEvaluableArg)
{
    const BasePtr res = Trigonometric::createAtan2(Numeric::create(-2, 3), seven);
    const BasePtr expected = Product::minus(Trigonometric::createAtan(Numeric::create(2, 21)));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atan2OfNegSymbolicArg)
/* No symmetry simplification should apply, as the argument(s) aren't clearly positive or
 * negative. */
{
    const BasePtr res = Trigonometric::createAtan2(Product::minus(a), b);

    BOOST_TEST(res->isFunction());
    BOOST_CHECK_EQUAL(Product::minus(a), res->operands().front());
    BOOST_CHECK_EQUAL(b, res->operands().back());
}

BOOST_AUTO_TEST_CASE(tanOfAtan2)
/* Tan(atan2(b, a)) = b/a. */
{
    const BasePtr atan2 = Trigonometric::createAtan2(b, a);
    const BasePtr result = Trigonometric::createTan(atan2);
    const BasePtr expected = Product::create(b, Power::oneOver(a));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(cosOfAtan2)
/* Cos(atan2(b, a)) = a/sqrt(a^2 + b^2). */
{
    const BasePtr atan2 = Trigonometric::createAtan2(b, a);
    const BasePtr result = Trigonometric::createCos(atan2);
    const BasePtr expected =
      Product::create(a, Power::create(Sum::create(aSquare, Power::create(b, two)), Numeric::create(-1, 2)));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(sinOfAtan2)
/* Sin(atan2(b, a)) = b/sqrt(a^2 + b^2). */
{
    const BasePtr atan2 = Trigonometric::createAtan2(b, a);
    const BasePtr result = Trigonometric::createSin(atan2);
    const BasePtr expected =
      Product::create(b, Power::create(Sum::create(aSquare, Power::create(b, two)), Numeric::create(-1, 2)));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(sinOfAtan2NegDeltaYOnly)
/* Sin(atan2(-a, 0)) = -1 for positive a. */
{
    const BasePtr minusA = Product::minus(Symbol::createPositive("a"));
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAtan2(minusA, zero));

    BOOST_CHECK_EQUAL(Numeric::mOne(), res);
}

BOOST_AUTO_TEST_CASE(sinOfAtan2NegDeltaYSumOnly)
/* Sin(atan2(-a - b, 0)) = -1 for positive a and b. */
{
    const BasePtr minusA = Product::minus(Symbol::createPositive("a"));
    const BasePtr minusB = Product::minus(Symbol::createPositive("b"));
    const BasePtr arg = Sum::create(minusA, minusB);
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAtan2(arg, zero));

    BOOST_CHECK_EQUAL(Numeric::mOne(), res);
}

BOOST_AUTO_TEST_CASE(numEvalPossibilityRequest)
{
    const BasePtr sin = Trigonometric::createSin(eight);

    BOOST_TEST(sin->isNumericallyEvaluable());
}

BOOST_AUTO_TEST_CASE(numericTerm)
{
    BOOST_CHECK_EQUAL(one, sinA->numericTerm());
}

BOOST_AUTO_TEST_CASE(nonNumericTerm)
{
    BOOST_CHECK_EQUAL(sinA, sinA->nonNumericTerm());
}

BOOST_AUTO_TEST_CASE(constTerm)
{
    const BasePtr sinTwo = Trigonometric::createSin(two);

    BOOST_CHECK_EQUAL(one, sinTwo->constTerm());
}

BOOST_AUTO_TEST_CASE(nonConstTerm)
{
    const BasePtr sinTwo = Trigonometric::createSin(two);

    BOOST_CHECK_EQUAL(sinTwo, sinTwo->nonConstTerm());
}

BOOST_AUTO_TEST_CASE(numericEvaluation)
/* Sin/cos/tan/asin/acos/atan(sqrt(2)/10) can be evaluated to a double. */
{
    const double dArg = 0.1 * std::sqrt(2.0);
    const BasePtr arg = Product::create(Numeric::create(1, 10), sqrtTwo);
    BasePtr fct;

    fct = Trigonometric::createSin(arg);
    BOOST_CHECK_EQUAL(std::sin(dArg), fct->numericEval());

    fct = Trigonometric::createCos(arg);
    BOOST_CHECK_EQUAL(std::cos(dArg), fct->numericEval());

    fct = Trigonometric::createTan(arg);
    BOOST_CHECK_EQUAL(std::tan(dArg), fct->numericEval());

    fct = Trigonometric::createAsin(arg);
    BOOST_CHECK_EQUAL(std::asin(dArg), fct->numericEval());

    fct = Trigonometric::createAcos(arg);
    BOOST_CHECK_EQUAL(std::acos(dArg), fct->numericEval());

    fct = Trigonometric::createAtan(arg);
    BOOST_CHECK_EQUAL(std::atan(dArg), fct->numericEval());
}

BOOST_AUTO_TEST_CASE(numericEvaluationAtan2)
{
    const BasePtr atan2 = Trigonometric::createAtan2(sqrtTwo, five);

    BOOST_CHECK_EQUAL(std::atan2(std::sqrt(2.0), 5), atan2->numericEval());
}

BOOST_AUTO_TEST_CASE(illegalNumericEvaluation)
{
    BOOST_CHECK_THROW(sinA->numericEval(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(sinOfLogarithm)
{
    const BasePtr log = Logarithm::create(a);
    const BasePtr result = Trigonometric::createSin(log);
    const Name expectedSinName("sin");

    BOOST_TEST(result->isFunction());
    BOOST_CHECK_EQUAL(expectedSinName, result->name());
    BOOST_CHECK_EQUAL(log, result->operands().front());
}

BOOST_AUTO_TEST_SUITE_END()
