
#include "abc.h"
#include "product.h"
#include "sum.h"
#include "constant.h"
#include "power.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Has)
{
    const BasePtr undefined = Undefined::create();
    const BasePtr pi = Constant::createPi();
    const BasePtr twoPiAbProduct = Product::create(two, a, b, pi);
    const BasePtr onePlusATimesB = Sum::create(one, Product::create(a, b));
    const BasePtr aPlusBSquare = Power::create(Sum::create(a, b), two);
};

TEST(Has, equalSymbols)
{
    CHECK(a->has(a));
}

TEST(Has, differentSymbols)
{
    CHECK_FALSE(a->has(b));
}

TEST(Has, equalNumerics)
{
    CHECK(two->has(two));
}

TEST(Has, differentNumerics)
{
    CHECK_FALSE(two->has(three));
}

TEST(Has, equalConstant)
{
    CHECK(pi->has(pi));
}

TEST(Has, equalUndefined)
    /* Two Undefined are never equal, thus this query shall return false. */
{
    CHECK_FALSE(undefined->has(undefined));
}

TEST(Has, differentScalarTypes)
{
    CHECK_FALSE(a->has(two));
    CHECK_FALSE(a->has(pi));
    CHECK_FALSE(a->has(undefined));
    CHECK_FALSE(ten->has(b));
    CHECK_FALSE(ten->has(pi));
    CHECK_FALSE(ten->has(undefined));
    CHECK_FALSE(pi->has(a));
    CHECK_FALSE(pi->has(one));
    CHECK_FALSE(pi->has(undefined));
    CHECK_FALSE(undefined->has(a));
    CHECK_FALSE(undefined->has(ten));
    CHECK_FALSE(undefined->has(pi));
}

TEST(Has, equalProducts)
{
    CHECK(twoPiAbProduct->has(twoPiAbProduct));
}

TEST(Has, differentProducts)
{
    const BasePtr different = Product::create(d, e);

    CHECK_FALSE(twoPiAbProduct->has(different));
}

TEST(Has, subProductsMultipleFactors)
{
    const BasePtr sub = Product::create(a, b);

    CHECK_FALSE(twoPiAbProduct->has(sub));
}

TEST(Has, subProductsOneFactor)
{
    const BasePtr sub = Sum::create(a, b);
    const BasePtr product = Product::create(pi, sub);

    CHECK(product->has(sub));
}

TEST(Has, equalSums)
{
    onePlusATimesB->has(onePlusATimesB);
}

TEST(Has, differentSums)
{
    const BasePtr different = Sum::create(c, d);

    CHECK_FALSE(onePlusATimesB->has(different));
}

TEST(Has, subSumMultipleSummands)
{
    const BasePtr sub = Sum::create(a, b);
    const BasePtr sum = Sum::create(c, sub);

    CHECK_FALSE(sum->has(sub));
}

TEST(Has, subSumOneSummand)
{
    const BasePtr sub = Product::create(a, b);

    CHECK(onePlusATimesB->has(sub));
}

TEST(Has, equalPowers)
{
    CHECK(aPlusBSquare->has(aPlusBSquare));
}

TEST(Has, differentPowers)
{
    const BasePtr different = Power::create(pi, Product::create(a, b));

    CHECK_FALSE(aPlusBSquare->has(different));
}

TEST(Has, baseOfPower)
{
    const BasePtr base = Sum::create(a, b);

    CHECK(aPlusBSquare->has(base));
}

TEST(Has, exponentOfPower)
{
    CHECK(aPlusBSquare->has(two));
}

TEST(Has, equalFunctions)
{
    const BasePtr sinA = Trigonometric::createSin(a);

    CHECK(sinA->has(sinA));
}

TEST(Has, differentFunctions)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr cosB = Trigonometric::createCos(b);

    CHECK_FALSE(sinA->has(cosB));
}

TEST(Has, argOfFunction)
{
    const BasePtr arg = Sum::create(a, two, pi);
    const BasePtr fct = Trigonometric::createAsin(arg);

    CHECK(fct->has(two));
    CHECK(fct->has(pi));
    CHECK(fct->has(a));
    CHECK(fct->has(arg));
}
