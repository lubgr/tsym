
#include <cmath>
#include <limits>
#include "fixtures.h"
#include "logging.h"
#include "numpowersimpl.h"
#include "options.h"
#include "tsymtests.h"

using namespace tsym;

struct NumPowerSimplFixture {
    const Number half = Number(1, 2);
    const Number third = Number(1, 3);
    const double TOL = 1.e-10;
    Number preFac = 1;
    Number base;
    Number exp;
    Int primeResolution;

    void setPower(const Number& base, const Number& exp)
    {
        this->base = base;
        this->exp = exp;
    }

    void setPrefac(const Number& preFac)
    {
        this->preFac = preFac;
    }

    void setPrimeResolution(const Int& limit)
    {
        primeResolution = limit;
    }

    void check(const Number& preFac, const Number& base, const Number& exp)
    {
        const NumPowerSimpl instance({base, exp, preFac});

        check(preFac, instance.getPreFactor());
        check(base, instance.getNewBase());
        check(exp, instance.getNewExp());
    }

    void check(const Number& expected, const Number& given)
    {
        if (expected.isRational() && given.isRational()) {
            BOOST_CHECK_EQUAL(expected, given);
        } else {
            BOOST_TEST(expected.isDouble());
            BOOST_TEST(given.isDouble());
            BOOST_CHECK_CLOSE(expected.toDouble(), given.toDouble(), TOL);
        }
    }

    bool isUndefined()
    {
        const NumPowerSimpl instance({base, exp, preFac});

        return !instance.isInputValid();
    }
};

BOOST_FIXTURE_TEST_SUITE(TestNumPowerSimpl, NumPowerSimplFixture)

BOOST_AUTO_TEST_CASE(doubleBase)
{
    const Number exp(4, 3);

    setPower(0.1234567, exp);

    check(1, 0.06147325630530969, 1);
}

BOOST_AUTO_TEST_CASE(doubleBaseWithPrefactor)
{
    const Number exp(4, 3);

    setPower(0.1234567, exp);
    setPrefac(5);

    check(1, 0.30736628152654843, 1);
}

BOOST_AUTO_TEST_CASE(doubleExp)
{
    const Number exp(12.3456789);

    setPower(Number(5, 2), exp);

    check(1, 81816.24972559595, 1);
}

BOOST_AUTO_TEST_CASE(doubleExpWithPrefactor)
{
    const Number exp(12.3456789);

    setPrefac(Number(13, 17));
    setPower(Number(5, 2), exp);

    check(1, 62565.367437220426, 1);
}

BOOST_AUTO_TEST_CASE(doublePreFactor)
{
    const double prefac = 1.23456789;

    setPrefac(prefac);
    setPower(32, half);

    check(4.0 * prefac, 2, half);
}

BOOST_AUTO_TEST_CASE(fractionBaseToIntBase)
/* sqrt(1/2) = 2^(-1/2). */
{
    setPower(half, half);

    check(1, 2, -half);
}

BOOST_AUTO_TEST_CASE(fractionBaseWithPrefac)
/* 1/4*sqrt(2) = 1/2*2^(-1/2). */
{
    setPower(2, half);
    setPrefac(Number(1, 4));

    check(half, 2, Number(-1, 2));
}

BOOST_AUTO_TEST_CASE(intBaseToFractionBase)
/* 3^(-1) = 1/3. */
{
    setPower(3, -1);

    check(1, third, 1);
}

BOOST_AUTO_TEST_CASE(fractionBaseWithNegativeExp)
/* (2/3)^(-1/5) = (3/2)^(1/5). */
{
    setPower(Number(2, 3), Number(-1, 5));

    check(1, Number(3, 2), Number(1, 5));
}

BOOST_AUTO_TEST_CASE(expGreaterThanOne)
/* (5/3)^(4/3) = (5/3)*(5/3)^(1/3). */
{
    const Number base(5, 3);

    setPower(base, Number(4, 3));

    check(base, base, Number(1, 3));
}

BOOST_AUTO_TEST_CASE(expGreaterThanThree)
/* 2^(10/3) = 8*2^(1/3). */
{
    setPower(2, Number(10, 3));

    check(8, 2, Number(1, 3));
}

BOOST_AUTO_TEST_CASE(expGreaterThanOneWithPreFac)
/* 13*4^(7/3) = 208*2^(2, 3). */
{
    setPrefac(13);
    setPower(4, Number(7, 3));

    check(208, 2, Number(2, 3));
}

BOOST_AUTO_TEST_CASE(expLessThanMinusOne)
/* 5^(-14/3) = (1/625)*5^(-2/3). */
{
    setPower(5, Number(-14, 3));

    check(Number(1, 625), 5, Number(-2, 3));
}

BOOST_AUTO_TEST_CASE(negativePreFactor)
/* No change of (-3)*sqrt(2). */
{
    setPower(2, half);
    setPrefac(-3);

    check(-3, 2, half);
}

BOOST_AUTO_TEST_CASE(negPreFactorDoubleExp)
/* (-1)*2^0.1234567 shall be evaluated to double. */
{
    const double exp = 0.1234567;
    const double expected = -std::pow(2, exp);

    setPower(2, exp);
    setPrefac(-1);

    check(1, expected, 1);
}

BOOST_AUTO_TEST_CASE(negPreFactorResolvablePow)
/* (-2)*sqrt(9) = -6. */
{
    setPrefac(-2);
    setPower(9, half);

    check(1, -6, 1);
}

BOOST_AUTO_TEST_CASE(positiveBaseNegativePrefactor)
/* (-12(*7^(1/3) = (-12)*7^(1/3). */
{
    setPower(7, third);
    setPrefac(-12);

    check(-12, 7, third);
}

BOOST_AUTO_TEST_CASE(negativePreFactorPositiveBase)
/* (-9)*3^(-1/5) = 3*3^(4/5). */
{
    const Number exp(-1, 5);

    setPower(3, exp);
    setPrefac(-9);

    check(-3, 3, Number(4, 5));
}

BOOST_AUTO_TEST_CASE(negativeBaseFracExponentToUndefined, noLogs())
/* (-3)^(2/5) is undefined. */
{
    const Number exp(2, 5);

    setPower(-3, exp);

    BOOST_TEST(isUndefined());
}

BOOST_AUTO_TEST_CASE(negativeBaseDoubleExpToUndefined, noLogs())
/* (-3/2)^(1.2345678) is undefined. */
{
    setPower(Number(-3, 2), 1.2345678);

    BOOST_TEST(isUndefined());
}

BOOST_AUTO_TEST_CASE(negativeDoubleBaseFracExpToUndefined, noLogs())
/* (-0.12345678)^(2/5) is undefined. */
{
    setPower(-0.12345678, Number(2, 5));

    BOOST_TEST(isUndefined());
}

BOOST_AUTO_TEST_CASE(allOne)
{
    setPower(1, 1);

    check(1, 1, 1);
}

BOOST_AUTO_TEST_CASE(simpleResolvableSquareRoot)
/* sqrt(4) = 2. */
{
    setPower(4, half);

    check(1, 2, 1);
}

BOOST_AUTO_TEST_CASE(simpleSplittableSquareRoot)
/* sqrt(12) = 2*sqrt(3). */
{
    setPower(12, half);

    check(2, 3, half);
}

BOOST_AUTO_TEST_CASE(smallIntWithoutChange)
/* 9*3^(1/5) isn't altered. */
{
    const Number exp(1, 5);

    setPower(3, exp);
    setPrefac(9);

    check(9, 3, exp);
}

BOOST_AUTO_TEST_CASE(intWithoutChange)
/* 625*5^(2/3) isn't altered. */
{
    const Number exp(2, 3);

    setPower(5, exp);
    setPrefac(625);

    check(625, 5, exp);
}

BOOST_AUTO_TEST_CASE(simplePrimeFac)
/* sqrt(8) = 2*sqrt(2). */
{
    setPower(8, half);

    check(2, 2, half);
}

BOOST_AUTO_TEST_CASE(intPrimeFactorization)
/* sqrt(833) = 7*sqrt(17). */
{
    setPower(833, half);

    check(7, 17, half);
}

BOOST_AUTO_TEST_CASE(fractionPrimeFactorization)
/* 9/25*(28/117)^(2/3) = 1/25*(84/13)^(2/3). */
{
    const Number exp(2, 3);

    setPower(Number(28, 117), exp);
    setPrefac(Number(9, 25));

    check(Number(1, 25), Number(84, 13), exp);
}

BOOST_AUTO_TEST_CASE(intCollectionOfPrimes)
/* 25^(1/3) = 5^(2/3). */
{
    setPower(25, third);

    check(1, 5, Number(2, 3));
}

BOOST_AUTO_TEST_CASE(noIntCollectionOfPrimes)
/* 18^(1/3) isn't altered. */
{
    setPower(18, third);

    check(1, 18, third);
}

BOOST_AUTO_TEST_CASE(simpleFractionCollectionToPreFactor)
/* 4^(2/3) = 2*2^(1/3). */
{
    setPower(4, Number(2, 3));

    check(2, 2, third);
}

BOOST_AUTO_TEST_CASE(fractionCollectionToPreFactor)
/* (8/27)^(5/6) = 4/9*sqrt(2/3). */
{
    setPower(Number(8, 27), Number(5, 6));

    check(Number(4, 9), Number(2, 3), half);
}

BOOST_AUTO_TEST_CASE(fractionNoCollectionToPreFactor)
/* (4/27)^(5/6) isn't altered. */
{
    const Number exp(5, 6);
    const Number base(4, 27);

    setPower(base, exp);

    check(1, base, exp);
}

BOOST_AUTO_TEST_CASE(multipleSets)
/* Checks whether the internal mechanism to minimize the computation effort works, after a new
 * rational number has been set. */
{
    const Number exp(1, 4);

    setPower(2, exp);

    check(1, 2, exp);

    setPower(3, exp);

    check(1, 3, exp);
}

BOOST_AUTO_TEST_CASE(largePlainIntBasePosExp)
/* A large base with fraction exponent > 1 is not automatically simplified to something else. */
{
    const Number large(std::numeric_limits<int>::max() - 5);
    const Number exp(9, 4);

    NumPowerSimpl nps({large, exp, 1}, 100);

    BOOST_CHECK_EQUAL(1, nps.getPreFactor());
    BOOST_CHECK_EQUAL(large, nps.getNewBase());
    BOOST_CHECK_EQUAL(exp, nps.getNewExp());

    nps = NumPowerSimpl({large, exp, 1}, std::numeric_limits<int>::max());

    BOOST_CHECK_EQUAL(Number(Int("4611685992657584164")), nps.getPreFactor());
    BOOST_CHECK_EQUAL(large, nps.getNewBase());
    BOOST_CHECK_EQUAL(Number(1, 4), nps.getNewExp());
}

BOOST_AUTO_TEST_CASE(largePosPreFac)
{
    const Number large = Number(std::numeric_limits<int>::max() - 111);
    const Number exp(4, 3);
    const NumPowerSimpl nps({3, exp, large}, 10000);

    BOOST_CHECK_EQUAL(large, nps.getPreFactor());
    BOOST_CHECK_EQUAL(3, nps.getNewBase());
    BOOST_CHECK_EQUAL(exp, nps.getNewExp());
}

BOOST_AUTO_TEST_SUITE_END()
