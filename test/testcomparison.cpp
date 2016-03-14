
#include "cpputest.h"
#include "abc.h"
#include "constant.h"
#include "symbol.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "trigonometric.h"
#include "undefined.h"

using namespace tsym;

TEST_GROUP(Comparison)
{
    BasePtr undefined;
};

TEST(Comparison, differentTypes)
{
    two->isDifferent(a);
    two->isDifferent(undefined);
    undefined->isDifferent(a);
}

TEST(Comparison, equalSymbols)
{
    CHECK(a->isEqual(a));
}

TEST(Comparison, equalConstants)
{
    const BasePtr pi = Constant::createPi();

    CHECK(pi->isEqual(pi));
}

TEST(Comparison, differentSymbols)
{
    CHECK(a->isDifferent(b));
}

TEST(Comparison, equalNumerics)
{
    CHECK(two->isEqual(two));
}

TEST(Comparison, differentNumerics)
{
    CHECK(two->isDifferent(Numeric::create(1, 2)));
}

TEST(Comparison, twoUndefined)
    /* Comparing undefined  doesn't make sense, this shall always return false. */
{
    disableLog();
    CHECK(!undefined->isEqual(undefined));
    enableLog();
}

TEST(Comparison, equalPowers)
{
    const BasePtr p1 = Power::create(a, two);
    const BasePtr p2 = Power::create(a, two);

    CHECK(p1->isEqual(p2));
}

TEST(Comparison, differentExpPowers)
{
    const BasePtr p1 = Power::create(a, two);
    const BasePtr p2 = Power::sqrt(a);

    CHECK(p1->isDifferent(p2));
}

TEST(Comparison, diffBasePowers)
{
    const BasePtr p1 = Power::sqrt(a);
    const BasePtr p2 = Power::sqrt(b);

    CHECK(p1->isDifferent(p2));
}

TEST(Comparison, equalProduct)
{
    const BasePtr p1 = Product::create(two, a);
    const BasePtr p2 = Product::create(two, a);

    CHECK(p1->isEqual(p2));
}

TEST(Comparison, differentProduct)
{
    const BasePtr p1 = Product::create(two, a);
    const BasePtr p2 = Product::create(two, b);

    CHECK(p1->isDifferent(p2));
}

TEST(Comparison, differentProductByNumberOfFactors)
{
    const BasePtr p1 = Product::create(a, b);
    const BasePtr p2 = Product::create(a, b, c);

    CHECK(p1->isDifferent(p2));
}

TEST(Comparison, equalSum)
{
    const BasePtr s1 = Sum::create(a, b);
    const BasePtr s2 = Sum::create(a, b);

    CHECK(s1->isEqual(s2));
}

TEST(Comparison, differentSum)
{
    const BasePtr s1 = Sum::create(two, a);
    const BasePtr s2 = Sum::create(two, b);

    CHECK(s1->isDifferent(s2));
}

TEST(Comparison, equalProductOfSymbolAndPower)
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr product = Product::create(a, bSquare);
    const BasePtr res = Product::create(product, product);

    CHECK(res->isEqual(res));
}

TEST(Comparison, equalFunctions)
{
    const BasePtr sin = Trigonometric::createSin(Product::create(a, b));

    CHECK(sin->isEqual(sin));
}

TEST(Comparison, functionsDifferentArg)
{
    const BasePtr sin1 = Trigonometric::createSin(a);
    const BasePtr sin2 = Trigonometric::createSin(b);

    CHECK(sin1->isDifferent(sin2));
}

TEST(Comparison, functionsDifferentTrigonometric)
{
    const BasePtr sin = Trigonometric::createSin(Sum::create(a, b));
    const BasePtr cos = Trigonometric::createCos(Sum::create(a, b));

    CHECK(sin->isDifferent(cos));
}

TEST(Comparison, equalLists)
{
    BasePtrList list;

    list.push_back(ten);
    list.push_back(a);
    list.push_back(Product::create(two, b));
    list.push_back(Sum::create(three, c));

    CHECK(list.isEqual(list));
}

TEST(Comparison, differentLists)
{
    BasePtrList l1;
    BasePtrList l2;

    l1.push_back(three);
    l1.push_back(four);
    l1.push_back(a);

    l2 = l1;

    l1.push_back(Trigonometric::createSin(a));
    l2.push_back(Product::create(b, c));

    CHECK(l1.isDifferent(l2));
    CHECK(l1.isDifferent(l2));
}
