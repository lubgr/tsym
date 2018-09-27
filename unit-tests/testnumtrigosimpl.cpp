
#include <cmath>
#include "basefct.h"
#include "constant.h"
#include "fixtures.h"
#include "numeric.h"
#include "numtrigosimpl.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct NumTrigoSimplFixture : public AbcFixture {
    const BasePtr& minusOne = Numeric::mOne();
    const BasePtr& half = Numeric::half();
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sqrtSix = Power::sqrt(six);
    BasePtr arg{};

    void check(const BasePtr& expectedSin, const BasePtr& expectedCos, const BasePtr& expectedTan)
    {
        check(Trigonometric::Type::SIN, expectedSin);
        check(Trigonometric::Type::COS, expectedCos);
        check(Trigonometric::Type::TAN, expectedTan);
    }

    void check(Trigonometric::Type type, const BasePtr& expected)
    {
        NumTrigoSimpl nts(arg, type);
        const auto result = nts.simplify();

        BOOST_TEST(result);

        if (isUndefined(*expected)) {
            BOOST_TEST(isUndefined(*result.value()));
        } else {
            BOOST_CHECK_EQUAL(expected, result.value());
        }
    }

    void checkInverse(const BasePtr& expectedAsin, const BasePtr& expectedAcos, const BasePtr& expectedAtan)
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
        NumTrigoSimpl nts(arg, type);

        BOOST_TEST(!nts.simplify());
    }
};

BOOST_FIXTURE_TEST_SUITE(TestNumTrigoSimpl, NumTrigoSimplFixture)

BOOST_AUTO_TEST_CASE(triviallyZero)
/* Sin/cos/tan(0) = 0, 1, 0. */
{
    arg = zero;

    check(zero, one, zero);
}

BOOST_AUTO_TEST_CASE(eightPi)
/* Sin/cos/tan(8*Pi) = 0, 1, 0. */
{
    arg = Product::create(eight, pi);

    check(zero, one, zero);
}

BOOST_AUTO_TEST_CASE(minusTwoPi)
/* Sin/cos/tan(-2*Pi) = 0, 1, 0. */
{
    arg = Product::create(Numeric::create(-2), pi);

    check(zero, one, zero);
}

BOOST_AUTO_TEST_CASE(minusPi)
/* Sin/cos/tan(-Pi) = 0, -1, 0. */
{
    arg = Product::minus(pi);

    check(zero, minusOne, zero);
}

BOOST_AUTO_TEST_CASE(piFourth)
/* Sin/cos/tan(Pi/4) = 1/sqrt(2), 1/sqrt(2), 1. */
{
    const BasePtr sinCos = Power::create(two, Numeric::create(-1, 2));

    arg = Product::create(pi, Numeric::fourth());

    check(sinCos, sinCos, one);
}

BOOST_AUTO_TEST_CASE(threeFourthPi)
/* Sin/cos/tan(3*Pi/4) = -1/sqrt(2), 1/sqrt(2), -1. */
{
    const BasePtr expectedSin = Power::oneOver(sqrtTwo);
    const BasePtr expectedCos = Product::minus(expectedSin);

    arg = Product::create(Numeric::create(3, 4), pi);

    check(expectedSin, expectedCos, minusOne);
}

BOOST_AUTO_TEST_CASE(fiveFourthPi)
/* Sin/cos/tan(5*Pi/4) = 1/sqrt(2), -1/sqrt(2), -1. */
{
    const BasePtr expectedSinCos = Product::minus(Power::oneOver(sqrtTwo));

    arg = Product::create(Numeric::create(5, 4), pi);

    check(expectedSinCos, expectedSinCos, one);
}

BOOST_AUTO_TEST_CASE(sevenFourthPi)
/* Sin/cos/tan(7*Pi/4) = -1/sqrt(2), 1/sqrt(2), -1. */
{
    const BasePtr expectedCos = Power::oneOver(sqrtTwo);
    const BasePtr expectedSin = Product::minus(expectedCos);

    arg = Product::create(Numeric::create(7, 4), pi);

    check(expectedSin, expectedCos, minusOne);
}

BOOST_AUTO_TEST_CASE(threePiOverEight)
/* Sin/cos/tan(3*Pi/8) = sqrt(2 + sqrt(2))/2, sqrt(2 - sqrt(2))/2, 1 + sqrt(2). */
{
    const BasePtr expectedSin = Product::create(half, Power::sqrt(Sum::create(two, sqrtTwo)));
    const BasePtr expectedCos = Product::create(half, Power::sqrt(Sum::create(two, Product::minus(sqrtTwo))));
    const BasePtr expectedTan = Sum::create(one, sqrtTwo);

    arg = Product::create(Numeric::create(3, 8), pi);

    check(expectedSin, expectedCos, expectedTan);
}

BOOST_AUTO_TEST_CASE(cosLeadsToNewAdjustment)
/* Cos(5/3*Pi) is computed via Sin(5/3*Pi + Pi/2), where the argument is again greater than 2*Pi
 * and needs to be shifted back. */
{
    const BasePtr expectedSin = Product::create(Numeric::create(-1, 2), sqrtThree);
    const BasePtr expectedTan = Product::minus(sqrtThree);

    arg = Product::create(Numeric::create(5, 3), pi);

    check(expectedSin, half, expectedTan);
}

BOOST_AUTO_TEST_CASE(unresolvableNumeric)
/* Sin/cos/tan(1/4) shouldn't be simplified. */
{
    arg = Numeric::fourth();

    checkUnsimplified();
}

BOOST_AUTO_TEST_CASE(unresolvableNumericPower)
/* Sin/cos/tan(sqrt(2)) shouldn't be simplified. */
{
    arg = sqrtTwo;

    checkUnsimplified();
}

BOOST_AUTO_TEST_CASE(exactFromDouble)
/* Sin/cos/tan(Pi/12.0) = (sqrt(6) - sqrt(2))/4, (sqrt(6) + sqrt(2))/4, 2 - sqrt(3). */
{
    const BasePtr expectedSin = Product::create(Numeric::fourth(), Sum::create(sqrtSix, Product::minus(sqrtTwo)));
    const BasePtr expectedCos = Product::create(Numeric::fourth(), Sum::create(sqrtSix, sqrtTwo));
    const BasePtr expectedTan = Sum::create(two, Product::minus(sqrtThree));

    arg = Numeric::create(Number(M_PI / 12.0));

    check(expectedSin, expectedCos, expectedTan);
}

BOOST_AUTO_TEST_CASE(exactFromNumericallyEvaluable)
/* A product of numerically evaluable factors, that matches an entry of the exact tables of the
 * class will lead to an exact result. */
{
    const BasePtr expectedSin = Power::oneOver(sqrtTwo);

    arg = Product::create(
      {Numeric::create(0.176351684975302), sqrtThree, Power::create(Numeric::create(17), Numeric::third())});

    check(Trigonometric::Type::SIN, expectedSin);
}

BOOST_AUTO_TEST_CASE(numericEvaluation)
/* Sin/cos/tan(1.23456789) should be evaluated to a plain Numeric. */
{
    const double doubleArg = 1.23456789;
    const double expectedSin = std::sin(doubleArg);
    const double expectedCos = std::cos(doubleArg);
    const double expectedTan = std::tan(doubleArg);

    arg = Numeric::create(doubleArg);

    /* The comparison will be evaluated with the internal double comparison tolerance of the Number
     * class. */
    check(Numeric::create(expectedSin), Numeric::create(expectedCos), Numeric::create(expectedTan));
}

BOOST_AUTO_TEST_CASE(numEvalSecondQuadrant)
/* Sin/cos/tan(2.3456789) should be evaluated to a plain Numeric. */
{
    const double doubleArg = 3.456789;
    const double expectedSin = std::sin(doubleArg);
    const double expectedCos = std::cos(doubleArg);
    const double expectedTan = std::tan(doubleArg);

    arg = Numeric::create(doubleArg);

    check(Numeric::create(expectedSin), Numeric::create(expectedCos), Numeric::create(expectedTan));
}

BOOST_AUTO_TEST_CASE(undefinedTan)
/* Tan(Pi/2) = Undefined. */
{
    arg = Product::create(half, pi);

    check(one, zero, undefined);
}

BOOST_AUTO_TEST_CASE(inverseZero)
/* Asin/acos/atan(0) = 0, Pi/2, 0. */
{
    arg = zero;

    checkInverse(zero, Product::create(half, pi), zero);
}

BOOST_AUTO_TEST_CASE(inverseOneOverSqrtTwo)
/* Asin/acos/atan(1/sqrt(2)) = Pi/4, Pi/4, unsimplified. */
{
    const BasePtr piFourth = Product::create(pi, Numeric::fourth());

    arg = Power::oneOver(sqrtTwo);

    check(Trigonometric::Type::ASIN, piFourth);
    check(Trigonometric::Type::ACOS, piFourth);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(inverseNegativeArg)
/* Asin/acos/atan(-1/sqrt(2)) = -Pi/4, 3*Pi/4, unsimplified. */
{
    const BasePtr expectedAsin = Product::create(Numeric::create(-1, 4), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(3, 4), pi);

    arg = Product::minus(Power::oneOver(sqrtTwo));

    check(Trigonometric::Type::ASIN, expectedAsin);
    check(Trigonometric::Type::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(inverseFromSum)
/* Asin/acos/atan((sqrt(6) - sqrt(2))/4  = Pi/12, 5*Pi/12, unsimplified. */
{
    const BasePtr expectedAsin = Product::create(Numeric::create(1, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(5, 12), pi);

    arg = Product::create(Numeric::fourth(), Sum::create(sqrtSix, Product::minus(sqrtTwo)));

    check(Trigonometric::Type::ASIN, expectedAsin);
    check(Trigonometric::Type::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(inverseNegativeSum)
/* Asin/acos/atan(-(sqrt(6) + sqrt(2))/4) = -5/12*pi, 11/12*pi, unsimplified. */
{
    const BasePtr expectedAsin = Product::create(Numeric::create(-5, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(11, 12), pi);

    arg = Product::create(Numeric::create(-1, 4), Sum::create(sqrtSix, sqrtTwo));

    check(Trigonometric::Type::ASIN, expectedAsin);
    check(Trigonometric::Type::ACOS, expectedAcos);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(atanNegativeArg)
/* Atan(-sqrt(3)) = -Pi/3. */
{
    const BasePtr expected = Product::create(Numeric::create(-1, 3), pi);

    arg = Product::minus(sqrtThree);

    check(Trigonometric::Type::ATAN, expected);
}

BOOST_AUTO_TEST_CASE(atanNegativeSum)
/* Atan(-sqrt(2) - 1) = -3/8*pi. */
{
    const BasePtr expected = Product::create(Numeric::create(-3, 8), pi);

    arg = Product::minus(Sum::create(sqrtTwo, one));

    check(Trigonometric::Type::ATAN, expected);
}

BOOST_AUTO_TEST_CASE(inverseExactFromDouble)
/* The same as before, but the argument is a double. This implies, that the inverse tangent
 * evaluates to a double, and is thus simplified. */
{
    const double doubleArg = 0.25 * (std::sqrt(6.0) - std::sqrt(2.0));
    const BasePtr expectedAsin = Product::create(Numeric::create(1, 12), pi);
    const BasePtr expectedAcos = Product::create(Numeric::create(5, 12), pi);
    const BasePtr expectedAtan = Numeric::create(std::atan(doubleArg));

    arg = Numeric::create(doubleArg);

    checkInverse(expectedAsin, expectedAcos, expectedAtan);
}

BOOST_AUTO_TEST_CASE(inverseFromSumAtan)
/* Asin/acos/atan(sqrt(2) + 1) = undefined, undefined, 3/8*Pi. */
{
    const BasePtr expectedAtan = Product::create(Numeric::create(3, 8), pi);

    arg = Sum::create(sqrtTwo, one);

    checkInverse(undefined, undefined, expectedAtan);
}

BOOST_AUTO_TEST_CASE(inverseExactFromDoubleAtan)
/* Asin/acos/atan(2 - sqrt(3)) = 0.27126(...), 1.2995(...), 1/12*Pi. */
{
    const BasePtr expectedAtan = Product::create(Numeric::create(1, 12), pi);
    const double doubleArg = 2.0 - std::sqrt(3.0);
    const BasePtr expectedAsin = Numeric::create(std::asin(doubleArg));
    const BasePtr expectedAcos = Numeric::create(std::acos(doubleArg));

    arg = Numeric::create(doubleArg);

    checkInverse(expectedAsin, expectedAcos, expectedAtan);
}

BOOST_AUTO_TEST_CASE(noSimplification)
/* Sin/cos/tan aren't simplified for a numerically evaluable argument, that isn't in the
 * exact tables. */
{
    arg = Product::create(sqrtTwo, pi);

    checkUnsimplified(Trigonometric::Type::SIN);
    checkUnsimplified(Trigonometric::Type::COS);
    checkUnsimplified(Trigonometric::Type::TAN);
}

BOOST_AUTO_TEST_CASE(noSimplificationInverse)
/* The same for inverse functions. */
{
    arg = Product::create(sqrtTwo, Numeric::create(1, 10), pi);

    checkUnsimplified(Trigonometric::Type::ASIN);
    checkUnsimplified(Trigonometric::Type::ACOS);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(noSimplificationLargeInput)
{
    const BasePtrList fac{Numeric::create(1, 20), pi, sqrtThree, Power::create(four, Numeric::create(-1, 5)),
      Power::create(six, Numeric::create(-1, 10))};

    arg = Product::create(fac);

    checkUnsimplified(Trigonometric::Type::SIN);
    checkUnsimplified(Trigonometric::Type::COS);
    checkUnsimplified(Trigonometric::Type::TAN);
    checkUnsimplified(Trigonometric::Type::ASIN);
    checkUnsimplified(Trigonometric::Type::ACOS);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(greaterThanRange)
/* Asin/acos aren't defined for arguments < -1. */
{
    arg = Product::create(Numeric::create(-17), sqrtTwo);

    check(Trigonometric::Type::ASIN, undefined);
    check(Trigonometric::Type::ACOS, undefined);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_CASE(lessThanRange)
/* The same for arguments > 1. */
{
    const double doubleArg = 1.0000001;

    arg = Numeric::create(doubleArg);

    checkInverse(undefined, undefined, Numeric::create(std::atan(doubleArg)));
}

BOOST_AUTO_TEST_CASE(largeNumericEvaluationToExact)
/* A product of numerically evaluable factors, that matches an entry of the exact tables of the
 * class will lead to an exact result. */
{
    const BasePtr piFourth = Product::create(pi, Numeric::fourth());

    arg = Product::create({Numeric::create(0.6258398439057556), Power::sqrt(five), Power::oneOver(pi),
      Power::create(four, Numeric::third())});

    check(Trigonometric::Type::ASIN, piFourth);
}

BOOST_AUTO_TEST_CASE(inverseUnresolvableNumeric)
/* Asin/acos/atan(-1/10) shouldn't be simplified. */
{
    arg = Numeric::create(-1, 10);

    checkUnsimplified(Trigonometric::Type::ASIN);
    checkUnsimplified(Trigonometric::Type::ACOS);
    checkUnsimplified(Trigonometric::Type::ATAN);
}

BOOST_AUTO_TEST_SUITE_END()
