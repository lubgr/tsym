
#include "abc.h"
#include "poly.h"
#include "sum.h"
#include "product.h"
#include "trigonometric.h"
#include "power.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(PolyMinDegree) {};

TEST(PolyMinDegree, minDegreeMatchingSymbol)
{
    CHECK_EQUAL(1, poly::minDegree(a, a));
}

TEST(PolyMinDegree, minDegreeDifferentSymbol)
{
    CHECK_EQUAL(0, poly::minDegree(a, b));
}

TEST(PolyMinDegree, minDegreeInteger)
{
    CHECK_EQUAL(0, poly::minDegree(two, a));

    disableLog();
    CHECK_EQUAL(0, poly::minDegree(two, two));
    enableLog();
}

TEST(PolyMinDegree, minDegreePowerOneSymbolInBase)
    /* minDegree((2*a + a^2)^4) for a: 4. */
{
    const BasePtr base = Sum::create(Product::create(two, a), Power::create(a, two));
    const BasePtr pow = Power::create(base, four);

    CHECK_EQUAL(4, poly::minDegree(pow, a));
}

TEST(PolyMinDegree, minDegreeNestedPower)
    /* minDegree((a^2 + a^3)^4) for a: 8. */
{
    const BasePtr base = Sum::create(Power::create(a, two), Power::create(a, three));
    const BasePtr pow = Power::create(base, four);

    CHECK_EQUAL(8, poly::minDegree(pow, a));
}

TEST(PolyMinDegree, minDegreePowerOtherSymbolInBase)
    /* minDegree((2*a + b)^4) for a: 0. */
{
    const BasePtr base = Sum::create(Product::create(two, a), b);
    const BasePtr pow = Power::create(base, four);

    CHECK_EQUAL(0, poly::minDegree(pow, a));
}

TEST(PolyMinDegree, minDegreeSum)
    /* minDegree(a + a^2 + a^5) for a: 1. */
{
    const BasePtr sum = Sum::create(a, Power::create(a, two), Power::create(a, five));

    CHECK_EQUAL(1, poly::minDegree(sum, a));
}

TEST(PolyMinDegree, minDegreeSumOneIntegerSummand)
    /* minDegree(a + 2) for a: 0. */
{
    const BasePtr sum = Sum::create(a, two);

    CHECK_EQUAL(0, poly::minDegree(sum, a));
}

TEST(PolyMinDegree, minDegreeSumOneSummandIndependent)
    /* minDegree(a^2 + b) for a: 0. */
{
    const BasePtr sum = Sum::create(Power::create(a, two), b);

    CHECK_EQUAL(0, poly::minDegree(sum, a));
}

TEST(PolyMinDegree, minDegreeProduct)
    /* minDegree(a*b*(a + 2)) for a: 1. */
{
    const BasePtr product = Product::create(a, b, Sum::create(a, two));

    CHECK_EQUAL(1, poly::minDegree(product, a));
}

TEST(PolyMinDegree, minDegreeProductNoMatchingSymbol)
    /* minDegree(a*b*c) for d: 0. */
{
    const BasePtr product = Product::create(a, b, c);

    CHECK_EQUAL(0, poly::minDegree(product, d));
}

TEST(PolyMinDegree, minDegreeInvalidInput)
    /* Shall return 0. */
{
    const BasePtr arg = Trigonometric::createSin(a);
    int result;

    result = poly::minDegree(arg, a);

    CHECK_EQUAL(0, result);
}

TEST(PolyMinDegree, minDegreeLargeExp)
{
    const Int largeExp("1000000000000000000000000000000000000000");
    const BasePtr arg = Power::create(a, Numeric::create(largeExp));
    int result;

    disableLog();
    result = poly::minDegree(arg, a);
    enableLog();

    CHECK_EQUAL(0, result);
}
