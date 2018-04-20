
#include "poly.h"
#include "product.h"
#include "undefined.h"
#include "power.h"
#include "numeric.h"
#include "sum.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestPolyUnit, AbcFixture)

BOOST_AUTO_TEST_CASE(unitOfZero)
{
    BOOST_CHECK_EQUAL(1, poly::unit(zero, a));
    BOOST_CHECK_EQUAL(1, poly::unit(zero, zero));
}

BOOST_AUTO_TEST_CASE(undefined, noLogs())
    /* Illegal input is supposed to return 1. */
{
    BOOST_CHECK_EQUAL(1, poly::unit(Undefined::create(), a));
}

BOOST_AUTO_TEST_CASE(illegalNumPowerInput, noLogs())
{
    const BasePtr illegal = Product::create(two,
            Sum::create(Power::create(five, Numeric::third()), b));
    const BasePtr sum = Sum::create(a, illegal);

    BOOST_CHECK_EQUAL(1, poly::unit(sum, b));
}

BOOST_AUTO_TEST_CASE(posSymbol)
{
    BOOST_CHECK_EQUAL(1, poly::unit(a, a));
}

BOOST_AUTO_TEST_CASE(negSymbol)
{
    BOOST_CHECK_EQUAL(-1, poly::unit(Product::minus(a), a));
}

BOOST_AUTO_TEST_CASE(negMonomial)
{
    const BasePtr monomial = Product::minus(a, b, Power::create(c, two));

    BOOST_CHECK_EQUAL(-1, poly::unit(monomial, c));
}

BOOST_AUTO_TEST_CASE(posMonomial)
{
    const BasePtr monomial = Product::create(a, b, Power::create(c, three));

    BOOST_CHECK_EQUAL(1, poly::unit(monomial, b));
}

BOOST_AUTO_TEST_CASE(longNegativeMonomial)
{
    BasePtrList factors;
    BasePtr monomial;

    factors.push_back(Numeric::mOne()); factors.push_back(Power::create(a, ten));
    factors.push_back(b); factors.push_back(Power::create(c, five));
    factors.push_back(Power::create(d, Numeric::create(112))); factors.push_back(Power::create(e,
                two)); factors.push_back(f);

    monomial = Product::create(factors);

    BOOST_CHECK_EQUAL(-1, poly::unit(monomial, a));
    BOOST_CHECK_EQUAL(-1, poly::unit(monomial, b));
    BOOST_CHECK_EQUAL(-1, poly::unit(monomial, c));
    BOOST_CHECK_EQUAL(-1, poly::unit(monomial, d));
    BOOST_CHECK_EQUAL(-1, poly::unit(monomial, e));
    BOOST_CHECK_EQUAL(-1, poly::unit(monomial, g));
}

BOOST_AUTO_TEST_CASE(mixedSignPolynomial)
    /* Unit(a*b^2 - a*c^3, a) = 1. */
{
    const BasePtr polynomial = Sum::create(Product::create(a, b, b),
            Product::minus(a, Power::create(c, three)));

    BOOST_CHECK_EQUAL(1, poly::unit(polynomial, a));
}

BOOST_AUTO_TEST_CASE(mixedInverseSignPolynomial)
    /* Same as before, but multiplied with -1. */
{
    const BasePtr polynomial = Sum::create(Product::minus(a, b, b), Product::create(a, c, c, c));

    BOOST_CHECK_EQUAL(-1, poly::unit(polynomial, a));
}

BOOST_AUTO_TEST_CASE(positiveSignPolynomial)
    /* Unit(1/2*a*b^2 + a^2*b^3 + a^3*b^5, a) = 1. */
{
    const BasePtr polynomial = Sum::create(Product::create(Numeric::half(), a, b, b),
            Product::create(a, a, Power::create(b, three)),
            Product::create(Power::create(a, three), Power::create(b, five)));

    BOOST_CHECK_EQUAL(1, poly::unit(polynomial, a));
}

BOOST_AUTO_TEST_CASE(negativeSignPolynomial)
    /* Unit(-1/5*a*b - 2/7*c*d, d) = -1. */
{
    const BasePtr polynomial = Sum::create(Product::create(Numeric::create(-1, 5), a, b),
            Product::create(Numeric::create(-2, 7), c, d));

    BOOST_CHECK_EQUAL(-1, poly::unit(polynomial, d));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(TestPolyContent, AbcFixture)

BOOST_AUTO_TEST_CASE(contentOfZero)
{
    BOOST_CHECK_EQUAL(zero, poly::content(zero, a));
}

BOOST_AUTO_TEST_CASE(posSymbol)
{
    BOOST_CHECK_EQUAL(one, poly::content(a, a));
}

BOOST_AUTO_TEST_CASE(negSymbol)
{
    BOOST_CHECK_EQUAL(one, poly::content(Product::minus(a), a));
}

BOOST_AUTO_TEST_CASE(positiveNumeric)
{
    BOOST_CHECK_EQUAL(five, poly::content(five, a));
}

BOOST_AUTO_TEST_CASE(negativeNumeric)
{
    const BasePtr n = Numeric::create(-1, 10);

    BOOST_CHECK_EQUAL(Numeric::create(1, 10), poly::content(n, a));
}

BOOST_AUTO_TEST_CASE(simpleMultiVarPolyWithIntCoeff)
    /* Content(-2*a*b - b^2, a) = b. */
{
    const BasePtr poly = Sum::create(Product::minus(two, a, b), Product::minus(b, b));

    BOOST_CHECK_EQUAL(b, poly::content(poly, a));
}

BOOST_AUTO_TEST_CASE(simpleMultivarPoly)
    /* Content(-a^2*b + b^3, a) = b. */
{
    const BasePtr arg = Sum::create(Product::minus(a, a, b), Power::create(b, three));

    BOOST_CHECK_EQUAL(b, poly::content(arg, a));
}

BOOST_AUTO_TEST_CASE(multivarPoly)
    /* Content(14*a^2 + 14*a*c^5, a) = 14. */
{
    const BasePtr fourteen = Numeric::create(14);
    const BasePtr arg = Sum::create(Product::create(fourteen, a, a),
            Product::create(fourteen, a, Power::create(c, five)));
    const BasePtr result = poly::content(arg, a);

    BOOST_CHECK_EQUAL(fourteen, result);
}

BOOST_AUTO_TEST_SUITE_END()
