
#include "bplist.h"
#include "constant.h"
#include "fixtures.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestComparison, AbcFixture)

BOOST_AUTO_TEST_CASE(differentTypes)
{
    two->isDifferent(a);
    two->isDifferent(undefined);
    undefined->isDifferent(a);
}

BOOST_AUTO_TEST_CASE(equalSymbols)
{
    BOOST_TEST(a->isEqual(a));
}

BOOST_AUTO_TEST_CASE(symbolsEqualNameOnePositive)
{
    const BasePtr aPos = Symbol::createPositive("a");

    BOOST_TEST(!a->isEqual(aPos));
}

BOOST_AUTO_TEST_CASE(equalConstants)
{
    BOOST_TEST(pi->isEqual(pi));
}

BOOST_AUTO_TEST_CASE(differentSymbols)
{
    BOOST_TEST(a->isDifferent(b));
}

BOOST_AUTO_TEST_CASE(equalNumerics)
{
    BOOST_TEST(two->isEqual(two));
}

BOOST_AUTO_TEST_CASE(differentNumerics)
{
    BOOST_TEST(two->isDifferent(Numeric::half()));
}

BOOST_AUTO_TEST_CASE(twoUndefined, noLogs())
/* Comparing undefined  doesn't make sense, this shall always return false. */
{
    BOOST_TEST(!undefined->isEqual(undefined));
}

BOOST_AUTO_TEST_CASE(equalPowers)
{
    const BasePtr p1 = Power::create(a, two);
    const BasePtr p2 = Power::create(a, two);

    BOOST_TEST(p1->isEqual(p2));
}

BOOST_AUTO_TEST_CASE(differentExpPowers)
{
    const BasePtr p1 = Power::create(a, two);
    const BasePtr p2 = Power::sqrt(a);

    BOOST_TEST(p1->isDifferent(p2));
}

BOOST_AUTO_TEST_CASE(diffBasePowers)
{
    const BasePtr p1 = Power::sqrt(a);
    const BasePtr p2 = Power::sqrt(b);

    BOOST_TEST(p1->isDifferent(p2));
}

BOOST_AUTO_TEST_CASE(equalProduct)
{
    const BasePtr p1 = Product::create(two, a);
    const BasePtr p2 = Product::create(two, a);

    BOOST_TEST(p1->isEqual(p2));
}

BOOST_AUTO_TEST_CASE(differentProduct)
{
    const BasePtr p1 = Product::create(two, a);
    const BasePtr p2 = Product::create(two, b);

    BOOST_TEST(p1->isDifferent(p2));
}

BOOST_AUTO_TEST_CASE(differentProductByNumberOfFactors)
{
    const BasePtr p1 = Product::create(a, b);
    const BasePtr p2 = Product::create(a, b, c);

    BOOST_TEST(p1->isDifferent(p2));
}

BOOST_AUTO_TEST_CASE(equalSum)
{
    const BasePtr s1 = Sum::create(a, b);
    const BasePtr s2 = Sum::create(a, b);

    BOOST_TEST(s1->isEqual(s2));
}

BOOST_AUTO_TEST_CASE(differentSum)
{
    const BasePtr s1 = Sum::create(two, a);
    const BasePtr s2 = Sum::create(two, b);

    BOOST_TEST(s1->isDifferent(s2));
}

BOOST_AUTO_TEST_CASE(equalProductOfSymbolAndPower)
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr product = Product::create(a, bSquare);
    const BasePtr res = Product::create(product, product);

    BOOST_TEST(res->isEqual(res));
}

BOOST_AUTO_TEST_CASE(equalFunctions)
{
    const BasePtr sin = Trigonometric::createSin(Product::create(a, b));

    BOOST_TEST(sin->isEqual(sin));
}

BOOST_AUTO_TEST_CASE(functionsDifferentArg)
{
    const BasePtr sin1 = Trigonometric::createSin(a);
    const BasePtr sin2 = Trigonometric::createSin(b);

    BOOST_TEST(sin1->isDifferent(sin2));
}

BOOST_AUTO_TEST_CASE(functionsDifferentTrigonometric)
{
    const BasePtr sin = Trigonometric::createSin(Sum::create(a, b));
    const BasePtr cos = Trigonometric::createCos(Sum::create(a, b));

    BOOST_TEST(sin->isDifferent(cos));
}

BOOST_AUTO_TEST_CASE(equalContainer)
{
    const BasePtrList container{ten, a, Product::create(two, b), Sum::create(three, c)};

    BOOST_TEST(bplist::areEqual(container, container));
}

BOOST_AUTO_TEST_CASE(differentContainer)
{
    BasePtrList c1{three, four, a};
    BasePtrList c2;

    c2 = c1;

    c1.push_back(Trigonometric::createSin(a));
    c2.push_back(Product::create(b, c));

    BOOST_TEST(!bplist::areEqual(c1, c2));
    BOOST_TEST(!bplist::areEqual(c2, c1));
}

BOOST_AUTO_TEST_SUITE_END()
