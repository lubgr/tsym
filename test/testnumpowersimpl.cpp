
#include <limits>
#include <cmath>
#include "numpowersimpl.h"
#include "logging.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(NumPowerSimpl)
{
    Int defaultMaxPrimeLimit;
    NumPowerSimpl nps;
    Number half;
    Number third;
    double TOL;

    void setup()
    {
        half = Number(1, 2);
        third = Number(1, 3);
        TOL = 1.e-10;

        defaultMaxPrimeLimit = NumPowerSimpl::getMaxPrimeResolution();
    }

    void teardown()
    {
        nps = NumPowerSimpl();
        enableLog();

        NumPowerSimpl::setMaxPrimeResolution(defaultMaxPrimeLimit);
    }

    void check(const Number& preFac, const Number& base, const Number& exp)
    {
        check(preFac, nps.getPreFactor());
        check(base, nps.getNewBase());
        check(exp, nps.getNewExp());
    }

    void check(const Number& expected, const Number& given)
    {
        if (expected.isRational() && given.isRational()) {
            CHECK_EQUAL(expected, given);
        } else {
            CHECK(expected.isDouble());
            CHECK(given.isDouble());
            DOUBLES_EQUAL(expected.toDouble(), given.toDouble(), TOL);
        }
    }

    bool isUndefined()
    {
        return nps.getNewBase().isUndefined() && nps.getNewExp().isUndefined() &&
            nps.getPreFactor().isUndefined();
    }
};

TEST(NumPowerSimpl, doubleBase)
{
    const Number exp(4, 3);

    nps.setPower(0.1234567, exp);

    check(1, 0.06147325630530969, 1);
}

TEST(NumPowerSimpl, doubleBaseWithPrefactor)
{
    const Number exp(4, 3);

    nps.setPower(0.1234567, exp);
    nps.setPreFac(5);

    check(1, 0.30736628152654843, 1);
}

TEST(NumPowerSimpl, doubleExp)
{
    const Number exp(12.3456789);

    nps.setPower(Number(5, 2), exp);

    check(1, 81816.24972559595, 1);
}

TEST(NumPowerSimpl, doubleExpWithPrefactor)
{
    const Number exp(12.3456789);

    nps.setPreFac(Number(13, 17));
    nps.setPower(Number(5, 2), exp);

    check(1, 62565.367437220426, 1);
}

TEST(NumPowerSimpl, doublePreFactor)
{
    const double prefac = 1.23456789;

    nps.setPreFac(prefac);
    nps.setPower(32, half);

    check(4.0*prefac, 2, half);
}

TEST(NumPowerSimpl, fractionBaseToIntBase)
    /* sqrt(1/2) = 2^(-1/2). */
{
    nps.setPower(half, half);

    check(1, 2, -half);
}

TEST(NumPowerSimpl, fractionBaseWithPrefac)
    /* 1/4*sqrt(2) = 1/2*2^(-1/2). */
{
    nps.setPower(2, half);
    nps.setPreFac(Number(1, 4));

    check(half, 2, Number(-1, 2));
}

TEST(NumPowerSimpl, intBaseToFractionBase)
    /* 3^(-1) = 1/3. */
{
    nps.setPower(3, -1);

    check(1, third, 1);
}

TEST(NumPowerSimpl, fractionBaseWithNegativeExp)
    /* (2/3)^(-1/5) = (3/2)^(1/5). */
{
    nps.setPower(Number(2, 3), Number(-1, 5));

    check(1, Number(3, 2), Number(1, 5));
}

TEST(NumPowerSimpl, expGreaterThanOne)
    /* (5/3)^(4/3) = (5/3)*(5/3)^(1/3). */
{
    const Number base(5, 3);

    nps.setPower(base, Number(4, 3));

    check(base, base, Number(1, 3));
}

TEST(NumPowerSimpl, expGreaterThanThree)
    /* 2^(10/3) = 8*2^(1/3). */
{
    nps.setPower(2, Number(10, 3));

    check(8, 2, Number(1, 3));
}

TEST(NumPowerSimpl, expGreaterThanOneWithPreFac)
    /* 13*4^(7/3) = 208*2^(2, 3). */
{
    nps.setPreFac(13);
    nps.setPower(4, Number(7, 3));

    check(208, 2, Number(2, 3));
}

TEST(NumPowerSimpl, expLessThanMinusOne)
    /* 5^(-14/3) = (1/625)*5^(-2/3). */
{
    nps.setPower(5, Number(-14, 3));

    check(Number(1, 625), 5, Number(-2, 3));
}

TEST(NumPowerSimpl, negativePreFactor)
    /* No change of (-3)*sqrt(2). */
{
    nps.setPower(2, half);
    nps.setPreFac(-3);

    check(-3, 2, half);
}

TEST(NumPowerSimpl, negPreFactorDoubleExp)
    /* (-1)*2^0.1234567 shall be evaluated to double. */
{
    const double exp = 0.1234567;
    const double expected = -std::pow(2, exp);

    nps.setPower(2, exp);
    nps.setPreFac(-1);

    check(1, expected, 1);
}

TEST(NumPowerSimpl, negPreFactorResolvablePow)
    /* (-2)*sqrt(9) = -6. */
{
    nps.setPreFac(-2);
    nps.setPower(9, half);

    check(1, -6, 1);
}

TEST(NumPowerSimpl, positiveBaseNegativePrefactor)
    /* (-12(*7^(1/3) = (-12)*7^(1/3). */
{
    nps.setPower(7, third);
    nps.setPreFac(-12);

    check(-12, 7, third);
}

TEST(NumPowerSimpl, negativePreFactorPositiveBase)
    /* (-9)*3^(-1/5) = 3*3^(4/5). */
{
    const Number exp(-1, 5);

    nps.setPower(3, exp);
    nps.setPreFac(-9);

    check(-3, 3, Number(4, 5));
}

TEST(NumPowerSimpl, negativeBaseFracExponentToUndefined)
    /* (-3)^(2/5) is undefined. */
{
    const Number exp(2, 5);

    nps.setPower(-3, exp);

    CHECK(isUndefined());
}

TEST(NumPowerSimpl, negativeBaseDoubleExpToUndefined)
    /* (-3/2)^(1.2345678) is undefined. */
{
    nps.setPower(Number(-3, 2), 1.2345678);

    CHECK(isUndefined());
}

TEST(NumPowerSimpl, negativeDoubleBaseFracExpToUndefined)
    /* (-0.12345678)^(2/5) is undefined. */
{
    nps.setPower(-0.12345678, Number(2, 5));

    CHECK(isUndefined());
}

TEST(NumPowerSimpl, undefinedBase)
{
    Number base(1);

    disableLog();
    base /= 0;
    enableLog();

    nps.setPower(base, half);

    CHECK(isUndefined());
}

TEST(NumPowerSimpl, undefinedExp)
    /* If one component is undefined, all the others will be, too. */
{
    Number exp(1);

    disableLog();
    exp /= 0;
    enableLog();

    nps.setPower(13, exp);
    nps.setPreFac(-5);

    CHECK(isUndefined());
}

TEST(NumPowerSimpl, undefinedPreFactor)
    /* Same as above. */
{
    Number preFac(1);

    disableLog();
    preFac /= 0;
    enableLog();

    nps.setPower(17, half);
    nps.setPreFac(preFac);

    CHECK(isUndefined());
}

TEST(NumPowerSimpl, allOne)
{
    nps.setPower(1, 1);

    check(1, 1, 1);
}

TEST(NumPowerSimpl, simpleResolvableSquareRoot)
    /* sqrt(4) = 2. */
{
    nps.setPower(4, half);

    check(1, 2, 1);
}

TEST(NumPowerSimpl, simpleSplittableSquareRoot)
    /* sqrt(12) = 2*sqrt(3). */
{
    nps.setPower(12, half);

    check(2, 3, half);
}

TEST(NumPowerSimpl, smallIntWithoutChange)
    /* 9*3^(1/5) isn't altered. */
{
    const Number exp(1, 5);

    nps.setPower(3, exp);
    nps.setPreFac(9);

    check(9, 3, exp);
}

TEST(NumPowerSimpl, intWithoutChange)
    /* 625*5^(2/3) isn't altered. */
{
    const Number exp(2, 3);

    nps.setPower(5, exp);
    nps.setPreFac(625);

    check(625, 5, exp);
}

TEST(NumPowerSimpl, simplePrimeFac)
    /* sqrt(8) = 2*sqrt(2). */
{
    nps.setPower(8, half);

    check(2, 2, half);
}

TEST(NumPowerSimpl, intPrimeFactorization)
    /* sqrt(833) = 7*sqrt(17). */
{
    nps.setPower(833, half);

    check(7, 17, half);
}

TEST(NumPowerSimpl, fractionPrimeFactorization)
    /* 9/25*(28/117)^(2/3) = 1/25*(84/13)^(2/3). */
{
    const Number exp(2, 3);

    nps.setPower(Number(28, 117), exp);
    nps.setPreFac(Number(9, 25));

    check(Number(1, 25), Number(84, 13), exp);
}

TEST(NumPowerSimpl, intCollectionOfPrimes)
    /* 25^(1/3) = 5^(2/3). */
{
    nps.setPower(25, third);

    check(1, 5, Number(2, 3));
}

TEST(NumPowerSimpl, noIntCollectionOfPrimes)
    /* 18^(1/3) isn't altered. */
{
    nps.setPower(18, third);

    check(1, 18, third);
}

TEST(NumPowerSimpl, simpleFractionCollectionToPreFactor)
    /* 4^(2/3) = 2*2^(1/3). */
{
    nps.setPower(4, Number(2, 3));

    check(2, 2, third);
}

TEST(NumPowerSimpl, fractionCollectionToPreFactor)
    /* (8/27)^(5/6) = 4/9*sqrt(2/3). */
{
    nps.setPower(Number(8, 27), Number(5, 6));

    check(Number(4, 9), Number(2, 3), half);
}

TEST(NumPowerSimpl, fractionNoCollectionToPreFactor)
    /* (4/27)^(5/6) isn't altered. */
{
    const Number exp(5, 6);
    const Number base(4, 27);

    nps.setPower(base, exp);

    check(1, base, exp);
}

TEST(NumPowerSimpl, multipleSets)
    /* Checks whether the internal mechanism to minimize the computation effort works, after a new
     * rational number has been set. */
{
    const Number exp(1, 4);

    nps.setPower(2, exp);

    check(1, 2, exp);

    nps.setPower(3, exp);

    check(1, 3, exp);
}

TEST(NumPowerSimpl, largePlainIntBasePosExp)
    /* A large base with fraction exponent > 1 is not automatically simplified to something else. */
{
    const Number large(std::numeric_limits<int>::max() - 5);
    const Number exp(9, 4);

    nps.setMaxPrimeResolution(100);
    nps.setPower(large, exp);

    check(1, large, exp);

    nps.setMaxPrimeResolution(std::numeric_limits<int>::max());
    nps.setPower(large, exp);

    check(Number(Int("4611685992657584164")), large, Number(1, 4));
}

TEST(NumPowerSimpl, largePosPreFac)
{
    const Number large = Number(std::numeric_limits<int>::max() - 111);
    const Number exp(4, 3);

    nps.setMaxPrimeResolution(10000);

    nps.setPower(3, exp);
    nps.setPreFac(large);

    check(large, 3, exp);
}
