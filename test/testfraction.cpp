
#include "abc.h"
#include "fraction.h"
#include "power.h"
#include "product.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Fraction)
{
    Fraction aOverB;

    void setup()
    {
        aOverB = Fraction(a, b);
    }
};

TEST(Fraction, constructionWithoutParameter)
{
    const Fraction frac;

    CHECK(frac.num()->isUndefined());
    CHECK_EQUAL(one, frac.denom());
}

TEST(Fraction, constructWithoutDenom)
{
    const Fraction frac(a);

    CHECK_EQUAL(a, frac.num());
    CHECK_EQUAL(one, frac.denom());
}

TEST(Fraction, constructWithNumAndDenom)
{
    CHECK_EQUAL(a, aOverB.num());
    CHECK_EQUAL(b, aOverB.denom());
}

TEST(Fraction, inversion)
{
    Fraction result;

    result = aOverB.invert();

    CHECK_EQUAL(b, result.num());
    CHECK_EQUAL(a, result.denom());
}

TEST(Fraction, evaluation)
{
    const BasePtr expected = Product::create(a, Power::oneOver(b));
    const BasePtr result = aOverB.eval();

    CHECK_EQUAL(expected, result);
}

TEST(Fraction, cancelZeroNumerator)
{
    const Fraction frac(zero, b);
    Fraction result;

    result = frac.cancel();

    CHECK_EQUAL(zero, result.num());
    CHECK_EQUAL(one, result.denom());
}

TEST(Fraction, cancelZeroDenominator)
{
    const Fraction frac(a, zero);
    Fraction result;

    disableLog();
    result = frac.cancel();
    enableLog();

    CHECK(result.num()->isUndefined());
    CHECK_EQUAL(one, result.denom());
}

TEST(Fraction, evalZeroDenominator)
{
    const Fraction frac(a, zero);
    BasePtr result;

    disableLog();
    result = frac.eval();
    enableLog();

    CHECK(result->isUndefined());
}
