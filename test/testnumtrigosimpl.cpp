
#include <cmath>
#include "numtrigosimpl.h"
#include "undefined.h"
#include "power.h"
#include "constant.h"
#include "numeric.h"
#include "sum.h"
#include "product.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

struct NumTrigoSimplFixture : public AbcFixture {
    const BasePtr& minusOne = Numeric::mOne();
    const BasePtr& half = Numeric::half();
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sqrtSix = Power::sqrt(six);
    NumTrigoSimpl nts {};

    void check(const BasePtr& expectedSin, const BasePtr& expectedCos, const BasePtr& expectedTan)
    {
        check(Trigonometric::Type::SIN, expectedSin);
        check(Trigonometric::Type::COS, expectedCos);
        check(Trigonometric::Type::TAN, expectedTan);
    }

    void check(Trigonometric::Type type, const BasePtr& expected)
    {
        nts.setType(type);
        nts.compute();

        BOOST_TEST(nts.hasSimplifiedResult());
        if (expected->isUndefined()) {
            BOOST_TEST(nts.get()->isUndefined());
        } else {
            BOOST_CHECK_EQUAL(expected, nts.get());
        }
    }

    void checkInverse(const BasePtr& expectedAsin, const BasePtr& expectedAcos,
            const BasePtr& expectedAtan)
    {
        check(Trigonometric::Type::ASIN, expectedAsin);
        check(Trigonometric::Type::ACOS, expectedAcos);
        check(Trigonometric::Type::ATAN, expectedAtan);
    }

    void checkUnsimplified()
    {
        checkUnsimplified(Trigonometric::Type::SIN);
        checkUnsimplified(Trigonometric::Type::COS);
        checkUnsimplified(Trigonometric::Type::TAN);
    }

    void checkUnsimplified(Trigonometric::Type type)
    {
        nts.setType(type);
        nts.compute();
        BOOST_TEST(!nts.hasSimplifiedResult());

        /* Requesting the result should give an Undefined. */
        BOOST_TEST(nts.get()->isUndefined());
    }
};

BOOST_FIXTURE_TEST_SUITE(TestNumTrigoSimpl, NumTrigoSimplFixture)

BOOST_AUTO_TEST_CASE(triviallyZero)
    /* Sin/cos/tan(0) = 0, 1, 0. */
{
    nts.setArg(zero);

    check(zero, one, zero);
}

BOOST_AUTO_TEST_CASE(eightPi)
    /* Sin/cos/tan(8*Pi) = 0, 1, 0. */
{
    nts.setArg(Product::create(eight, pi));

    check(zero, one, zero);
}

BOOST_AUTO_TEST_CASE(minusTwoPi)
    /* Sin/cos/tan(-2*Pi) = 0, 1, 0. */
{
    nts.setArg(Product::create(Numeric::create(-2), pi));

    check(zero, one, zero);
}

BOOST_AUTO_TEST_CASE(minusPi)
    /* Sin/cos/tan(-Pi) = 0, -1, 0. */
{
    nts.setArg(Product::minus(pi));

    check(zero, minusOne, zero);
}

BOOST_AUTO_TEST_CASE(piFourth)
    /* Sin/cos/tan(Pi/4) = 1/sqrt(2), 1/sqrt(2), 1. */
{
    const BasePtr sinCos = Power::create(two, Numeric::create(-1, 2));

    nts.setArg(Product::create(pi, Numeric::fourth()));

    check(sinCos, sinCos, one);
}

BOOST_AUTO_TEST_CASE(threeFourthPi)
    /* Sin/cos/tan(3*Pi/4) = -1/sqrt(2), 1/sqrt(2), -1. */
{
    const BasePtr expectedSin = Power::oneOver(sqrtTwo);
    const BasePtr expectedCos = Product::minus(expectedSin);

    nts.setArg(Product::create(Numeric::create(3, 4), pi));

    check(expectedSin, expectedCos, minusOne);
}

BOOST_AUTO_TEST_CASE(fiveFourthPi)
    /* Sin/cos/tan(5*Pi/4) = 1/sqrt(2), -1/sqrt(2), -1. */
{
    const BasePtr expectedSinCos = Product::minus(Power::oneOver(sqrtTwo));

    nts.setArg(Product::create(Numeric::create(5, 4), pi));

    check(expectedSinCos, expectedSinCos, one);
}

BOOST_AUTO_TEST_CASE(sevenFourthPi)
    /* Sin/cos/tan(7*Pi/4) = -1/sqrt(2), 1/sqrt(2), -1. */
{
    const BasePtr expectedCos = Power::oneOver(sqrtTwo);
    const BasePtr expectedSin = Product::minus(expectedCos);

    nts.setArg(Product::create(Numeric::create(7, 4), pi));

    check(expectedSin, expectedCos, minusOne);
}

BOOST_AUTO_TEST_CASE(threePiOverEight)
    /* Sin/cos/tan(3*Pi/8) = sqrt(2 + sqrt(2))/2, sqrt(2 - sqrt(2))/2, 1 + sqrt(2). */
{
    const BasePtr expectedSin = Product::create(half, Power::sqrt(Sum::create(two, sqrtTwo)));
    const BasePtr expectedCos = Product::create(half, Power::sqrt(
                Sum::create(two, Product::minus(sqrtTwo))));
    const BasePtr expectedTan = Sum::create(one, sqrtTwo);

    nts.setArg(Product::create(Numeric::create(3, 8), pi));

    check(expectedSin, expectedCos, expectedTan);
}

BOOST_AUTO_TEST_CASE(cosLeadsToNewAdjustment)
    /* Cos(5/3*Pi) is computed via Sin(5/3*Pi + Pi/2), where the argument is again greater than 2*Pi
     * and needs to be shifted back. */
{
    const BasePtr expectedSin = Product::create(Numeric::create(-1, 2), sqrtThree);
    const BasePtr expectedTan = Product::minus(sqrtThree);

    nts.setArg(Product::create(Numeric::create(5, 3), pi));

    check(expectedSin, half, expectedTan);
}

BOOST_AUTO_TEST_CASE(unresolvableNumeric, noLogs())
    /* Sin/cos/tan(1/4) shouldn't be simplified. */
{
    nts.setArg(Numeric::fourth());

    checkUnsimplified();
}

BOOST_AUTO_TEST_CASE(unresolvableNumericPower, noLogs())
    /* Sin/cos/tan(sqrt(2)) shouldn't be simplified. */
{
    nts.setArg(sqrtTwo);

    checkUnsimplified();
}

BOOST_AUTO_TEST_CASE(exactFromDouble)
    /* Sin/cos/tan(Pi/12.0) = (sqrt(6) - sqrt(2))/4, (sqrt(6) + sqrt(2))/4, 2 - sqrt(3). */
{
    const BasePtr expectedSin = Product::create(Numeric::fourth(), Sum::create(sqrtSix,
                Product::minus(sqrtTwo)));
    const BasePtr expectedCos = Product::create(Numeric::fourth(), Sum::create(sqrtSix,
                sqrtTwo));
    const BasePtr expectedTan = Sum::create(two, Product::minus(sqrtThree));

    nts.setArg(Number(M_PI/12.0));

    check(expectedSin, expectedCos, expectedTan);
}

BOOST_AUTO_TEST_CASE(exactFromNumericallyEvaluable)
    /* A product of numerically evaluable factors, that matches an entry of the exact tables of the
     * class will lead to an exact result. */
{
    const BasePtr expectedSin = Power::oneOver(sqrtTwo);
    const BasePtr arg = Product::create({ Numeric::create(0.176351684975302), sqrtThree,
            Power::create(Numeric::create(17), Numeric::third()) });

    nts.setArg(arg);

    check(Trigonometric::Type::SIN, expectedSin);
}

BOOST_AUTO_TEST_CASE(numericEvaluation)
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

BOOST_AUTO_TEST_CASE(numEvalSecondQuadrant)
    /* Sin/cos/tan(2.3456789) should be evaluated to a plain Numeric. */
{
    const double arg = 3.456789;
    const double expectedSin = std::sin(arg);
    const double expectedCos = std::cos(arg);
    const double expectedTan = std::tan(arg);

    nts.setArg(arg);

    check(Numeric::create(expectedSin), Numeric::create(expectedCos), Numeric::create(expectedTan));
}

BOOST_AUTO_TEST_CASE(undefinedTan)
    /* Tan(Pi/2) = Undefined. */
{
    nts.setArg(Product::create(half, pi));

    check(one, zero, Undefined::create());
}

BOOST_AUTO_TEST_CASE(inverseZero)
    /* Asin/acos/atan(0) = 0, Pi/2, 0. */
{
    nts.setArg(zero);

    checkInverse(zero, Product::create(half, pi), zero);
}

BOOST_AUTO_TEST_CASE(inverseOneOverSqrtTwo, noLogs())
    /* Asin/acos/atan(1/sqrt(2)) = Pi/4, Pi/4, unsimplified. */
{
    const BasePtr piFourth = Product::create(pi, Numeric::fourth());

    nts.setArg(Power::oneOver(sqrtTwo));

    check(Trigonometric::Type::ASIN, piFourth);
    check(Trigonometric::Type::ACOS, piFourth);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(inverseNegativeArg, noLogs())
    /* Asin/acos/atan(-1/sqrt(2)) = -Pi/4, 3*Pi/4, unsimplified. */
{
    const BasePtr arg = Product::minus(Power::oneOver(sqrtTwo));
    const BasePtr expectedAsin = Product::create(Numeric::create(-1, 4), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(3, 4), pi);

    nts.setArg(arg);

    check(Trigonometric::Type::ASIN, expectedAsin);
    check(Trigonometric::Type::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(inverseFromSum, noLogs())
    /* Asin/acos/atan((sqrt(6) - sqrt(2))/4  = Pi/12, 5*Pi/12, unsimplified. */
{
    const BasePtr arg = Product::create(Numeric::fourth(), Sum::create(sqrtSix,
                Product::minus(sqrtTwo)));
    const BasePtr expectedAsin = Product::create(Numeric::create(1, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(5, 12), pi);

    nts.setArg(arg);

    check(Trigonometric::Type::ASIN, expectedAsin);
    check(Trigonometric::Type::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(inverseNegativeSum, noLogs())
    /* Asin/acos/atan(-(sqrt(6) + sqrt(2))/4) = -5/12*pi, 11/12*pi, unsimplified. */
{
    const BasePtr arg = Product::create(Numeric::create(-1, 4), Sum::create(sqrtSix, sqrtTwo));
    const BasePtr expectedAsin = Product::create(Numeric::create(-5, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(11, 12), pi);

    nts.setArg(arg);

    check(Trigonometric::Type::ASIN, expectedAsin);
    check(Trigonometric::Type::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(atanNegativeArg)
    /* Atan(-sqrt(3)) = -Pi/3. */
{
    const BasePtr expected = Product::create(Numeric::create(-1, 3), pi);

    nts.setArg(Product::minus(sqrtThree));

    check(Trigonometric::Type::ATAN, expected);
}

BOOST_AUTO_TEST_CASE(atanNegativeSum)
    /* Atan(-sqrt(2) - 1) = -3/8*pi. */
{
    const BasePtr expected = Product::create(Numeric::create(-3, 8), pi);
    const BasePtr arg = Product::minus(Sum::create(sqrtTwo, one));

    nts.setArg(arg);

    check(Trigonometric::Type::ATAN, expected);
}

BOOST_AUTO_TEST_CASE(inverseExactFromDouble)
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

BOOST_AUTO_TEST_CASE(inverseFromSumAtan)
    /* Asin/acos/atan(sqrt(2) + 1) = undefined, undefined, 3/8*Pi. */
{
    const BasePtr expectedAtan = Product::create(Numeric::create(3, 8), pi);
    const BasePtr undefined = Undefined::create();
    const BasePtr arg = Sum::create(sqrtTwo, one);

    nts.setArg(arg);

    checkInverse(undefined, undefined, expectedAtan);
}

BOOST_AUTO_TEST_CASE(inverseExactFromDoubleAtan)
    /* Asin/acos/atan(2 - sqrt(3)) = 0.27126(...), 1.2995(...), 1/12*Pi. */
{
    const BasePtr expectedAtan = Product::create(Numeric::create(1, 12), pi);
    const double arg = 2.0 - std::sqrt(3.0);
    const BasePtr expectedAsin = Numeric::create(std::asin(arg));
    const BasePtr expectedAcos = Numeric::create(std::acos(arg));

    nts.setArg(arg);

    checkInverse(expectedAsin, expectedAcos, expectedAtan);
}

BOOST_AUTO_TEST_CASE(noSimplification, noLogs())
    /* Sin/cos/tan aren't simplified for a numerically evaluable argument, that isn't in the
     * exact tables. */
{
    const BasePtr arg = Product::create(sqrtTwo, pi);

    nts.setArg(arg);

    checkUnsimplified(Trigonometric::Type::SIN);
    checkUnsimplified(Trigonometric::Type::COS);
    checkUnsimplified(Trigonometric::Type::TAN);
}

BOOST_AUTO_TEST_CASE(noSimplificationInverse, noLogs())
    /* The same for inverse functions. */
{
    const BasePtr arg = Product::create(sqrtTwo, Numeric::create(1, 10), pi);

    nts.setArg(arg);

    checkUnsimplified(Trigonometric::Type::ASIN);
    checkUnsimplified(Trigonometric::Type::ACOS);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(noSimplificationLargeInput, noLogs())
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

    checkUnsimplified(Trigonometric::Type::SIN);
    checkUnsimplified(Trigonometric::Type::COS);
    checkUnsimplified(Trigonometric::Type::TAN);
    checkUnsimplified(Trigonometric::Type::ASIN);
    checkUnsimplified(Trigonometric::Type::ACOS);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(greaterThanRange, noLogs())
    /* Asin/acos aren't defined for arguments < -1. */
{
    const BasePtr arg = Product::create(Numeric::create(-17), sqrtTwo);

    nts.setArg(arg);

    check(Trigonometric::Type::ASIN, Undefined::create());
    check(Trigonometric::Type::ACOS, Undefined::create());
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(lessThanRange)
    /* The same for arguments > 1. */
{
    const double arg = 1.0000001;

    nts.setArg(arg);

    checkInverse(Undefined::create(), Undefined::create(), Numeric::create(std::atan(arg)));
}

BOOST_AUTO_TEST_CASE(largeNumericEvaluationToExact)
    /* A product of numerically evaluable factors, that matches an entry of the exact tables of the
     * class will lead to an exact result. */
{
    const BasePtr piFourth = Product::create(pi, Numeric::fourth());
    const BasePtr arg = Product::create({ Numeric::create(0.6258398439057556), Power::sqrt(five),
            Power::oneOver(pi), Power::create(four, Numeric::third()) });

    nts.setArg(arg);

    check(Trigonometric::Type::ASIN, piFourth);
}

BOOST_AUTO_TEST_CASE(inverseUnresolvableNumeric, noLogs())
    /* Asin/acos/atan(-1/10) shouldn't be simplified. */
{
    nts.setArg(Numeric::create(-1, 10));

    checkUnsimplified(Trigonometric::Type::ASIN);
    checkUnsimplified(Trigonometric::Type::ACOS);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_SUITE_END()
