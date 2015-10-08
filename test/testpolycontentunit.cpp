
#include "cpputest.h"
#include "abc.h"
#include "poly.h"
#include "product.h"
#include "power.h"
#include "sum.h"

using namespace tsym;

TEST_GROUP(PolyUnit) {};

TEST(PolyUnit, zero)
{
    CHECK_EQUAL(1, poly::unit(zero, a));
    CHECK_EQUAL(1, poly::unit(zero, zero));
}

TEST(PolyUnit, undefined)
    /* Illegal input is supposed to return 1. */
{
    disableLog();
    CHECK_EQUAL(1, poly::unit(Undefined::create(), a));
    enableLog();
}

TEST(PolyUnit, illegalNumPowerInput)
{
    const BasePtr illegal = Product::create(two,
            Sum::create(Power::create(five, Numeric::create(1, 3)), b));
    const BasePtr sum = Sum::create(a, illegal);

    disableLog();
    CHECK_EQUAL(1, poly::unit(sum, b));
    enableLog();
}

TEST(PolyUnit, posSymbol)
{
    CHECK_EQUAL(1, poly::unit(a, a));
}

TEST(PolyUnit, negSymbol)
{
    CHECK_EQUAL(-1, poly::unit(Product::minus(a), a));
}

TEST(PolyUnit, negMonomial)
{
    const BasePtr monomial = Product::minus(a, b, Power::create(c, two));

    CHECK_EQUAL(-1, poly::unit(monomial, c));
}

TEST(PolyUnit, posMonomial)
{
    const BasePtr monomial = Product::create(a, b, Power::create(c, three));

    CHECK_EQUAL(1, poly::unit(monomial, b));
}

TEST(PolyUnit, longNegativeMonomial)
{
    BasePtrList factors;
    BasePtr monomial;

    factors.push_back(Numeric::mOne());
    factors.push_back(Power::create(a, ten));
    factors.push_back(b);
    factors.push_back(Power::create(c, five));
    factors.push_back(Power::create(d, Numeric::create(112)));
    factors.push_back(Power::create(e, two));
    factors.push_back(f);

    monomial = Product::create(factors);

    CHECK_EQUAL(-1, poly::unit(monomial, a));
    CHECK_EQUAL(-1, poly::unit(monomial, b));
    CHECK_EQUAL(-1, poly::unit(monomial, c));
    CHECK_EQUAL(-1, poly::unit(monomial, d));
    CHECK_EQUAL(-1, poly::unit(monomial, e));
    CHECK_EQUAL(-1, poly::unit(monomial, g));
}

TEST(PolyUnit, mixedSignPolynomial)
    /* Unit(a*b^2 - a*c^3, a) = 1. */
{
    const BasePtr polynomial = Sum::create(Product::create(a, b, b),
            Product::minus(a, Power::create(c, three)));

    CHECK_EQUAL(1, poly::unit(polynomial, a));
}

TEST(PolyUnit, mixedInverseSignPolynomial)
    /* Same as before, but multiplied with -1. */
{
    const BasePtr polynomial = Sum::create(Product::minus(a, b, b), Product::create(a, c, c, c));

    CHECK_EQUAL(-1, poly::unit(polynomial, a));
}

TEST(PolyUnit, positiveSignPolynomial)
    /* Unit(1/2*a*b^2 + a^2*b^3 + a^3*b^5, a) = 1. */
{
    const BasePtr polynomial = Sum::create(Product::create(Numeric::create(1, 2), a, b, b),
            Product::create(a, a, Power::create(b, three)),
            Product::create(Power::create(a, three), Power::create(b, five)));

    CHECK_EQUAL(1, poly::unit(polynomial, a));
}

TEST(PolyUnit, negativeSignPolynomial)
    /* Unit(-1/5*a*b - 2/7*c*d, d) = -1. */
{
    const BasePtr polynomial = Sum::create(Product::create(Numeric::create(-1, 5), a, b),
            Product::create(Numeric::create(-2, 7), c, d));

    CHECK_EQUAL(-1, poly::unit(polynomial, d));
}

TEST_GROUP(PolyContent) {};

TEST(PolyContent, zero)
{
    CHECK_EQUAL(zero, poly::content(zero, a));
}

TEST(PolyContent, posSymbol)
{
    CHECK_EQUAL(one, poly::content(a, a));
}

TEST(PolyContent, negSymbol)
{
    CHECK_EQUAL(one, poly::content(Product::minus(a), a));
}

TEST(PolyContent, positiveNumeric)
{
    CHECK_EQUAL(five, poly::content(five, a));
}

TEST(PolyContent, negativeNumeric)
{
    const BasePtr n = Numeric::create(-1, 10);

    CHECK_EQUAL(Numeric::create(1, 10), poly::content(n, a));
}

TEST(PolyContent, simpleMultiVarPolyWithIntCoeff)
    /* Content(-2*a*b - b^2, a) = b. */
{
    const BasePtr poly = Sum::create(Product::minus(two, a, b), Product::minus(b, b));

    CHECK_EQUAL(b, poly::content(poly, a));
}

TEST(PolyContent, simpleMultivarPoly)
    /* Content(-a^2*b + b^3, a) = b. */
{
    const BasePtr arg = Sum::create(Product::minus(a, a, b), Power::create(b, three));

    CHECK_EQUAL(b, poly::content(arg, a));
}

TEST(PolyContent, multivarPoly)
    /* Content(14*a^2 + 14*a*c^5, a) = 14. */
{
    const BasePtr fourteen = Numeric::create(14);
    const BasePtr arg = Sum::create(Product::create(fourteen, a, a),
            Product::create(fourteen, a, Power::create(c, five)));
    const BasePtr result = poly::content(arg, a);

    CHECK_EQUAL(fourteen, result);
}
