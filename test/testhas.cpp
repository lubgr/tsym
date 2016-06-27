
#include "abc.h"
#include "product.h"
#include "sum.h"
#include "constant.h"
#include "power.h"
#include "trigonometric.h"
#include "cpputest.h"

using namespace tsym;

TEST_GROUP(Has)
{
    BasePtr undefined;
    BasePtr pi;
    BasePtr twoPiAbProduct;
    BasePtr onePlusATimesB;
    BasePtr aPlusBSquare;

    void setup()
    {
        undefined = Undefined::create();
        pi = Constant::createPi();
        twoPiAbProduct = Product::create(two, a, b, pi);
        onePlusATimesB = Sum::create(one, Product::create(a, b));
        aPlusBSquare = Power::create(Sum::create(a, b), two);
    }
};

TEST(Has, equalSymbols)
{
    CHECK(a->has(a));
}

TEST(Has, differentSymbols)
{
    CHECK(!a->has(b));
}

TEST(Has, equalNumerics)
{
    CHECK(two->has(two));
}

TEST(Has, differentNumerics)
{
    CHECK(!two->has(three));
}

TEST(Has, equalConstant)
{
    CHECK(pi->has(pi));
}

TEST(Has, equalUndefined)
    /* Two Undefined are never equal, thus this query shall return false. */
{
    CHECK(!undefined->has(undefined));
}

TEST(Has, differentScalarTypes)
{
    CHECK(!a->has(two));
    CHECK(!a->has(pi));
    CHECK(!a->has(undefined));
    CHECK(!ten->has(b));
    CHECK(!ten->has(pi));
    CHECK(!ten->has(undefined));
    CHECK(!pi->has(a));
    CHECK(!pi->has(one));
    CHECK(!pi->has(undefined));
    CHECK(!undefined->has(a));
    CHECK(!undefined->has(ten));
    CHECK(!undefined->has(pi));
}

TEST(Has, equalProducts)
{
    CHECK(twoPiAbProduct->has(twoPiAbProduct));
}

TEST(Has, differentProducts)
{
    const BasePtr different = Product::create(d, e);

    CHECK(!twoPiAbProduct->has(different));
}

TEST(Has, subProductsMultipleFactors)
{
    const BasePtr sub = Product::create(a, b);

    CHECK(!twoPiAbProduct->has(sub));
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

    CHECK(!onePlusATimesB->has(different));
}

TEST(Has, subSumMultipleSummands)
{
    const BasePtr sub = Sum::create(a, b);
    const BasePtr sum = Sum::create(c, sub);

    CHECK(!sum->has(sub));
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

    CHECK(!aPlusBSquare->has(different));
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

    CHECK(!sinA->has(cosB));
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
