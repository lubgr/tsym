
#include <initializer_list>
#include "primefac.h"
#include "tsymtests.h"
#include "number.h"

using namespace tsym;

struct PrimeFacFixture {
    const Number half = Number(1, 2);
    const Number third = Number(1, 3);
    const Number twoThird = Number(2, 3);
    PrimeFac pf{};

    void checkNum(std::initializer_list<Int> primes)
    {
        check(primes, pf.getNumPrimes());
    }

    void checkDenom(std::initializer_list<Int> primes)
    {
        check(primes, pf.getDenomPrimes());
    }

    void check(const std::vector<Int>& expected, const std::vector<Int>& primes)
    {
        BOOST_CHECK_EQUAL_COLLECTIONS(begin(expected), end(expected), begin(primes), end(primes));
    }
};

BOOST_FIXTURE_TEST_SUITE(TestPrimeFac, PrimeFacFixture)

BOOST_AUTO_TEST_CASE(doubleArgument)
{
    pf = PrimeFac(0.123456789);

    BOOST_TEST(pf.getNumPrimes().empty());
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(negativeArgument)
{
    pf = PrimeFac(-2);

    BOOST_TEST(pf.getNumPrimes().empty());
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(simpleInt)
{
    pf = PrimeFac(13650);

    checkNum({2, 3, 5, 5, 7, 13});
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(simpleFraction)
{
    pf = PrimeFac(twoThird);

    checkNum({2});
    checkDenom({3});
}

BOOST_AUTO_TEST_CASE(largeFraction)
{
    pf = PrimeFac(Number(6578, 4515));

    checkNum({2, 11, 13, 23});
    checkDenom({3, 5, 7, 43});
}

BOOST_AUTO_TEST_CASE(positivePower)
{
    pf = PrimeFac(Number(4, 3));
    pf.toThe(3);

    checkNum({2, 2, 2, 2, 2, 2});
    checkDenom({3, 3, 3});
}

BOOST_AUTO_TEST_CASE(negativePower)
{
    pf = PrimeFac(Number(9, 17));
    pf.toThe(-2);

    checkNum({17, 17});
    checkDenom({3, 3, 3, 3});
}

BOOST_AUTO_TEST_CASE(powerWithZeroExponent)
{
    pf = PrimeFac(7);
    pf.toThe(0);

    BOOST_TEST(pf.getNumPrimes().empty());
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(simpleProduct)
{
    pf = PrimeFac(Number(7, 2));
    pf.multiply(Number(3, 2));

    checkNum({3, 7});
    checkDenom({2, 2});
}

BOOST_AUTO_TEST_CASE(productWithEmptyPrimes)
{
    pf = PrimeFac(36);
    pf.multiply(1);

    checkNum({2, 2, 3, 3});
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(product)
{
    PrimeFac other(Number(65, 46));

    pf = PrimeFac(Number(154, 975));
    pf.multiply(other);

    checkNum({7, 11});
    checkDenom({3, 5, 23});
}

BOOST_AUTO_TEST_CASE(noExtractionFromInt)
    /* Nothing is extracted from 5^(2/3). */
{
    PrimeFac extraction;

    pf = PrimeFac(5);

    extraction = pf.extract(twoThird);

    checkNum({5});
    BOOST_TEST(pf.getDenomPrimes().empty());

    pf = extraction;

    BOOST_TEST(pf.getNumPrimes().empty());
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(simpleExtraction)
    /* Extraction from sqrt(4): 2. */
{
    PrimeFac extraction;

    pf = PrimeFac(4);

    extraction = pf.extract(Number(1, 2));

    BOOST_TEST(pf.getNumPrimes().empty());
    BOOST_TEST(pf.getDenomPrimes().empty());

    pf = extraction;

    checkNum({2});
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(extraction)
    /* Extraction from sqrt(12): 2. */
{
    PrimeFac extraction;

    pf = PrimeFac(12);

    extraction = pf.extract(Number(1, 2));

    checkNum({3});
    BOOST_TEST(pf.getDenomPrimes().empty());

    pf = extraction;

    checkNum({2});
    BOOST_TEST(pf.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(noExtractionFromFraction)
    /* Extraction from (297/65000)^(2/5): 3/10.*/
{
    PrimeFac extraction;

    pf = PrimeFac(Number(297, 65000));
    extraction = pf.extract(Number(2, 5));

    BOOST_TEST(extraction.getNumPrimes().empty());
    BOOST_TEST(extraction.getDenomPrimes().empty());
}

BOOST_AUTO_TEST_CASE(extractionFromFraction)
    /* Extraction from (297/65000)^(2/3): 9/100.*/
{
    PrimeFac extraction;

    pf = PrimeFac(Number(297, 65000));
    extraction = pf.extract(twoThird);

    checkNum({11});
    checkDenom({5, 13});

    pf = extraction;

    checkNum({3, 3});
    checkDenom({2, 2, 5, 5});
}

BOOST_AUTO_TEST_CASE(extractionNegativeExponent)
    /* Extraction from (4/9)^(-3/2): 27/8. */
{
    PrimeFac extraction;

    pf = PrimeFac(Number(4, 9));
    extraction = pf.extract(Number(-3, 2));

    BOOST_TEST(pf.getNumPrimes().empty());
    BOOST_TEST(pf.getDenomPrimes().empty());

    pf = extraction;

    checkNum({3, 3, 3});
    checkDenom({2, 2, 2});
}

BOOST_AUTO_TEST_CASE(emptyCount)
    /* Count of 1: 0. */
{
    int count;

    pf = PrimeFac(1);
    count = pf.getEqualCount();

    BOOST_CHECK_EQUAL(0, count);
}

BOOST_AUTO_TEST_CASE(intCount)
    /* Count of (2^5*3^5): 5. */
{
    int count;

    pf = PrimeFac(7776);
    count = pf.getEqualCount();

    BOOST_CHECK_EQUAL(5, count);
}

BOOST_AUTO_TEST_CASE(intCountNonEqual)
    /* Count of (2^3*3^3*7^2*11^3) : 0. */
{
    int count;

    pf = PrimeFac(14087304);
    count = pf.getEqualCount();

    BOOST_CHECK_EQUAL(0, count);
}

BOOST_AUTO_TEST_CASE(fractionCount)
    /* Count of (2^4*3^4)/(7^4): 4. */
{
    int count;

    pf = PrimeFac(Number(1296, 2401));
    count = pf.getEqualCount();

    BOOST_CHECK_EQUAL(4, count);
}

BOOST_AUTO_TEST_CASE(fractionCountNumeratorOne)
    /* 1/(2*2*3*3*5*5*7*7*11*11) : 2. */
{
    int count;

    pf = PrimeFac(Number(1, 5336100));
    count = pf.getEqualCount();

    BOOST_CHECK_EQUAL(2, count);
}

BOOST_AUTO_TEST_CASE(fractionCountNonEqual)
    /* Count of (2*3*5*7)/(11*13*13) : 0. */
{
    int count;

    pf = PrimeFac(Number(210, 1859));
    count = pf.getEqualCount();

    BOOST_CHECK_EQUAL(0, count);
}

BOOST_AUTO_TEST_CASE(intCollection)
    /* 25^(1/3) = 5^(2/3). */
{
    Number newExp;

    pf = PrimeFac(25);
    newExp = pf.collectToNewExp(third);

    checkNum({5});
    BOOST_TEST(pf.getDenomPrimes().empty());

    BOOST_CHECK_EQUAL(twoThird, newExp);
}

BOOST_AUTO_TEST_CASE(intNoCollection)
    /* 18^(1/3) isn't altered. */
{
    Number newExp;

    pf = PrimeFac(18);
    newExp = pf.collectToNewExp(third);

    checkNum({2, 3, 3});
    BOOST_TEST(pf.getDenomPrimes().empty());

    BOOST_CHECK_EQUAL(third, newExp);
}

BOOST_AUTO_TEST_CASE(simpleFractionCollection)
    /* 4^(2/3) = 2^(4/3). Results in an exponent greater than one (no extraction performed). */
{
    Number newExp;

    pf = PrimeFac(4);
    newExp = pf.collectToNewExp(twoThird);

    checkNum({2});
    BOOST_TEST(pf.getDenomPrimes().empty());

    BOOST_CHECK_EQUAL(Number(4, 3), newExp);
}

BOOST_AUTO_TEST_CASE(fractionCollection)
    /* (8/27)^(5/6) = (2/3)^(15/6), same as above but with a fraction base. */
{
    Number newExp;

    pf = PrimeFac(Number(8, 27));
    newExp = pf.collectToNewExp(Number(5, 6));

    checkNum({2});
    checkDenom({3});

    BOOST_CHECK_EQUAL(Number(15, 6), newExp);
}

BOOST_AUTO_TEST_CASE(emptyColletion)
    /* 1^(2/3) = 1^1. */
{
    Number newExp;

    pf = PrimeFac(1);
    newExp = pf.collectToNewExp(Number(2, 3));

    BOOST_TEST(pf.getNumPrimes().empty());
    BOOST_TEST(pf.getDenomPrimes().empty());

    BOOST_CHECK_EQUAL(1, newExp);
}

BOOST_AUTO_TEST_CASE(evaluate)
{
    const Number n(10626, 3211);

    pf = PrimeFac(n);

    BOOST_CHECK_EQUAL(n, pf.eval());
}

BOOST_AUTO_TEST_SUITE_END()
