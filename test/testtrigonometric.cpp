
#include <cmath>
#include "cpputest.h"
#include "abc.h"
#include "power.h"
#include "sum.h"
#include "constant.h"
#include "product.h"
#include "trigonometric.h"

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

TEST(Trigonometric, undefinedArg)
{
    const BasePtr sin = Trigonometric::createSin(undefined);

    CHECK(sin->isUndefined());
}

TEST(Trigonometric, sinOfAsin)
{
    const BasePtr asin = Trigonometric::createAsin(a);
    const BasePtr res = Trigonometric::createSin(asin);

    CHECK_EQUAL(a, res);
}

TEST(Trigonometric, cosOfAcos)
{
    const BasePtr acos = Trigonometric::createAcos(a);
    const BasePtr res = Trigonometric::createCos(acos);

    CHECK_EQUAL(a, res);
}

TEST(Trigonometric, tanOfAan)
{
    const BasePtr atan = Trigonometric::createAtan(a);
    const BasePtr res = Trigonometric::createTan(atan);

    CHECK_EQUAL(a, res);
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

TEST(Trigonometric, illegalNumericEvaluation)
{
    CHECK(sinA->numericEval().isUndefined());
}
