
#include <cmath>
#include "cpputest.h"
#include "abc.h"
#include "numtrigosimpl.h"
#include "power.h"
#include "constant.h"
#include "sum.h"
#include "product.h"

using namespace tsym;

TEST_GROUP(NumTrigoSimpl)
{
    NumTrigoSimpl nts;
    BasePtr minusOne;
    BasePtr half;
    BasePtr sqrtTwo;
    BasePtr sqrtThree;
    BasePtr sqrtSix;
    BasePtr pi;

    void setup()
    {
        minusOne = Numeric::mOne();
        half = Numeric::create(1, 2);
        sqrtTwo = Power::sqrt(two);
        sqrtThree = Power::sqrt(three);
        sqrtSix = Power::sqrt(six);
        pi = Constant::createPi();
    }

    void check(const BasePtr& expectedSin, const BasePtr& expectedCos, const BasePtr& expectedTan)
    {
        check(Trigonometric::SIN, expectedSin);
        check(Trigonometric::COS, expectedCos);
        check(Trigonometric::TAN, expectedTan);
    }

    void check(Trigonometric::Type type, const BasePtr& expected)
    {
        nts.setType(type);
        nts.compute();

        CHECK(nts.hasSimplifiedResult());
        if (expected->isUndefined()) {
            CHECK(nts.get()->isUndefined());
        } else {
            CHECK_EQUAL(expected, nts.get());
        }
    }

    void checkInverse(const BasePtr& expectedAsin, const BasePtr& expectedAcos,
            const BasePtr& expectedAtan)
    {
        check(Trigonometric::ASIN, expectedAsin);
        check(Trigonometric::ACOS, expectedAcos);
        check(Trigonometric::ATAN, expectedAtan);
    }

    void checkUnsimplified()
    {
        checkUnsimplified(Trigonometric::SIN);
        checkUnsimplified(Trigonometric::COS);
        checkUnsimplified(Trigonometric::TAN);
    }

    void checkUnsimplified(Trigonometric::Type type)
    {
        nts.setType(type);
        nts.compute();
        CHECK(!nts.hasSimplifiedResult());

        disableLog();
        /* Requesting the result should give an Undefined. */
        CHECK(nts.get()->isUndefined());
        enableLog();
    }
};

TEST(NumTrigoSimpl, zero)
    /* Sin/cos/tan(0) = 0, 1, 0. */
{
    nts.setArg(zero);

    check(zero, one, zero);
}

TEST(NumTrigoSimpl, eightPi)
    /* Sin/cos/tan(8*Pi) = 0, 1, 0. */
{
    nts.setArg(Product::create(eight, pi));

    check(zero, one, zero);
}

TEST(NumTrigoSimpl, minusTwoPi)
    /* Sin/cos/tan(-2*Pi) = 0, 1, 0. */
{
    nts.setArg(Product::create(Numeric::create(-2), pi));

    check(zero, one, zero);
}

TEST(NumTrigoSimpl, minusPi)
    /* Sin/cos/tan(-Pi) = 0, -1, 0. */
{
    nts.setArg(Product::minus(pi));

    check(zero, minusOne, zero);
}

TEST(NumTrigoSimpl, piFourth)
    /* Sin/cos/tan(Pi/4) = 1/sqrt(2), 1/sqrt(2), 1. */
{
    const BasePtr sinCos = Power::create(two, Numeric::create(-1, 2));

    nts.setArg(Product::create(pi, Numeric::create(1, 4)));

    check(sinCos, sinCos, one);
}

TEST(NumTrigoSimpl, threeFourthPi)
    /* Sin/cos/tan(3*Pi/4) = -1/sqrt(2), 1/sqrt(2), -1. */
{
    const BasePtr expectedSin = Power::oneOver(sqrtTwo);
    const BasePtr expectedCos = Product::minus(expectedSin);

    nts.setArg(Product::create(Numeric::create(3, 4), pi));

    check(expectedSin, expectedCos, minusOne);
}

TEST(NumTrigoSimpl, fiveFourthPi)
    /* Sin/cos/tan(5*Pi/4) = 1/sqrt(2), -1/sqrt(2), -1. */
{
    const BasePtr expectedSinCos = Product::minus(Power::oneOver(sqrtTwo));

    nts.setArg(Product::create(Numeric::create(5, 4), pi));

    check(expectedSinCos, expectedSinCos, one);
}

TEST(NumTrigoSimpl, sevenFourthPi)
    /* Sin/cos/tan(7*Pi/4) = -1/sqrt(2), 1/sqrt(2), -1. */
{
    const BasePtr expectedCos = Power::oneOver(sqrtTwo);
    const BasePtr expectedSin = Product::minus(expectedCos);

    nts.setArg(Product::create(Numeric::create(7, 4), pi));

    check(expectedSin, expectedCos, minusOne);
}

TEST(NumTrigoSimpl, threePiOverEight)
    /* Sin/cos/tan(3*Pi/8) = sqrt(2 + sqrt(2))/2, sqrt(2 - sqrt(2))/2, 1 + sqrt(2). */
{
    const BasePtr expectedSin = Product::create(half, Power::sqrt(Sum::create(two, sqrtTwo)));
    const BasePtr expectedCos = Product::create(half, Power::sqrt(
                Sum::create(two, Product::minus(sqrtTwo))));
    const BasePtr expectedTan = Sum::create(one, sqrtTwo);

    nts.setArg(Product::create(Numeric::create(3, 8), pi));

    check(expectedSin, expectedCos, expectedTan);
}

TEST(NumTrigoSimpl, cosLeadsToNewAdjustment)
    /* Cos(5/3*Pi) is computed via Sin(5/3*Pi + Pi/2), where the argument is again greater than 2*Pi
     * and needs to be shifted back. */
{
    const BasePtr expectedSin = Product::create(Numeric::create(-1, 2), sqrtThree);
    const BasePtr expectedTan = Product::minus(sqrtThree);

    nts.setArg(Product::create(Numeric::create(5, 3), pi));

    check(expectedSin, half, expectedTan);
}

TEST(NumTrigoSimpl, unresolvableNumeric)
    /* Sin/cos/tan(1/4) shouldn't be simplified. */
{
    nts.setArg(Numeric::create(1, 4));

    checkUnsimplified();
}

TEST(NumTrigoSimpl, unresolvableNumericPower)
    /* Sin/cos/tan(sqrt(2)) shouldn't be simplified. */
{
    nts.setArg(sqrtTwo);

    checkUnsimplified();
}

TEST(NumTrigoSimpl, exactFromDouble)
    /* Sin/cos/tan(Pi/12.0) = (sqrt(6) - sqrt(2))/4, (sqrt(6) + sqrt(2))/4, 2 - sqrt(3). */
{
    const BasePtr expectedSin = Product::create(Numeric::create(1, 4), Sum::create(sqrtSix,
                Product::minus(sqrtTwo)));
    const BasePtr expectedCos = Product::create(Numeric::create(1, 4), Sum::create(sqrtSix,
                sqrtTwo));
    const BasePtr expectedTan = Sum::create(two, Product::minus(sqrtThree));

    nts.setArg(Number(M_PI/12.0));

    check(expectedSin, expectedCos, expectedTan);
}

TEST(NumTrigoSimpl, exactFromNumericallyEvaluable)
    /* A product of numerically evaluable factors, that matches an entry of the exact tables of the
     * class will lead to an exact result. */
{
    const BasePtr expectedSin = Power::oneOver(sqrtTwo);
    BasePtrList fac;
    BasePtr arg;

    fac.push_back(Numeric::create(0.176351684975302));
    fac.push_back(sqrtThree);
    fac.push_back(Power::create(Numeric::create(17), Numeric::create(1, 3)));

    arg = Product::create(fac);

    nts.setArg(arg);

    check(Trigonometric::SIN, expectedSin);
}

TEST(NumTrigoSimpl, numericEvaluation)
    /* Sin/cos/tan(1.23456789) should be evaluated to a plain Numeric. */
{
    const double arg = 1.23456789;
    const double expectedSin = std::sin(arg);
    const double expectedCos = std::cos(arg);
    const double expectedTan = std::tan(arg);

    nts.setArg(Number(arg));

    /* The comparison will be evaluated with the internal double comparison tolerance of the Number
     * class. */
    check(Numeric::create(expectedSin), Numeric::create(expectedCos), Numeric::create(expectedTan));
}

TEST(NumTrigoSimpl, numEvalSecondQuadrant)
    /* Sin/cos/tan(2.3456789) should be evaluated to a plain Numeric. */
{
    const double arg = 3.456789;
    const double expectedSin = std::sin(arg);
    const double expectedCos = std::cos(arg);
    const double expectedTan = std::tan(arg);

    nts.setArg(arg);

    check(Numeric::create(expectedSin), Numeric::create(expectedCos), Numeric::create(expectedTan));
}

TEST(NumTrigoSimpl, undefinedTan)
    /* Tan(Pi/2) = Undefined. */
{
    nts.setArg(Product::create(half, pi));

    check(one, zero, Undefined::create());
}

TEST(NumTrigoSimpl, inverseZero)
    /* Asin/acos/atan(0) = 0, Pi/2, 0. */
{
    nts.setArg(zero);

    checkInverse(zero, Product::create(half, pi), zero);
}

TEST(NumTrigoSimpl, inverseOneOverSqrtTwo)
    /* Asin/acos/atan(1/sqrt(2)) = Pi/4, Pi/4, unsimplified. */
{
    const BasePtr piFourth = Product::create(pi, Numeric::create(1, 4));

    nts.setArg(Power::oneOver(sqrtTwo));

    check(Trigonometric::ASIN, piFourth);
    check(Trigonometric::ACOS, piFourth);
    checkUnsimplified(Trigonometric::ATAN);
}

TEST(NumTrigoSimpl, inverseNegativeArg)
    /* Asin/acos/atan(-1/sqrt(2)) = -Pi/4, 3*Pi/4, unsimplified. */
{
    const BasePtr arg = Product::minus(Power::oneOver(sqrtTwo));
    const BasePtr expectedAsin = Product::create(Numeric::create(-1, 4), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(3, 4), pi);

    nts.setArg(arg);

    check(Trigonometric::ASIN, expectedAsin);
    check(Trigonometric::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::ATAN);
}

TEST(NumTrigoSimpl, inverseFromSum)
    /* Asin/acos/atan((sqrt(6) - sqrt(2))/4  = Pi/12, 5*Pi/12, unsimplified. */
{
    const BasePtr arg = Product::create(Numeric::create(1, 4), Sum::create(sqrtSix,
                Product::minus(sqrtTwo)));
    const BasePtr expectedAsin = Product::create(Numeric::create(1, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(5, 12), pi);

    nts.setArg(arg);

    check(Trigonometric::ASIN, expectedAsin);
    check(Trigonometric::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::ATAN);
}

TEST(NumTrigoSimpl, inverseNegativeSum)
    /* Asin/acos/atan(-(sqrt(6) + sqrt(2))/4) = -5/12*pi, 11/12*pi, unsimplified. */
{
    const BasePtr arg = Product::create(Numeric::create(-1, 4), Sum::create(sqrtSix, sqrtTwo));
    const BasePtr expectedAsin = Product::create(Numeric::create(-5, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(11, 12), pi);

    nts.setArg(arg);

    check(Trigonometric::ASIN, expectedAsin);
    check(Trigonometric::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::ATAN);
}

TEST(NumTrigoSimpl, atanNegativeArg)
    /* Atan(-sqrt(3)) = -Pi/3. */
{
    const BasePtr expected = Product::create(Numeric::create(-1, 3), pi);

    nts.setArg(Product::minus(sqrtThree));

    check(Trigonometric::ATAN, expected);
}

TEST(NumTrigoSimpl, atanNegativeSum)
    /* Atan(-sqrt(2) - 1) = -3/8*pi. */
{
    const BasePtr expected = Product::create(Numeric::create(-3, 8), pi);
    const BasePtr arg = Product::minus(Sum::create(sqrtTwo, one));

    nts.setArg(arg);

    check(Trigonometric::ATAN, expected);
}

TEST(NumTrigoSimpl, inverseExactFromDouble)
    /* The same as before, but the argument is a double. This implies, that the inverse tangent
     * evaluates to a double, and is thus simplified. */
{
    const double arg = 0.25*(std::sqrt(6.0) - std::sqrt(2.0));
    const BasePtr expectedAsin = Product::create(Numeric::create(1, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(5, 12), pi);
    const BasePtr expectedAtan = Numeric::create(std::atan(arg));

    nts.setArg(arg);

    checkInverse(expectedAsin, expectedAcos, expectedAtan);
}

TEST(NumTrigoSimpl, inverseFromSumAtan)
    /* Asin/acos/atan(sqrt(2) + 1) = undefined, undefined, 3/8*Pi. */
{
    const BasePtr expectedAtan = Product::create(Numeric::create(3, 8), pi);
    const BasePtr undefined = Undefined::create();
    const BasePtr arg = Sum::create(sqrtTwo, one);

    nts.setArg(arg);

    checkInverse(undefined, undefined, expectedAtan);
}

TEST(NumTrigoSimpl, inverseExactFromDoubleAtan)
    /* Asin/acos/atan(2 - sqrt(3)) = 0.27126(...), 1.2995(...), 1/12*Pi. */
{
    const BasePtr expectedAtan = Product::create(Numeric::create(1, 12), pi);
    const double arg = 2.0 - std::sqrt(3.0);
    const BasePtr expectedAsin = Numeric::create(std::asin(arg));
    const BasePtr expectedAcos = Numeric::create(std::acos(arg));

    nts.setArg(arg);

    checkInverse(expectedAsin, expectedAcos, expectedAtan);
}

TEST(NumTrigoSimpl, noSimplification)
    /* Sin/cos/tan aren't simplified for a numerically evaluable argument, that isn't in the
     * exact tables. */
{
    const BasePtr arg = Product::create(sqrtTwo, pi);

    nts.setArg(arg);

    checkUnsimplified(Trigonometric::SIN);
    checkUnsimplified(Trigonometric::COS);
    checkUnsimplified(Trigonometric::TAN);
}

TEST(NumTrigoSimpl, noSimplificationInverse)
    /* The same for inverse functions. */
{
    const BasePtr arg = Product::create(sqrtTwo, Numeric::create(1, 10), pi);

    nts.setArg(arg);

    checkUnsimplified(Trigonometric::ASIN);
    checkUnsimplified(Trigonometric::ACOS);
    checkUnsimplified(Trigonometric::ATAN);
}

TEST(NumTrigoSimpl, noSimplificationLargeInput)
{
    BasePtrList fac;
    BasePtr arg;

    fac.push_back(Numeric::create(1, 20));
    fac.push_back(pi);
    fac.push_back(sqrtThree);
    fac.push_back(Power::create(four, Numeric::create(-1, 5)));
    fac.push_back(Power::create(six, Numeric::create(-1, 10)));

    arg = Product::create(fac);

    nts.setArg(arg);

    checkUnsimplified(Trigonometric::SIN);
    checkUnsimplified(Trigonometric::COS);
    checkUnsimplified(Trigonometric::TAN);
    checkUnsimplified(Trigonometric::ASIN);
    checkUnsimplified(Trigonometric::ACOS);
    checkUnsimplified(Trigonometric::ATAN);
}

TEST(NumTrigoSimpl, greaterThanRange)
    /* Asin/acos aren't defined for arguments < -1. */
{
    const BasePtr arg = Product::create(Numeric::create(-17), sqrtTwo);

    nts.setArg(arg);

    check(Trigonometric::ASIN, Undefined::create());
    check(Trigonometric::ACOS, Undefined::create());
    checkUnsimplified(Trigonometric::ATAN);
}

TEST(NumTrigoSimpl, lessThanRange)
    /* The same for arguments > 1. */
{
    const double arg = 1.0000001;

    nts.setArg(arg);

    checkInverse(Undefined::create(), Undefined::create(), Numeric::create(std::atan(arg)));
}

TEST(NumTrigoSimpl, largeNumericEvaluationToExact)
    /* A product of numerically evaluable factors, that matches an entry of the exact tables of the
     * class will lead to an exact result. */
{
    const BasePtr piFourth = Product::create(pi, Numeric::create(1, 4));
    BasePtrList fac;
    BasePtr arg;

    fac.push_back(Numeric::create(0.6258398439057556));
    fac.push_back(Power::sqrt(five));
    fac.push_back(Power::oneOver(pi));
    fac.push_back(Power::create(four, Numeric::create(1, 3)));

    arg = Product::create(fac);

    nts.setArg(arg);

    check(Trigonometric::ASIN, piFourth);
}

TEST(NumTrigoSimpl, inverseUnresolvableNumeric)
    /* Asin/acos/atan(-1/10) shouldn't be simplified. */
{
    nts.setArg(Numeric::create(-1, 10));

    checkUnsimplified(Trigonometric::ASIN);
    checkUnsimplified(Trigonometric::ACOS);
    checkUnsimplified(Trigonometric::ATAN);
}
