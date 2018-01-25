
#include "abc.h"
#include "constant.h"
#include "trigonometric.h"
#include "sum.h"
#include "power.h"
#include "number.h"
#include "symbol.h"
#include "logarithm.h"
#include "product.h"
#include "tsymtests.h"
#include "int.h"

using namespace tsym;

TEST_GROUP(Hash)
{
    size_t hash(const BasePtr& ptr)
    {
        return std::hash<BasePtr>{}(ptr);
    }
};

TEST(Hash, smallAndLargeInts)
{
    const size_t largeN1 = std::hash<Int>{}(Int("987928309824092309420934809284309283049204"));
    const size_t largeN2 = std::hash<Int>{}(Int("734859834598374985739847598345374398475"));
    const size_t smallN1 = std::hash<Int>{}(Int(1234));
    const size_t smallN2 = std::hash<Int>{}(Int(12345));

    CHECK(smallN1 != smallN2);
    CHECK(smallN1 != largeN1);
    CHECK(smallN2 != largeN2);
    CHECK(largeN1 != largeN2);
}

TEST(Hash, number)
{
    const size_t n1 = std::hash<Number>{}(Number(2, 3));
    const size_t n2 = std::hash<Number>{}(Number(-10));
    const size_t n3 = std::hash<Number>{}(Number(1.23456789));

    CHECK(n1 != n2);
    CHECK(n2 != n3);
    CHECK(n1 != n3);
}

TEST(Hash, name)
{
    const size_t textualHash = std::hash<Name>{}(Name("a", "b", "c"));
    const size_t shortTextualHash = std::hash<Name>{}(Name("a"));
    const size_t numericHash = std::hash<Name>{}(Name("123"));

    CHECK(textualHash != numericHash);
    CHECK(textualHash != shortTextualHash);
    CHECK(numericHash != shortTextualHash);
}

TEST(Hash, constant)
{
    const size_t hashPi = hash(Constant::createPi());
    const size_t hashE = hash(Constant::createE());
    const size_t hashPiSymbol = hash(Symbol::create("Pi"));

    CHECK(hashPi != hashE);
    CHECK(hashPi != hashPiSymbol);
}

TEST(Hash, trigoAndLogSameArgument)
{
    const size_t hashTrig = hash(Trigonometric::createSin(a));
    const size_t hashLog = hash(Logarithm::create(a));

    CHECK(hashTrig != hashLog);
}

TEST(Hash, differentTrigFctSameArgument)
{
    const size_t hashSin = hash(Trigonometric::createSin(a));
    const size_t hashCos = hash(Trigonometric::createCos(a));

    CHECK(hashSin != hashCos);
}

TEST(Hash, sameTrigFctDifferentInstance)
{
    const size_t hashTrig1 = hash(Trigonometric::createAsin(a));
    const size_t hashTrig2 = hash(Trigonometric::createAsin(a));

    CHECK_EQUAL(hashTrig1, hashTrig2);
}

TEST(Hash, logarithDifferentArgument)
{
    const size_t hashLogA = hash(Logarithm::create(a));
    const size_t hashLogB = hash(Logarithm::create(b));

    CHECK(hashLogA != hashLogB);
}

TEST(Hash, sameSymbolDifferentInstance)
{
    const size_t hashA1 = hash(Symbol::create("a"));
    const size_t hashA2 = hash(a);

    CHECK_EQUAL(hashA1, hashA2);
}

TEST(Hash, sameSymbolNameOnePositive)
{
    const size_t hashPosA = hash(Symbol::createPositive("a"));
    const size_t hashA = hash(a);

    CHECK(hashA != hashPosA);
}

TEST(Hash, equalPowers)
{
    const size_t hashPow1 = hash(Power::create(a, b));
    const size_t hashPow2 = hash(Power::create(a, b));

    CHECK_EQUAL(hashPow1, hashPow2);
}

TEST(Hash, powerDifferentBase)
{
    const size_t hashPow1 = hash(Power::create(a, b));
    const size_t hashPow2 = hash(Power::create(b, b));

    CHECK(hashPow1 != hashPow2);
}

TEST(Hash, powerDifferentExp)
{
    const size_t hashPow1 = hash(Power::create(a, b));
    const size_t hashPow2 = hash(Power::create(a, c));

    CHECK(hashPow1 != hashPow2);
}

TEST(Hash, compositeTypesSameOperands)
{
    const size_t sumHash = hash(Sum::create(a, b));
    const size_t productHash = hash(Product::create(a, b));
    const size_t powerHash = hash(Power::create(a, b));

    CHECK(sumHash != productHash);
    CHECK(sumHash != powerHash);
    CHECK(productHash != powerHash);
}

TEST(Hash, differentSums)
{
    const size_t sumHash1 = hash(Sum::create(a, b));
    const size_t sumHash2 = hash(Sum::create(b, c));

    CHECK(sumHash1 != sumHash2);
}

TEST(Hash, equalSums)
{
    const size_t sumHash1 = hash(Sum::create(a, b, c));
    const size_t sumHash2 = hash(Sum::create(a, b, c));

    CHECK_EQUAL(sumHash1, sumHash2);
}

TEST(Hash, sumAndProductEqualOperands)
{
    const size_t sumHash = hash(Sum::create(a, b, c));
    const size_t productHash = hash(Product::create(a, b, c));

    CHECK_FALSE(sumHash == productHash);
}
