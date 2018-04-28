
#include "constant.h"
#include "fixtures.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct HasFixture : public AbcFixture {
    const BasePtr twoPiAbProduct = Product::create(two, a, b, pi);
    const BasePtr onePlusATimesB = Sum::create(one, Product::create(a, b));
    const BasePtr aPlusBSquare = Power::create(Sum::create(a, b), two);
};

BOOST_FIXTURE_TEST_SUITE(TestHas, HasFixture)

BOOST_AUTO_TEST_CASE(equalSymbols)
{
    BOOST_TEST(a->has(a));
}

BOOST_AUTO_TEST_CASE(differentSymbols)
{
    BOOST_TEST(!a->has(b));
}

BOOST_AUTO_TEST_CASE(equalNumerics)
{
    BOOST_TEST(two->has(two));
}

BOOST_AUTO_TEST_CASE(differentNumerics)
{
    BOOST_TEST(!two->has(three));
}

BOOST_AUTO_TEST_CASE(equalConstant)
{
    BOOST_TEST(pi->has(pi));
}

BOOST_AUTO_TEST_CASE(equalUndefined)
/* Two Undefined are never equal, thus this query shall return false. */
{
    BOOST_TEST(!undefined->has(undefined));
}

BOOST_AUTO_TEST_CASE(differentScalarTypes)
{
    BOOST_TEST(!a->has(two));
    BOOST_TEST(!a->has(pi));
    BOOST_TEST(!a->has(undefined));
    BOOST_TEST(!ten->has(b));
    BOOST_TEST(!ten->has(pi));
    BOOST_TEST(!ten->has(undefined));
    BOOST_TEST(!pi->has(a));
    BOOST_TEST(!pi->has(one));
    BOOST_TEST(!pi->has(undefined));
    BOOST_TEST(!undefined->has(a));
    BOOST_TEST(!undefined->has(ten));
    BOOST_TEST(!undefined->has(pi));
}

BOOST_AUTO_TEST_CASE(equalProducts)
{
    BOOST_TEST(twoPiAbProduct->has(twoPiAbProduct));
}

BOOST_AUTO_TEST_CASE(differentProducts)
{
    const BasePtr different = Product::create(d, e);

    BOOST_TEST(!twoPiAbProduct->has(different));
}

BOOST_AUTO_TEST_CASE(subProductsMultipleFactors)
{
    const BasePtr sub = Product::create(a, b);

    BOOST_TEST(!twoPiAbProduct->has(sub));
}

BOOST_AUTO_TEST_CASE(subProductsOneFactor)
{
    const BasePtr sub = Sum::create(a, b);
    const BasePtr product = Product::create(pi, sub);

    BOOST_TEST(product->has(sub));
}

BOOST_AUTO_TEST_CASE(equalSums)
{
    onePlusATimesB->has(onePlusATimesB);
}

BOOST_AUTO_TEST_CASE(differentSums)
{
    const BasePtr different = Sum::create(c, d);

    BOOST_TEST(!onePlusATimesB->has(different));
}

BOOST_AUTO_TEST_CASE(subSumMultipleSummands)
{
    const BasePtr sub = Sum::create(a, b);
    const BasePtr sum = Sum::create(c, sub);

    BOOST_TEST(!sum->has(sub));
}

BOOST_AUTO_TEST_CASE(subSumOneSummand)
{
    const BasePtr sub = Product::create(a, b);

    BOOST_TEST(onePlusATimesB->has(sub));
}

BOOST_AUTO_TEST_CASE(equalPowers)
{
    BOOST_TEST(aPlusBSquare->has(aPlusBSquare));
}

BOOST_AUTO_TEST_CASE(differentPowers)
{
    const BasePtr different = Power::create(pi, Product::create(a, b));

    BOOST_TEST(!aPlusBSquare->has(different));
}

BOOST_AUTO_TEST_CASE(baseOfPower)
{
    const BasePtr base = Sum::create(a, b);

    BOOST_TEST(aPlusBSquare->has(base));
}

BOOST_AUTO_TEST_CASE(exponentOfPower)
{
    BOOST_TEST(aPlusBSquare->has(two));
}

BOOST_AUTO_TEST_CASE(equalFunctions)
{
    const BasePtr sinA = Trigonometric::createSin(a);

    BOOST_TEST(sinA->has(sinA));
}

BOOST_AUTO_TEST_CASE(differentFunctions)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr cosB = Trigonometric::createCos(b);

    BOOST_TEST(!sinA->has(cosB));
}

BOOST_AUTO_TEST_CASE(argOfFunction)
{
    const BasePtr arg = Sum::create(a, two, pi);
    const BasePtr fct = Trigonometric::createAsin(arg);

    BOOST_TEST(fct->has(two));
    BOOST_TEST(fct->has(pi));
    BOOST_TEST(fct->has(a));
    BOOST_TEST(fct->has(arg));
}

BOOST_AUTO_TEST_SUITE_END()
