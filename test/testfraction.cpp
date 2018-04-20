
#include "fraction.h"
#include "power.h"
#include "product.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestFraction, AbcFixture)

BOOST_AUTO_TEST_CASE(constructionWithoutParameter)
{
    const Fraction frac;

    BOOST_TEST(frac.num()->isUndefined());
    BOOST_CHECK_EQUAL(one, frac.denom());
}

BOOST_AUTO_TEST_CASE(constructWithoutDenom)
{
    const Fraction frac(a);

    BOOST_CHECK_EQUAL(a, frac.num());
    BOOST_CHECK_EQUAL(one, frac.denom());
}

BOOST_AUTO_TEST_CASE(constructWithNumAndDenom)
{
    const Fraction aOverB = Fraction(a, b);
    BOOST_CHECK_EQUAL(a, aOverB.num());
    BOOST_CHECK_EQUAL(b, aOverB.denom());
}

BOOST_AUTO_TEST_CASE(inversion)
{
    const Fraction aOverB = Fraction(a, b);
    Fraction result;

    result = aOverB.invert();

    BOOST_CHECK_EQUAL(b, result.num());
    BOOST_CHECK_EQUAL(a, result.denom());
}

BOOST_AUTO_TEST_CASE(evaluation)
{
    const Fraction aOverB = Fraction(a, b);
    const BasePtr expected = Product::create(a, Power::oneOver(b));
    const BasePtr result = aOverB.eval();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(cancelZeroNumerator)
{
    const Fraction frac(zero, b);
    Fraction result;

    result = frac.cancel();

    BOOST_CHECK_EQUAL(zero, result.num());
    BOOST_CHECK_EQUAL(one, result.denom());
}

BOOST_AUTO_TEST_CASE(cancelZeroDenominator, noLogs())
{
    const Fraction frac(a, zero);
    Fraction result;

    result = frac.cancel();

    BOOST_TEST(result.num()->isUndefined());
    BOOST_CHECK_EQUAL(one, result.denom());
}

BOOST_AUTO_TEST_CASE(evalZeroDenominator, noLogs())
{
    const Fraction frac(a, zero);
    BasePtr result;

    result = frac.eval();

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_SUITE_END()
