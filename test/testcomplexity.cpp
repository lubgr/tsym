
#include "constant.h"
#include "power.h"
#include "sum.h"
#include "numeric.h"
#include "product.h"
#include "trigonometric.h"
#include "undefined.h"
#include "logarithm.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestComplexity, AbcFixture)

BOOST_AUTO_TEST_CASE(trivialUndefined)
{
    BOOST_CHECK_EQUAL(0, undefined->complexity());
}

BOOST_AUTO_TEST_CASE(integer)
{
    unsigned res = three->complexity();

    BOOST_CHECK_EQUAL(1, res);
}

BOOST_AUTO_TEST_CASE(fraction)
{
    const BasePtr fraction = Numeric::create(2, 3);

    BOOST_CHECK_EQUAL(2, fraction->complexity());
}

BOOST_AUTO_TEST_CASE(floatingPoint)
{
    const BasePtr n = Numeric::create(2.7665454894445454);

    BOOST_CHECK_EQUAL(3, n->complexity());
}

BOOST_AUTO_TEST_CASE(constant)
{
    BOOST_CHECK_EQUAL(4, pi->complexity());
}

BOOST_AUTO_TEST_CASE(symbol)
{
    BOOST_CHECK_EQUAL(5, a->complexity());
}

BOOST_AUTO_TEST_CASE(sum)
{
    const BasePtr sum = Sum::create(three, a);

    BOOST_CHECK_EQUAL(5 + 1 + 5, sum->complexity());
}

BOOST_AUTO_TEST_CASE(product)
{
    const BasePtr product = Product::create(three, a, pi);

    BOOST_CHECK_EQUAL(5 + 1 + 5 + 4, product->complexity());
}

BOOST_AUTO_TEST_CASE(power)
{
    const BasePtr pow = Power::create(three, a);

    BOOST_CHECK_EQUAL(5 + 1 + 2*5, pow->complexity());
}

BOOST_AUTO_TEST_CASE(sinA)
{
    const BasePtr sinA = Trigonometric::createSin(a);

    BOOST_CHECK_EQUAL(6 + 5, sinA->complexity());
}

BOOST_AUTO_TEST_CASE(logarithmOfSymbol)
{
    const unsigned res = Logarithm::create(a)->complexity();

    BOOST_CHECK_EQUAL(6 + 5, res);
}

BOOST_AUTO_TEST_CASE(largerSum)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr product = Product::create(three, b, pi);
    const BasePtr doubleNum = Numeric::create(-10.20394820938409243);
    const BasePtr pow = Power::create(three, a);
    const BasePtr sum = Sum::create({ product, a, doubleNum, sinA, pow, Logarithm::create(a), pi});

    BOOST_CHECK_EQUAL(5 + 15 + 5 + 3 + 11 + 16 + 11 + 4, sum->complexity());
}

BOOST_AUTO_TEST_SUITE_END()
