
#include "constant.h"
#include "fixtures.h"
#include "int.h"
#include "logarithm.h"
#include "number.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestHash, AbcFixture)

namespace {
    size_t hash(const BasePtr& ptr)
    {
        return std::hash<BasePtr>{}(ptr);
    }
}

BOOST_AUTO_TEST_CASE(smallAndLargeInts)
{
    const size_t largeN1 = std::hash<Int>{}(Int("987928309824092309420934809284309283049204"));
    const size_t largeN2 = std::hash<Int>{}(Int("734859834598374985739847598345374398475"));
    const size_t smallN1 = std::hash<Int>{}(Int(1234));
    const size_t smallN2 = std::hash<Int>{}(Int(12345));

    BOOST_TEST(smallN1 != smallN2);
    BOOST_TEST(smallN1 != largeN1);
    BOOST_TEST(smallN2 != largeN2);
    BOOST_TEST(largeN1 != largeN2);
}

BOOST_AUTO_TEST_CASE(number)
{
    const size_t n1 = std::hash<Number>{}(Number(2, 3));
    const size_t n2 = std::hash<Number>{}(Number(-10));
    const size_t n3 = std::hash<Number>{}(Number(1.23456789));

    BOOST_TEST(n1 != n2);
    BOOST_TEST(n2 != n3);
    BOOST_TEST(n1 != n3);
}

BOOST_AUTO_TEST_CASE(name)
{
    const size_t textualHash = std::hash<Name>{}(Name("a", "b", "c"));
    const size_t shortTextualHash = std::hash<Name>{}(Name("a"));
    const size_t numericHash = std::hash<Name>{}(Name("123"));

    BOOST_TEST(textualHash != numericHash);
    BOOST_TEST(textualHash != shortTextualHash);
    BOOST_TEST(numericHash != shortTextualHash);
}

BOOST_AUTO_TEST_CASE(constant)
{
    const size_t hashPi = hash(pi);
    const size_t hashE = hash(Constant::createE());
    const size_t hashPiSymbol = hash(Symbol::create("Pi"));

    BOOST_TEST(hashPi != hashE);
    BOOST_TEST(hashPi != hashPiSymbol);
}

BOOST_AUTO_TEST_CASE(trigoAndLogSameArgument)
{
    const size_t hashTrig = hash(Trigonometric::createSin(a));
    const size_t hashLog = hash(Logarithm::create(a));

    BOOST_TEST(hashTrig != hashLog);
}

BOOST_AUTO_TEST_CASE(differentTrigFctSameArgument)
{
    const size_t hashSin = hash(Trigonometric::createSin(a));
    const size_t hashCos = hash(Trigonometric::createCos(a));

    BOOST_TEST(hashSin != hashCos);
}

BOOST_AUTO_TEST_CASE(sameTrigFctDifferentInstance)
{
    const size_t hashTrig1 = hash(Trigonometric::createAsin(a));
    const size_t hashTrig2 = hash(Trigonometric::createAsin(a));

    BOOST_CHECK_EQUAL(hashTrig1, hashTrig2);
}

BOOST_AUTO_TEST_CASE(logarithDifferentArgument)
{
    const size_t hashLogA = hash(Logarithm::create(a));
    const size_t hashLogB = hash(Logarithm::create(b));

    BOOST_TEST(hashLogA != hashLogB);
}

BOOST_AUTO_TEST_CASE(sameSymbolDifferentInstance)
{
    const size_t hashA1 = hash(Symbol::create("a"));
    const size_t hashA2 = hash(a);

    BOOST_CHECK_EQUAL(hashA1, hashA2);
}

BOOST_AUTO_TEST_CASE(sameSymbolNameOnePositive)
{
    const size_t hashPosA = hash(Symbol::createPositive("a"));
    const size_t hashA = hash(a);

    BOOST_TEST(hashA != hashPosA);
}

BOOST_AUTO_TEST_CASE(equalPowers)
{
    const size_t hashPow1 = hash(Power::create(a, b));
    const size_t hashPow2 = hash(Power::create(a, b));

    BOOST_CHECK_EQUAL(hashPow1, hashPow2);
}

BOOST_AUTO_TEST_CASE(powerDifferentBase)
{
    const size_t hashPow1 = hash(Power::create(a, b));
    const size_t hashPow2 = hash(Power::create(b, b));

    BOOST_TEST(hashPow1 != hashPow2);
}

BOOST_AUTO_TEST_CASE(powerDifferentExp)
{
    const size_t hashPow1 = hash(Power::create(a, b));
    const size_t hashPow2 = hash(Power::create(a, c));

    BOOST_TEST(hashPow1 != hashPow2);
}

BOOST_AUTO_TEST_CASE(compositeTypesSameOperands)
{
    const size_t sumHash = hash(Sum::create(a, b));
    const size_t productHash = hash(Product::create(a, b));
    const size_t powerHash = hash(Power::create(a, b));

    BOOST_TEST(sumHash != productHash);
    BOOST_TEST(sumHash != powerHash);
    BOOST_TEST(productHash != powerHash);
}

BOOST_AUTO_TEST_CASE(differentSums)
{
    const size_t sumHash1 = hash(Sum::create(a, b));
    const size_t sumHash2 = hash(Sum::create(b, c));

    BOOST_TEST(sumHash1 != sumHash2);
}

BOOST_AUTO_TEST_CASE(equalSums)
{
    const size_t sumHash1 = hash(Sum::create(a, b, c));
    const size_t sumHash2 = hash(Sum::create(a, b, c));

    BOOST_CHECK_EQUAL(sumHash1, sumHash2);
}

BOOST_AUTO_TEST_CASE(sumAndProductEqualOperands)
{
    const size_t sumHash = hash(Sum::create(a, b, c));
    const size_t productHash = hash(Product::create(a, b, c));

    BOOST_TEST(sumHash != productHash);
}

BOOST_AUTO_TEST_SUITE_END()
