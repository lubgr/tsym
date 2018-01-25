
#include "abc.h"
#include "constant.h"
#include "power.h"
#include "sum.h"
#include "numeric.h"
#include "product.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"
#include "logarithm.h"

using namespace tsym;

TEST_GROUP(Complexity)
{
    const BasePtr pi = Constant::createPi();
};

TEST(Complexity, undefined)
{
    const BasePtr undefined = Undefined::create();

    CHECK_EQUAL(0, undefined->complexity());
}

TEST(Complexity, three)
{
    unsigned res = three->complexity();

    CHECK_EQUAL(1, res);
}

TEST(Complexity, fraction)
{
    const BasePtr fraction = Numeric::create(2, 3);

    CHECK_EQUAL(2, fraction->complexity());
}

TEST(Complexity, floatingPoint)
{
    const BasePtr n = Numeric::create(2.7665454894445454);

    CHECK_EQUAL(3, n->complexity());
}

TEST(Complexity, constant)
{
    CHECK_EQUAL(4, pi->complexity());
}

TEST(Complexity, symbol)
{
    CHECK_EQUAL(5, a->complexity());
}

TEST(Complexity, sum)
{
    const BasePtr sum = Sum::create(three, a);

    CHECK_EQUAL(5 + 1 + 5, sum->complexity());
}

TEST(Complexity, product)
{
    const BasePtr product = Product::create(three, a, pi);

    CHECK_EQUAL(5 + 1 + 5 + 4, product->complexity());
}

TEST(Complexity, power)
{
    const BasePtr pow = Power::create(three, a);

    CHECK_EQUAL(5 + 1 + 2*5, pow->complexity());
}

TEST(Complexity, sinA)
{
    const BasePtr sinA = Trigonometric::createSin(a);

    CHECK_EQUAL(6 + 5, sinA->complexity());
}

TEST(Complexity, logarithmOfSymbol)
{
    const unsigned res = Logarithm::create(a)->complexity();

    CHECK_EQUAL(6 + 5, res);
}

TEST(Complexity, largerSum)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr product = Product::create(three, b, pi);
    const BasePtr doubleNum = Numeric::create(-10.20394820938409243);
    const BasePtr pow = Power::create(three, a);
    const BasePtr sum = Sum::create({ product, a, doubleNum, sinA, pow, Logarithm::create(a), pi});

    CHECK_EQUAL(5 + 15 + 5 + 3 + 11 + 16 + 11 + 4, sum->complexity());
}
