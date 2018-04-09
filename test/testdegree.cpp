
#include <limits>
#include "abc.h"
#include "sum.h"
#include "trigonometric.h"
#include "undefined.h"
#include "product.h"
#include "constant.h"
#include "numeric.h"
#include "power.h"
#include "tsymtests.h"

using namespace tsym;

struct DegreeFixture {
    const BasePtr abSum = Sum::create(a, b);
    const BasePtr abProduct = Product::create(a, b);
};

BOOST_FIXTURE_TEST_SUITE(TestDegree, DegreeFixture)

/* All types other than Numeric should return 1 for a degree request with the variable being equal
 * to the BasePtr object. This is similiar to Mathematica, but not GiNaC. */
BOOST_AUTO_TEST_CASE(equalSymbol)
{
    BOOST_CHECK_EQUAL(1, a->degree(a));
}

BOOST_AUTO_TEST_CASE(equalFunction)
{
    const BasePtr sinA = Trigonometric::createSin(a);

    BOOST_CHECK_EQUAL(1, sinA->degree(sinA));
}

BOOST_AUTO_TEST_CASE(equalConstant)
{
    const BasePtr pi = Constant::createPi();

    BOOST_CHECK_EQUAL(1, pi->degree(pi));
}

BOOST_AUTO_TEST_CASE(equalSum)
{
    const BasePtr sum = Sum::create(ten, a);

    BOOST_CHECK_EQUAL(1, sum->degree(sum));
}

BOOST_AUTO_TEST_CASE(equalProduct)
{
    BOOST_CHECK_EQUAL(1, abProduct->degree(abProduct));
}

BOOST_AUTO_TEST_CASE(equalPower)
{
    const BasePtr pow = Power::create(a, b);

    BOOST_CHECK_EQUAL(1, pow->degree(pow));
}

BOOST_AUTO_TEST_CASE(equalNumeric)
{
    BOOST_CHECK_EQUAL(0, four->degree(four));
}

BOOST_AUTO_TEST_CASE(equalUndefined)
    /* Two Undefined are never equal, so querying the coefficient will return 0. */
{
    const BasePtr undefined = Undefined::create();

    BOOST_CHECK_EQUAL(0, undefined->degree(undefined));
}

/* Equal behavior for all types: If the argument is completeley different (i.e., no subexpression of
 * the BasePtr object, degree shall always return 0. */
BOOST_AUTO_TEST_CASE(differentSymbols)
{
    BOOST_CHECK_EQUAL(0, a->degree(b));
}

BOOST_AUTO_TEST_CASE(symbolDifferentType)
{
    BOOST_CHECK_EQUAL(0, a->degree(abSum));
}

BOOST_AUTO_TEST_CASE(differentNumerics)
{
    BOOST_CHECK_EQUAL(0, four->degree(five));
}

BOOST_AUTO_TEST_CASE(numericSymbol)
{
    BOOST_CHECK_EQUAL(0, four->degree(a));
}

BOOST_AUTO_TEST_CASE(constantSymbol)
{
    const BasePtr pi = Constant::createPi();

    BOOST_CHECK_EQUAL(0, pi->degree(a));
}

BOOST_AUTO_TEST_CASE(functionSum)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr sum = Sum::create(one, b);

    BOOST_CHECK_EQUAL(0, sinA->degree(sum));
}

BOOST_AUTO_TEST_CASE(undefinedSymbol)
{
    const BasePtr undefined = Undefined::create();

    BOOST_CHECK_EQUAL(0, undefined->degree(a));
}

BOOST_AUTO_TEST_CASE(differentPowers)
{
    const BasePtr pow1 = Power::create(a, b);
    const BasePtr pow2 = Power::create(c, three);

    BOOST_CHECK_EQUAL(0, pow1->degree(pow2));
}

BOOST_AUTO_TEST_CASE(sumProductNoSubExpression)
{
    const BasePtr sum = Sum::create(two, a);
    const BasePtr product = Product::create(b, c);

    BOOST_CHECK_EQUAL(0, sum->degree(product));
}

BOOST_AUTO_TEST_CASE(productPowerNoSubExpression)
{
    const BasePtr pow = Power::create(c, two);

    BOOST_CHECK_EQUAL(0, abProduct->degree(pow));
}

/* The following tests concern Power, Product or Sum types and degrees of subexpressions. */
BOOST_AUTO_TEST_CASE(simpleSum)
{
    BOOST_CHECK_EQUAL(1, abSum->degree(a));
    BOOST_CHECK_EQUAL(1, abSum->degree(b));
}

BOOST_AUTO_TEST_CASE(sumWithProduct)
{
    const BasePtr sum = Sum::create(two, a, Product::create(c, d));

    BOOST_CHECK_EQUAL(1, sum->degree(d));
}

BOOST_AUTO_TEST_CASE(sumWithMultipleExponents)
    /* Degree(10 + a + a*b + a^3 + a^5*b*c, a) = 5. */
{
    const BasePtr sum = Sum::create({ ten, a, abProduct, Power::create(a, three),
            Product::create(b, c, Power::create(a, five)) });

    BOOST_CHECK_EQUAL(5, sum->degree(a));
}

BOOST_AUTO_TEST_CASE(sumWithNegativeAndZeroDegree)
    /* Degree(a^(-4) + b^(-3), a) = 0, because degree(b^(-3), a) = 0 > -4. */
{
    const BasePtr sum = Sum::create(Power::create(a, Numeric::create(-4)),
            Power::create(b, Numeric::create(-3)));

    BOOST_CHECK_EQUAL(0, sum->degree(a));
}

BOOST_AUTO_TEST_CASE(sumWithOnlyNegativeDegrees)
    /* Degree(a^(-7) + a^(-3)*b*sin(a), a) = -3. */
{
    const BasePtr summand1 = Power::create(a, Numeric::create(-7));
    const BasePtr pow = Power::create(a, Numeric::create(-3));
    const BasePtr summand2 = Product::create(pow, b, Trigonometric::createSin(a));
    const BasePtr sum = Sum::create(summand1, summand2);

    BOOST_CHECK_EQUAL(-3, sum->degree(a));
}

BOOST_AUTO_TEST_CASE(simpleProduct)
{
    BOOST_CHECK_EQUAL(1, abProduct->degree(a));
    BOOST_CHECK_EQUAL(1, abProduct->degree(b));
}

BOOST_AUTO_TEST_CASE(unexpandedProduct)
    /* Degree(a*(a + a*b*(a + c)), a) = 3. */
{
    const BasePtr aTimesBTimesAPlusC = Product::create(abProduct, Sum::create(a, c));
    const BasePtr sum = Sum::create(a, aTimesBTimesAPlusC);
    const BasePtr product = Product::create(a, sum);

    BOOST_CHECK_EQUAL(3, product->degree(a));
}

BOOST_AUTO_TEST_CASE(productWithNegativeExp)
    /* Degree(a*(a^(-4) + b), a) = 1. */
{
    const BasePtr product = Product::create(a,
            Sum::create(Power::create(a, Numeric::create(-4)), b));

    BOOST_CHECK_EQUAL(1, product->degree(a));
}

BOOST_AUTO_TEST_CASE(simplePower)
{
    const BasePtr pow = Power::create(a, ten);

    BOOST_CHECK_EQUAL(10, pow->degree(a));
}

BOOST_AUTO_TEST_CASE(simplePowerSumBase)
{
    const BasePtr pow = Power::create(abSum, three);

    BOOST_CHECK_EQUAL(3, pow->degree(a));
}

BOOST_AUTO_TEST_CASE(powerSumBase)
{
    const BasePtr pow = Power::create(Sum::create(a, Power::create(a, Numeric::create(-5))),
            Numeric::create(-2));

    BOOST_CHECK_EQUAL(-2, pow->degree(a));
}

BOOST_AUTO_TEST_CASE(powerTooLargeIntExp)
{
    const BasePtr pow = Power::create(a, Numeric::create(std::numeric_limits<int>::max()));
    int degree;

    disableLog();
    degree = pow->degree(a);
    enableLog();

    BOOST_CHECK_EQUAL(0, degree);
}

BOOST_AUTO_TEST_CASE(powerWithNegIntExp)
{
    const BasePtr pow = Power::create(a, Numeric::create(-2));
    const int degree = pow->degree(a);

    BOOST_CHECK_EQUAL(-2, degree);
}

BOOST_AUTO_TEST_CASE(powerTooSmallIntExp)
{
    const Int largeNeg("-230980928430982309482098409283094832");
    const BasePtr pow = Power::create(a, Numeric::create(largeNeg));
    int degree;

    disableLog();
    degree = pow->degree(a);
    enableLog();

    BOOST_TEST(!integer::fitsInto<int>(largeNeg));
    BOOST_CHECK_EQUAL(0, degree);
}

BOOST_AUTO_TEST_SUITE_END()
