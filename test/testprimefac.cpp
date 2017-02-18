
#include "primefac.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(PrimeFac)
{
    PrimeFac pf;
    Number half;
    Number third;
    Number twoThird;

    void setup()
    {
        half = Number(1, 2);
        third = Number(1, 3);
        twoThird = Number(2, 3);
    }

    void checkNum(int p1, int p2 = 0, int p3 = 0, int p4 = 0, int p5 = 0, int p6 = 0)
    {
        const std::vector<Int> vec(getVec(p1, p2, p3, p4, p5, p6));

        check(vec, pf.getNumPrimes());
    }

    void checkDenom(int p1, int p2 = 0, int p3 = 0, int p4 = 0, int p5 = 0, int p6 = 0)
    {
        const std::vector<Int> vec(getVec(p1, p2, p3, p4, p5, p6));

        check(vec, pf.getDenomPrimes());
    }

    std::vector<Int> getVec(int p1, int p2 = 0, int p3 = 0, int p4 = 0, int p5 = 0, int p6 = 0)
    {
        const int values[6] = { p1, p2, p3, p4, p5, p6 };
        std::vector<Int> vec;

        for (int i = 0; i < 6; ++i)
            if (values[i] != 0)
                vec.push_back(Int(values[i]));

        return vec;
    }

    void check(const std::vector<Int>& expected, const std::vector<Int>& primes)
    {
        std::vector<Int>::const_iterator it;

        CHECK_EQUAL(expected.size(), primes.size());

        for (size_t i = 0; i < expected.size(); ++i)
            CHECK_EQUAL(expected[i], primes[i]);
    }
};

TEST(PrimeFac, doubleArgument)
{
    pf.set(0.123456789);

    CHECK(pf.getNumPrimes().empty());
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, negativeArgument)
{
    pf.set(-2);

    CHECK(pf.getNumPrimes().empty());
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, simpleInt)
{
    pf.set(13650);

    checkNum(2, 3, 5, 5, 7, 13);
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, simpleFraction)
{
    pf.set(twoThird);

    checkNum(2);
    checkDenom(3);
}

TEST(PrimeFac, largeFraction)
{
    pf.set(Number(6578, 4515));

    checkNum(2, 11, 13, 23);
    checkDenom(3, 5, 7, 43);
}

TEST(PrimeFac, positivePower)
{
    pf.set(Number(4, 3));
    pf.toThe(3);

    checkNum(2, 2, 2, 2, 2, 2);
    checkDenom(3, 3, 3);
}

TEST(PrimeFac, negativePower)
{
    pf.set(Number(9, 17));
    pf.toThe(-2);

    checkNum(17, 17);
    checkDenom(3, 3, 3, 3);
}

TEST(PrimeFac, powerWithZeroExponent)
{
    pf.set(7);
    pf.toThe(0);

    CHECK(pf.getNumPrimes().empty());
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, simpleProduct)
{
    pf.set(Number(7, 2));
    pf.multiply(Number(3, 2));

    checkNum(3, 7);
    checkDenom(2, 2);
}

TEST(PrimeFac, productWithEmptyPrimes)
{
    pf.set(36);
    pf.multiply(1);

    checkNum(2, 2, 3, 3);
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, product)
{
    PrimeFac other(Number(65, 46));

    pf.set(Number(154, 975));
    pf.multiply(other);

    checkNum(7, 11);
    checkDenom(3, 5, 23);
}

TEST(PrimeFac, noExtractionFromInt)
    /* Nothing is extracted from 5^(2/3). */
{
    PrimeFac extraction;

    pf.set(5);

    extraction = pf.extract(twoThird);

    checkNum(5);
    CHECK(pf.getDenomPrimes().empty());

    pf = extraction;

    CHECK(pf.getNumPrimes().empty());
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, simpleExtraction)
    /* Extraction from sqrt(4): 2. */
{
    PrimeFac extraction;

    pf.set(4);

    extraction = pf.extract(Number(1, 2));

    CHECK(pf.getNumPrimes().empty());
    CHECK(pf.getDenomPrimes().empty());

    pf = extraction;

    checkNum(2);
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, extraction)
    /* Extraction from sqrt(12): 2. */
{
    PrimeFac extraction;

    pf.set(12);

    extraction = pf.extract(Number(1, 2));

    checkNum(3);
    CHECK(pf.getDenomPrimes().empty());

    pf = extraction;

    checkNum(2);
    CHECK(pf.getDenomPrimes().empty());
}

TEST(PrimeFac, noExtractionFromFraction)
    /* Extraction from (297/65000)^(2/5): 3/10.*/
{
    PrimeFac extraction;

    pf.set(Number(297, 65000));
    extraction = pf.extract(Number(2, 5));

    CHECK(extraction.getNumPrimes().empty());
    CHECK(extraction.getDenomPrimes().empty());
}

TEST(PrimeFac, extractionFromFraction)
    /* Extraction from (297/65000)^(2/3): 9/100.*/
{
    PrimeFac extraction;

    pf.set(Number(297, 65000));
    extraction = pf.extract(twoThird);

    checkNum(11);
    checkDenom(5, 13);

    pf = extraction;

    checkNum(3, 3);
    checkDenom(2, 2, 5, 5);
}

TEST(PrimeFac, extractionNegativeExponent)
    /* Extraction from (4/9)^(-3/2): 27/8. */
{
    PrimeFac extraction;

    pf.set(Number(4, 9));
    extraction = pf.extract(Number(-3, 2));

    CHECK(pf.getNumPrimes().empty());
    CHECK(pf.getDenomPrimes().empty());

    pf = extraction;

    checkNum(3, 3, 3);
    checkDenom(2, 2, 2);
}

TEST(PrimeFac, emptyCount)
    /* Count of 1: 0. */
{
    int count;

    pf.set(1);
    count = pf.getEqualCount();

    CHECK_EQUAL(0, count);
}

TEST(PrimeFac, intCount)
    /* Count of (2^5*3^5): 5. */
{
    int count;

    pf.set(7776);
    count = pf.getEqualCount();

    CHECK_EQUAL(5, count);
}

TEST(PrimeFac, intCountNonEqual)
    /* Count of (2^3*3^3*7^2*11^3) : 0. */
{
    int count;

    pf.set(14087304);
    count = pf.getEqualCount();

    CHECK_EQUAL(0, count);
}

TEST(PrimeFac, fractionCount)
    /* Count of (2^4*3^4)/(7^4): 4. */
{
    int count;

    pf.set(Number(1296, 2401));
    count = pf.getEqualCount();

    CHECK_EQUAL(4, count);
}

TEST(PrimeFac, fractionCountNumeratorOne)
    /* 1/(2*2*3*3*5*5*7*7*11*11) : 2. */
{
    int count;

    pf.set(Number(1, 5336100));
    count = pf.getEqualCount();

    CHECK_EQUAL(2, count);
}

TEST(PrimeFac, fractionCountNonEqual)
    /* Count of (2*3*5*7)/(11*13*13) : 0. */
{
    int count;

    pf.set(Number(210, 1859));
    count = pf.getEqualCount();

    CHECK_EQUAL(0, count);
}

TEST(PrimeFac, intCollection)
    /* 25^(1/3) = 5^(2/3). */
{
    Number newExp;

    pf.set(25);
    newExp = pf.collectToNewExp(third);

    checkNum(5);
    CHECK(pf.getDenomPrimes().empty());

    CHECK_EQUAL(twoThird, newExp);
}

TEST(PrimeFac, intNoCollection)
    /* 18^(1/3) isn't altered. */
{
    Number newExp;

    pf.set(18);
    newExp = pf.collectToNewExp(third);

    checkNum(2, 3, 3);
    CHECK(pf.getDenomPrimes().empty());

    CHECK_EQUAL(third, newExp);
}

TEST(PrimeFac, simpleFractionCollection)
    /* 4^(2/3) = 2^(4/3). Results in an exponent greater than one (no extraction performed). */
{
    Number newExp;

    pf.set(4);
    newExp = pf.collectToNewExp(twoThird);

    checkNum(2);
    CHECK(pf.getDenomPrimes().empty());

    CHECK_EQUAL(Number(4, 3), newExp);
}

TEST(PrimeFac, fractionCollection)
    /* (8/27)^(5/6) = (2/3)^(15/6), same as above but with a fraction base. */
{
    Number newExp;

    pf.set(Number(8, 27));
    newExp = pf.collectToNewExp(Number(5, 6));

    checkNum(2);
    checkDenom(3);

    CHECK_EQUAL(Number(15, 6), newExp);
}

TEST(PrimeFac, emptyColletion)
    /* 1^(2/3) = 1^1. */
{
    Number newExp;

    pf.set(1);
    newExp = pf.collectToNewExp(Number(2, 3));

    CHECK(pf.getNumPrimes().empty());
    CHECK(pf.getDenomPrimes().empty());

    CHECK_EQUAL(1, newExp);
}

TEST(PrimeFac, evaluate)
{
    const Number n(10626, 3211);

    pf.set(n);

    CHECK_EQUAL(n, pf.eval());
}
