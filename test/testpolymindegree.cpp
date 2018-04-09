
#include "abc.h"
#include "poly.h"
#include "sum.h"
#include "product.h"
#include "numeric.h"
#include "trigonometric.h"
#include "power.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_AUTO_TEST_SUITE(TestPolyMinDegree)

BOOST_AUTO_TEST_CASE(minDegreeMatchingSymbol)
{
    BOOST_CHECK_EQUAL(1, poly::minDegree(a, a));
}

BOOST_AUTO_TEST_CASE(minDegreeDifferentSymbol)
{
    BOOST_CHECK_EQUAL(0, poly::minDegree(a, b));
}

BOOST_AUTO_TEST_CASE(minDegreeInteger)
{
    BOOST_CHECK_EQUAL(0, poly::minDegree(two, a));

    disableLog();
    BOOST_CHECK_EQUAL(0, poly::minDegree(two, two));
    enableLog();
}

BOOST_AUTO_TEST_CASE(minDegreePowerOneSymbolInBase)
    /* minDegree((2*a + a^2)^4) for a: 4. */
{
    const BasePtr base = Sum::create(Product::create(two, a), Power::create(a, two));
    const BasePtr pow = Power::create(base, four);

    BOOST_CHECK_EQUAL(4, poly::minDegree(pow, a));
}

BOOST_AUTO_TEST_CASE(minDegreeNestedPower)
    /* minDegree((a^2 + a^3)^4) for a: 8. */
{
    const BasePtr base = Sum::create(Power::create(a, two), Power::create(a, three));
    const BasePtr pow = Power::create(base, four);

    BOOST_CHECK_EQUAL(8, poly::minDegree(pow, a));
}

BOOST_AUTO_TEST_CASE(minDegreePowerOtherSymbolInBase)
    /* minDegree((2*a + b)^4) for a: 0. */
{
    const BasePtr base = Sum::create(Product::create(two, a), b);
    const BasePtr pow = Power::create(base, four);

    BOOST_CHECK_EQUAL(0, poly::minDegree(pow, a));
}

BOOST_AUTO_TEST_CASE(minDegreeSum)
    /* minDegree(a + a^2 + a^5) for a: 1. */
{
    const BasePtr sum = Sum::create(a, Power::create(a, two), Power::create(a, five));

    BOOST_CHECK_EQUAL(1, poly::minDegree(sum, a));
}

BOOST_AUTO_TEST_CASE(minDegreeSumOneIntegerSummand)
    /* minDegree(a + 2) for a: 0. */
{
    const BasePtr sum = Sum::create(a, two);

    BOOST_CHECK_EQUAL(0, poly::minDegree(sum, a));
}

BOOST_AUTO_TEST_CASE(minDegreeSumOneSummandIndependent)
    /* minDegree(a^2 + b) for a: 0. */
{
    const BasePtr sum = Sum::create(Power::create(a, two), b);

    BOOST_CHECK_EQUAL(0, poly::minDegree(sum, a));
}

BOOST_AUTO_TEST_CASE(minDegreeProduct)
    /* minDegree(a*b*(a + 2)) for a: 1. */
{
    const BasePtr product = Product::create(a, b, Sum::create(a, two));

    BOOST_CHECK_EQUAL(1, poly::minDegree(product, a));
}

BOOST_AUTO_TEST_CASE(minDegreeProductNoMatchingSymbol)
    /* minDegree(a*b*c) for d: 0. */
{
    const BasePtr product = Product::create(a, b, c);

    BOOST_CHECK_EQUAL(0, poly::minDegree(product, d));
}

BOOST_AUTO_TEST_CASE(minDegreeInvalidInput)
    /* Shall return 0. */
{
    const BasePtr arg = Trigonometric::createSin(a);
    int result;

    result = poly::minDegree(arg, a);

    BOOST_CHECK_EQUAL(0, result);
}

BOOST_AUTO_TEST_CASE(minDegreeLargeExp)
{
    const Int largeExp("1000000000000000000000000000000000000000");
    const BasePtr arg = Power::create(a, Numeric::create(largeExp));
    int result;

    disableLog();
    result = poly::minDegree(arg, a);
    enableLog();

    BOOST_CHECK_EQUAL(0, result);
}

BOOST_AUTO_TEST_SUITE_END()
