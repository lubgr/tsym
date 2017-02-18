
#include <cmath>
#include "abc.h"
#include "power.h"
#include "sum.h"
#include "constant.h"
#include "product.h"
#include "logarithm.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Trigonometric)
{
    const BasePtr undefined;
    BasePtr minusHalf;
    BasePtr sqrtTwo;
    BasePtr sqrtThree;
    BasePtr aSquare;
    BasePtr pi;
    BasePtr sinA;

    void setup()
    {
        minusHalf = Numeric::create(-1, 2);
        sqrtTwo = Power::sqrt(two);
        sqrtThree = Power::sqrt(three);
        aSquare = Power::create(a, two);
        pi = Constant::createPi();
        sinA = Trigonometric::createSin(a);
    }
};

TEST(Trigonometric, typeRequest)
{
    CHECK(sinA->isFunction());
}

TEST(Trigonometric, constRequest)
{
    const BasePtr symbolArg = Trigonometric::createCos(a);
    const BasePtr numericArg = Trigonometric::createCos(two);

    CHECK(!symbolArg->isConst());
    CHECK(!numericArg->isConst());
}

TEST(Trigonometric, sinFunctionName)
{
    const Name expected("sin");

    CHECK_EQUAL(expected, sinA->name());
}

TEST(Trigonometric, typeString)
{
    const std::string expected("Function");

    CHECK_EQUAL(expected, sinA->typeStr());
}

TEST(Trigonometric, cosFunctionName)
{
    const BasePtr cos = Trigonometric::createCos(a);
    const Name expected("cos");

    CHECK_EQUAL(expected, cos->name());
}

TEST(Trigonometric, tanFunctionName)
{
    const BasePtr tan = Trigonometric::createTan(a);
    const Name expected("tan");

    CHECK_EQUAL(expected, tan->name());
}

TEST(Trigonometric, asinFunctionName)
{
    const BasePtr asin = Trigonometric::createAsin(a);
    const Name expected("asin");

    CHECK_EQUAL(expected, asin->name());
}

TEST(Trigonometric, acosFunctionName)
{
    const BasePtr acos = Trigonometric::createAcos(a);
    const Name expected("acos");

    CHECK_EQUAL(expected, acos->name());
}

TEST(Trigonometric, atanFunctionName)
{
    const BasePtr atan = Trigonometric::createAtan(a);
    const Name expected("atan");

    CHECK_EQUAL(expected, atan->name());
}

TEST(Trigonometric, atan2FunctionName)
{
    const BasePtr atan = Trigonometric::createAtan2(b, a);
    const Name expected("atan2");

    CHECK_EQUAL(expected, atan->name());
}

TEST(Trigonometric, trivialEquality)
{
    CHECK(sinA->isEqual(sinA));
}

TEST(Trigonometric, inequalityDifferentArg)
{
    const BasePtr sinB = Trigonometric::createSin(b);

    CHECK(sinA->isDifferent(sinB));
}

TEST(Trigonometric, inequalityDifferentTrigonometric)
{
    const BasePtr cosA = Trigonometric::createCos(a);

    CHECK(sinA->isDifferent(cosA));
}

TEST(Trigonometric, sinZero)
{
    const BasePtr res = Trigonometric::createSin(zero);

    CHECK_EQUAL(zero, res);
}

TEST(Trigonometric, cos345Degree)
    /* Cos(23/12*pi) = (sqrt(6) + sqrt(2))/4. */
{
    const BasePtr arg = Product::create(Numeric::create(23, 12), pi);
    BasePtr res;
    const BasePtr expected = Product::create(Numeric::create(1, 4),
            Sum::create(Power::sqrt(six), sqrtTwo));

    res = Trigonometric::createCos(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, tan240Degree)
    /* Tan(4/3*Pi) = sqrt(3). */
{
    const BasePtr arg = Product::create(Numeric::create(4, 3), pi);
    const BasePtr res = Trigonometric::createTan(arg);

    CHECK_EQUAL(sqrtThree, res);
}

TEST(Trigonometric, atan2Order)
{
    const BasePtr atan2Ab = Trigonometric::createAtan2(a, b);
    const BasePtr atan2Ba = Trigonometric::createAtan2(b, a);

    CHECK_EQUAL(a, atan2Ab->operands().front());
    CHECK_EQUAL(a, atan2Ba->operands().back());

    CHECK_EQUAL(b, atan2Ab->operands().back());
    CHECK_EQUAL(b, atan2Ba->operands().front());
}

TEST(Trigonometric, atan2XZeroNonEvaluable)
{
    const BasePtr res = Trigonometric::createAtan2(a, zero);

    CHECK(res->isFunction());
    CHECK_EQUAL(a, res->operands().front());
    CHECK_EQUAL(zero, res->operands().back());
}

TEST(Trigonometric, atan2FirstQuadrantResolvable)
{
    const BasePtr arg = Product::create(Numeric::create(25, 180), pi);
    const BasePtr result = Trigonometric::createAtan2(Trigonometric::createSin(arg),
            Trigonometric::createCos(arg));

    CHECK_EQUAL(arg, result);
}

TEST(Trigonometric, atan2SecondQuadrantResolvable)
{
    const BasePtr result = Trigonometric::createAtan2(one, Numeric::mOne());
    const BasePtr expected = Product::create(pi, Numeric::create(3, 4));

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, atan2ThirdQuadrantResolvable)
{
    const BasePtr y = Product::create(Numeric::create(-3), Sum::create(one, sqrtTwo));
    const BasePtr result = Trigonometric::createAtan2(y, Numeric::create(-3));
    const BasePtr expected = Product::create(Numeric::create(11, 8), pi);

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, atan2FourthQuadrantNonResolvable)
{
    const BasePtr expected = Product::minus(Trigonometric::createAtan(Numeric::create(1, 2)));
    const BasePtr result = Trigonometric::createAtan2(one, Numeric::create(-2));

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, atan2FourthQuadrantResolvable)
{
    const BasePtr sqrtFive = Power::sqrt(five);
    const BasePtr result = Trigonometric::createAtan2(Product::minus(sqrtFive), sqrtFive);
    const BasePtr expected = Product::create(pi, Numeric::create(7, 4));

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, atan2XZeroEvaluable)
{
    const BasePtr negativeX = Trigonometric::createAtan2(Product::minus(two, sqrtTwo), zero);
    const BasePtr positiveX = Trigonometric::createAtan2(sqrtTwo, zero);

    CHECK_EQUAL(Product::create(Numeric::create(1, 2), pi), positiveX);
    CHECK_EQUAL(Product::create(Numeric::create(3, 2), pi), negativeX);
}

TEST(Trigonometric, pureNumericAtan2WithRangeCorretion)
{
    const double numX = -1.2345678;
    const double numY = -12.987654;
    const BasePtr x = Numeric::create(numX);
    const BasePtr y = Numeric::create(numY);
    const BasePtr result = Trigonometric::createAtan2(y, x);
    const BasePtr expected = Numeric::create(std::atan2(numY, numX) + 2.0*M_PI);

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, pureNumericAtan2NoRangeCorretion)
{
    const double numX = 1.2345678;
    const double numY = 7.6543456;
    const BasePtr x = Numeric::create(numX);
    const BasePtr y = Numeric::create(numY);
    const BasePtr result = Trigonometric::createAtan2(y, x);
    const BasePtr expected = Numeric::create(std::atan2(numY, numX));

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, illegalAtan2)
{
    BasePtr res;

    disableLog();
    res = Trigonometric::createAtan2(zero, zero);
    enableLog();

    CHECK(res->isUndefined());
}

TEST(Trigonometric, undefinedArg)
{
    const BasePtr sin = Trigonometric::createSin(undefined);

    CHECK(sin->isUndefined());
}

TEST(Trigonometric, undefinedArgAtan)
{
    BasePtr res = Trigonometric::createAtan2(undefined, a);

    CHECK(res->isUndefined());

    res = Trigonometric::createAtan2(a, undefined);

    CHECK(res->isUndefined());
}

TEST(Trigonometric, sinOfAsin)
{
    const BasePtr asin = Trigonometric::createAsin(a);
    const BasePtr res = Trigonometric::createSin(asin);

    CHECK_EQUAL(a, res);
}

TEST(Trigonometric, sinOfNegativeSum)
{
    const BasePtr arg = Sum::create(Product::minus(a, b, c),
            Product::create(Numeric::create(-7,8), a),
            Product::minus(a, Trigonometric::createCos(b)));
    const BasePtr expected = Product::minus(Trigonometric::createSin(Product::minus(arg)));
    const BasePtr res = Trigonometric::createSin(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, sinOfNegativeProduct)
{
    const BasePtr arg = Product::create(Numeric::create(-1, 5),
            Power::create(two, Numeric::create(1, 3)), a);
    const BasePtr res = Trigonometric::createSin(arg);
    const BasePtr expected = Product::minus(Trigonometric::createSin(Product::minus(arg)));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, sinOfMinusAsin)
{
    const BasePtr mAsin = Product::minus(Trigonometric::createAsin(a));
    const BasePtr res = Trigonometric::createSin(mAsin);

    CHECK_EQUAL(Product::minus(a), res);
}

TEST(Trigonometric, cosOfAcos)
{
    const BasePtr acos = Trigonometric::createAcos(a);
    const BasePtr res = Trigonometric::createCos(acos);

    CHECK_EQUAL(a, res);
}

TEST(Trigonometric, cosOfNegativeNumericArg)
{
    const BasePtr res = Trigonometric::createCos(Numeric::create(-2));
    const BasePtr expected = Trigonometric::createCos(two);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, cosOfNegativeProduct)
{
    const BasePtr arg = Product::minus(a, b, Trigonometric::createAtan(Sum::create(a, b)));
    const BasePtr res = Trigonometric::createCos(arg);
    const BasePtr expected = Trigonometric::createCos(Product::minus(arg));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, cosOfMinusAcos)
{
    const BasePtr arg = Sum::create(a, Product::create(two, b));
    const BasePtr mAcos = Product::minus(Trigonometric::createAcos(arg));
    const BasePtr res = Trigonometric::createCos(mAcos);

    CHECK_EQUAL(arg, res);
}

TEST(Trigonometric, tanOfAtan)
{
    const BasePtr atan = Trigonometric::createAtan(a);
    const BasePtr res = Trigonometric::createTan(atan);

    CHECK_EQUAL(a, res);
}

TEST(Trigonometric, tanOfSimpleNegativeProduct)
{
    const BasePtr expected = Product::minus(Trigonometric::createTan(a));
    const BasePtr arg = Product::minus(a);
    const BasePtr res = Trigonometric::createTan(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atanOfNegSimpleProduct)
{
    const BasePtr expected = Product::minus(Trigonometric::createAtan(a));
    const BasePtr res = Trigonometric::createAtan(Product::minus(a));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atanOfNegSum)
{
    const BasePtr arg = Sum::create(Product::minus(two, a, b, c),
            Product::create(Numeric::create(-7, 11), sqrtTwo),
            Product::minus(Logarithm::create(Sum::create(Numeric::create(-2, 3), a))));
    const BasePtr expected = Product::minus(Trigonometric::createAtan(Product::minus(arg)));
    const BasePtr res = Trigonometric::createAtan(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, tanOfMinusAtan)
{
    const BasePtr mAtan = Product::minus(Trigonometric::createAtan(a));
    const BasePtr res = Trigonometric::createTan(mAtan);

    CHECK_EQUAL(Product::minus(a), res);
}

TEST(Trigonometric, tanOfMinusAsin)
    /* Tan(-asin(a)) = -a/sqrt(1 - a^2). */
{
    const BasePtr expected = Product::minus(a,
            Power::create(Sum::create(one, Product::minus(a, a)), minusHalf));
    const BasePtr mAsin = Product::minus(Trigonometric::createAsin(a));
    const BasePtr res = Trigonometric::createTan(mAsin);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atanOfTan)
{
    const BasePtr tan = Trigonometric::createTan(a);
    const BasePtr atan = Trigonometric::createAtan(tan);
    const Name expected("atan");

    CHECK(atan->isFunction());
    CHECK_EQUAL(expected, atan->name());
    CHECK_EQUAL(tan, atan->operands().front());
}

TEST(Trigonometric, sinOfCos)
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr res = Trigonometric::createCos(sin);
    BasePtr arg;

    CHECK(res->isFunction());

    arg = res->operands().front();

    CHECK(arg->isFunction());
    CHECK_EQUAL(a, arg->operands().front());
}

TEST(Trigonometric, asinOfSin)
    /* Asin(sin(a)) isn't simplified. */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr res = Trigonometric::createAsin(sin);
    BasePtr arg;

    CHECK(res->isFunction());

    arg = res->operands().front();

    CHECK(arg->isFunction());
    CHECK_EQUAL(a, arg->operands().front());
}

TEST(Trigonometric, asinOfMinusSin)
    /* Asin(-sin(a)) = -asin(sin(a)). */
{
    const BasePtr expected = Product::minus(Trigonometric::createAsin(Trigonometric::createSin(a)));
    const BasePtr minusSin = Product::minus(Trigonometric::createSin(a));
    const BasePtr res = Trigonometric::createAsin(minusSin);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, acosOfCos)
    /* Acos(cos(a)) isn't simplified. */
{
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(a));

    CHECK(res->isFunction());
    CHECK_EQUAL(1, res->operands().size());
    CHECK_EQUAL(Trigonometric::createCos(a), res->operands().front());
}

TEST(Trigonometric, acosOfMinusCos)
    /* Acos(-cos(a)) = pi - acos(cos(a)). */
{
    const BasePtr expected = Sum::create(pi,
            Product::minus(Trigonometric::createAcos(Trigonometric::createCos(a))));
    const BasePtr minusCos = Product::minus(Trigonometric::createCos(a));
    const BasePtr res = Trigonometric::createAcos(minusCos);
    BasePtr arg;

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, asinOfSinOfNumEvalNoShift)
    /* Asin(sin(-1/sqrt(5))) = -1/sqrt(5). */
{
    const BasePtr arg = Power::oneOver(Product::minus(Power::create(five, Numeric::create(1, 2))));
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));

    CHECK_EQUAL(arg, res);
}

TEST(Trigonometric, asinOfSinOfNumEvalNoShift2)
    /* Asin(sin(2/17)) = 2/17. */
{
    const BasePtr arg = Numeric::create(2, 17);
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr res = Trigonometric::createAsin(sin);

    CHECK_EQUAL(arg, res);
}

TEST(Trigonometric, asinOfSinOfNumEvalNegShift)
    /*  Asin(sin(13/4*pi)) = -pi/4. */
{
    const BasePtr arg = Product::create(Numeric::create(13, 4), pi);
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));
    const BasePtr expected = Product::create(Numeric::create(-1, 4), pi);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, asinOfSinOfNumEvalNegShift2)
    /* Asin(sin(12*sqrt(pi))) = 7*pi - 12*sqrt(pi). */
{
    const BasePtr arg = Product::create(Numeric::create(12), Power::sqrt(Constant::createPi()));
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr res = Trigonometric::createAsin(sin);
    const BasePtr expected = Sum::create(Product::create(seven, pi), Product::minus(arg));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, asinOfSinOfNumEvalPosShift)
    /* Asin(sin(-79/7*pi + 2/15)) = 2/7*pi - 2/15. */
{
    const BasePtr arg = Sum::create(Numeric::create(2, 15),
            Product::create(Numeric::create(-79, 7), pi));
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));
    const BasePtr expected = Sum::create(Product::create(Numeric::create(2, 7), pi),
            Numeric::create(-2, 15));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, asinOfSinOfNumEvalPosShift2)
    /* Asin(sin(-6/7*pi)) = -pi/7. */
{
    const BasePtr arg = Product::create(Numeric::create(-6, 7), pi);
    const BasePtr res = Trigonometric::createAsin(Trigonometric::createSin(arg));
    const BasePtr expected = Product::create(Numeric::create(-1, 7), pi);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, asinOfMinusAsinNumEval)
    /* Asin(-sin(1/2)) = -1/2. */
{
    const BasePtr mSin = Product::minus(Trigonometric::createSin(Numeric::create(1, 2)));
    const BasePtr res = Trigonometric::createAsin(mSin);

    CHECK_EQUAL(Numeric::create(-1, 2), res);
}

TEST(Trigonometric, atanOfTanNumEvalNoShift)
    /* Atan(tan(1/2)) = 1/2. */
{
    const BasePtr half = Numeric::create(1, 2);
    const BasePtr tan = Trigonometric::createTan(half);
    const BasePtr res = Trigonometric::createAtan(tan);

    CHECK_EQUAL(half, res);
}

TEST(Trigonometric, atanOfTanNumEvalWithNegShift)
    /* Atan(tan(2)) = 2 - pi, after substitution. */
{
    const BasePtr expected = Sum::create(two, Product::minus(pi));
    const BasePtr tan = Trigonometric::createTan(a);
    const BasePtr res = Trigonometric::createAtan(tan);
    BasePtr replaced;

    CHECK(res->isFunction());
    CHECK(res->operands().front()->isFunction());

    replaced = res->subst(a, two);

    CHECK_EQUAL(expected, replaced);
}

TEST(Trigonometric, atanOfTanNumEvalWithPosShift)
    /* Atan(tan(-23/21*pi - sqrt(3))) = 19/21*pi - sqrt(3). */
{
    const BasePtr sqrt = Product::minus(Power::sqrt(three));
    const BasePtr arg = Sum::create(Product::create(Numeric::create(-23, 21), pi), sqrt);
    const BasePtr res = Trigonometric::createAtan(Trigonometric::createTan(arg));
    const BasePtr expected = Sum::create(Product::create(Numeric::create(19, 21), pi), sqrt);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atanOfMinusTanNumEval)
    /* Atan(-tan(-1/2)) = 1/2. */
{
    const BasePtr mTan = Product::minus(Trigonometric::createTan(Numeric::create(-1, 2)));
    const BasePtr res = Trigonometric::createAtan(mTan);

    CHECK_EQUAL(Numeric::create(1, 2), res);
}

TEST(Trigonometric, acosOfCosNumEvalNoShift)
    /* Acos(cos(1/2)) = 1/2. */
{
    const BasePtr half = Numeric::create(1, 2);
    const BasePtr cos = Trigonometric::createCos(half);
    const BasePtr res = Trigonometric::createAcos(cos);

    CHECK_EQUAL(half, res);
}

TEST(Trigonometric, acosOfCosNumEvalPosShift)
    /* Acos(cos(-sqrt(21) - 3/8*pi)) = 13/8*pi - sqrt(21). */
{
    const BasePtr sqrt = Power::sqrt(Numeric::create(21));
    const BasePtr arg = Product::minus(Sum::create(sqrt,
                Product::create(Numeric::create(3, 8), pi)));
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr acos = Trigonometric::createAcos(cos);
    const BasePtr expected = Sum::create(Product::create(Numeric::create(13, 8), pi),
            Product::minus(sqrt));

    CHECK_EQUAL(expected, acos);
}

TEST(Trigonometric, acosOfCosNumEvalNegShift)
    /* Acos(cos(11/7*pi + 2^(1/3) + 0.123456)) = 2^(1/3) + 0.123456 - 3/7*pi. */
{
    const BasePtr pow = Power::create(two, Numeric::create(1, 3));
    const BasePtr sum = Sum::create(Numeric::create(0.123456), pow);
    const BasePtr arg = Sum::create(Product::create(Numeric::create(11, 7), pi), sum);
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(arg));
    const BasePtr expected = Sum::create(sum, Product::create(Numeric::create(-3, 7), pi));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, acosOfCosNumEvalNegShiftAndCorrection)
    /* Acos(cos(37/11*pi)) = 7/11*pi. */
{
    const BasePtr arg = Product::create(Numeric::create(37, 11), pi);
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(arg));
    const BasePtr expected = Product::create(Numeric::create(7, 11), pi);

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, acosOfCosNumEvalPosShiftAndCorrection)
    /* Acos(cos(-sqrt(21) - pi)) = sqrt(21) - pi. */
{
    const BasePtr sqrt = Power::sqrt(Numeric::create(21));
    const BasePtr arg = Product::minus(Sum::create(sqrt, Product::minus(pi)));
    const BasePtr res = Trigonometric::createAcos(Trigonometric::createCos(arg));
    const BasePtr expected = Sum::create(sqrt, Product::minus(pi));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, acosOfNegativeCosNumEvalWithShift)
    /* Acos(-cos(1/2)) = pi - 1/2. */
{
    const BasePtr arg = Product::minus(Trigonometric::createCos(Numeric::create(1, 2)));
    const BasePtr res = Trigonometric::createAcos(arg);

    CHECK_EQUAL(Sum::create(pi, Numeric::create(-1, 2)), res);
}

TEST(Trigonometric, sinOfAcos)
    /* Sin(acos(a)) = sqrt(1 - a^2). */
{
    const BasePtr expected = Power::sqrt(Sum::create(one, Product::minus(aSquare)));
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAcos(a));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, cosOfAsin)
    /* Cos(asin(a)) = sqrt(1 - a^2). */
{
    const BasePtr expected = Power::sqrt(Sum::create(one, Product::minus(aSquare)));
    const BasePtr res = Trigonometric::createCos(Trigonometric::createAsin(a));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, sinOfAtan)
    /* Sin(atan(a)) = a/sqrt(a^2 + 1). */
{
    const BasePtr expected = Product::create(a,
            Power::create(Sum::create(aSquare, one), minusHalf));
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAtan(a));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, cosOfAtan)
    /* Cos(atan(a)) = 1/sqrt(a^2 + 1). */
{
    const BasePtr expected = Power::create(Sum::create(Power::create(a, two), one), minusHalf);
    const BasePtr res = Trigonometric::createCos(Trigonometric::createAtan(a));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, tanOfAsin)
    /* Tan(asin(a)) = a/sqrt(1 - a^2). */
{
    const BasePtr expected = Product::create(a,
            Power::create(Sum::create(one, Product::minus(Power::create(a, two))), minusHalf));
    const BasePtr res = Trigonometric::createTan(Trigonometric::createAsin(a));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, tanOfAcos)
    /* Tan(asin(a)) = sqrt(1 - a^2)/a. */
{
    const BasePtr expected = Product::create(Power::sqrt(Sum::create(one, Product::minus(aSquare))),
            Power::oneOver(a));
    const BasePtr res = Trigonometric::createTan(Trigonometric::createAcos(a));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atan2OfSinCos)
    /* Atan2(sin(a), cos(a)) isn't simplified. */
{
    const BasePtr cosA = Trigonometric::createCos(a);
    const BasePtr res = Trigonometric::createAtan2(sinA, cosA);

    CHECK(res->isFunction());
    CHECK_EQUAL(sinA, res->operands().front());
    CHECK_EQUAL(cosA, res->operands().back());
}

TEST(Trigonometric, atan2OfNumericsNotResolvableArg)
    /* Atan2(2/3, 7/11) = atan(22/21). */
{
    const BasePtr res = Trigonometric::createAtan2(Numeric::create(2, 3), Numeric::create(7, 11));
    const BasePtr expected = Trigonometric::createAtan(Numeric::create(22, 21));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atan2OfNumEvaluableNotResolvable)
{
    const BasePtr y = Logarithm::create(Trigonometric::createTan(Numeric::create(1, 2)));
    const BasePtr x = Sum::create(Product::create(sqrtTwo, Constant::createE()), five,
            Trigonometric::createCos(two));
    const BasePtr res = Trigonometric::createAtan2(y, x);
    const BasePtr expected = Trigonometric::createAtan(Product::create(y, Power::oneOver(x)));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atan2OfNegNumEvaluableArg)
{
    const BasePtr res = Trigonometric::createAtan2(Numeric::create(-2, 3), seven);
    const BasePtr expected = Product::minus(Trigonometric::createAtan(Numeric::create(2, 21)));

    CHECK_EQUAL(expected, res);
}

TEST(Trigonometric, atan2OfNegSymbolicArg)
    /* No symmetry simplification should apply, as the argument(s) aren't clearly positive or
     * negative. */
{
    const BasePtr res = Trigonometric::createAtan2(Product::minus(a), b);

    CHECK(res->isFunction());
    CHECK_EQUAL(Product::minus(a), res->operands().front());
    CHECK_EQUAL(b, res->operands().back());
}

TEST(Trigonometric, tanOfAtan2)
    /* Tan(atan2(b, a)) = b/a. */
{
    const BasePtr atan2 = Trigonometric::createAtan2(b, a);
    const BasePtr result = Trigonometric::createTan(atan2);
    const BasePtr expected = Product::create(b, Power::oneOver(a));

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, cosOfAtan2)
    /* Cos(atan2(b, a)) = a/sqrt(a^2 + b^2). */
{
    const BasePtr atan2 = Trigonometric::createAtan2(b, a);
    const BasePtr result = Trigonometric::createCos(atan2);
    const BasePtr expected = Product::create(a, Power::create(Sum::create(aSquare,
                    Power::create(b, two)), Numeric::create(-1, 2)));

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, sinOfAtan2)
    /* Sin(atan2(b, a)) = b/sqrt(a^2 + b^2). */
{
    const BasePtr atan2 = Trigonometric::createAtan2(b, a);
    const BasePtr result = Trigonometric::createSin(atan2);
    const BasePtr expected = Product::create(b, Power::create(Sum::create(aSquare,
                    Power::create(b, two)), Numeric::create(-1, 2)));

    CHECK_EQUAL(expected, result);
}

TEST(Trigonometric, sinOfAtan2NegDeltaYOnly)
    /* Sin(atan2(-a, 0)) = -1 for positive a. */
{
    const BasePtr minusA = Product::minus(Symbol::createPositive("a"));
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAtan2(minusA, zero));

    CHECK_EQUAL(Numeric::mOne(), res);
}

TEST(Trigonometric, sinOfAtan2NegDeltaYSumOnly)
    /* Sin(atan2(-a - b, 0)) = -1 for positive a and b. */
{
    const BasePtr minusA = Product::minus(Symbol::createPositive("a"));
    const BasePtr minusB = Product::minus(Symbol::createPositive("b"));
    const BasePtr arg = Sum::create(minusA, minusB);
    const BasePtr res = Trigonometric::createSin(Trigonometric::createAtan2(arg, zero));

    CHECK_EQUAL(Numeric::mOne(), res);
}

TEST(Trigonometric, numEvalPossibilityRequest)
{
    const BasePtr sin = Trigonometric::createSin(eight);

    CHECK(sin->isNumericallyEvaluable());
}

TEST(Trigonometric, numericTerm)
{
    CHECK_EQUAL(one, sinA->numericTerm());
}

TEST(Trigonometric, nonNumericTerm)
{
    CHECK_EQUAL(sinA, sinA->nonNumericTerm());
}

TEST(Trigonometric, constTerm)
{
    const BasePtr sinTwo = Trigonometric::createSin(two);

    CHECK_EQUAL(one, sinTwo->constTerm());
}

TEST(Trigonometric, nonConstTerm)
{
    const BasePtr sinTwo = Trigonometric::createSin(two);

    CHECK_EQUAL(sinTwo, sinTwo->nonConstTerm());
}

TEST(Trigonometric, numericEvaluation)
    /* Sin/cos/tan/asin/acos/atan(sqrt(2)/10) can be evaluated to a double. */
{
    const double dArg = 0.1*std::sqrt(2.0);
    const BasePtr arg = Product::create(Numeric::create(1, 10), sqrtTwo);
    BasePtr fct;

    fct = Trigonometric::createSin(arg);
    CHECK_EQUAL(std::sin(dArg), fct->numericEval());

    fct = Trigonometric::createCos(arg);
    CHECK_EQUAL(std::cos(dArg), fct->numericEval());

    fct = Trigonometric::createTan(arg);
    CHECK_EQUAL(std::tan(dArg), fct->numericEval());

    fct = Trigonometric::createAsin(arg);
    CHECK_EQUAL(std::asin(dArg), fct->numericEval());

    fct = Trigonometric::createAcos(arg);
    CHECK_EQUAL(std::acos(dArg), fct->numericEval());

    fct = Trigonometric::createAtan(arg);
    CHECK_EQUAL(std::atan(dArg), fct->numericEval());
}

TEST(Trigonometric, numericEvaluationAtan2)
{
    const BasePtr atan2 = Trigonometric::createAtan2(sqrtTwo, five);

    CHECK_EQUAL(std::atan2(std::sqrt(2.0), 5), atan2->numericEval());
}

TEST(Trigonometric, illegalNumericEvaluation)
{
    CHECK(sinA->numericEval().isUndefined());
}

TEST(Trigonometric, sinOfLogarithm)
{
    const BasePtr log = Logarithm::create(a);
    const BasePtr result = Trigonometric::createSin(log);
    const Name expectedSinName("sin");

    CHECK(result->isFunction());
    CHECK_EQUAL(expectedSinName, result->name());
    CHECK_EQUAL(log, result->operands().front());
}
