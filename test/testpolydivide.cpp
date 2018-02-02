
#include "abc.h"
#include "poly.h"
#include "sum.h"
#include "power.h"
#include "numeric.h"
#include "product.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(PolyDivide) {};

TEST(PolyDivide, zeroDividend)
{
    const BasePtrList result = poly::divide(zero, two, { a, b });

    CHECK_EQUAL(zero, result.front());
    CHECK_EQUAL(zero, result.back());
}

TEST(PolyDivide, emptyContainer)
{
    const BasePtr u = Sum::create(a, b);
    const BasePtr v = Product::create(c, Sum::create(d, e));
    const BasePtrList result = poly::divide(u, v, {});

    CHECK_EQUAL(zero, result.front());
    CHECK_EQUAL(u, result.back());
}

TEST(PolyDivide, wrongSymbolsInContainer)
{
    const BasePtr u = Power::create(Sum::create(a, b), two);
    const BasePtr v = Sum::create(Product::create(two, c), Power::create(d, three));
    const BasePtrList result = poly::divide(u, v, { e, f });

    CHECK_EQUAL(zero, result.front());
    CHECK_EQUAL(u, result.back());
}

TEST(PolyDivide, twoFractionsEmptyContainer)
    /* (1/3)/(4/5) = 5/12. */
{
    const BasePtr expected = Numeric::create(5, 12);
    const BasePtrList result = poly::divide(Numeric::third(), Numeric::create(4, 5), {});

    CHECK_EQUAL(expected, result.front());
    CHECK(result.back()->isZero());
}

TEST(PolyDivide, fractionCoeff)
{
    const BasePtr expected = Product::create(Numeric::create(-14, 9), Power::create(a, two));
    const BasePtr u = Product::create(Numeric::create(2, 3), Power::create(a, three), b);
    const BasePtr v = Product::create(Numeric::create(-3, 7), a, b);
    BasePtrList result;

    result = poly::divide(u, v, { a, b });

    CHECK_EQUAL(expected, result.front());
    CHECK_EQUAL(zero, result.back());
}

TEST(PolyDivide, invalidInputBothZero)
{
    BasePtrList result;

    disableLog();
    result = poly::divide(zero, zero);
    enableLog();

    CHECK(result.front()->isUndefined());
    CHECK(result.back()->isUndefined());
}

TEST(PolyDivide, invalidInputWithFunction)
{
    const BasePtr u = Trigonometric::createAcos(a);
    const BasePtr v = Sum::create(two, a);
    BasePtrList result;

    disableLog();
    result = poly::divide(u, v, { a });
    enableLog();

    CHECK(result.front()->isUndefined());
    CHECK(result.back()->isUndefined());
}

TEST(PolyDivide, intRemainder)
    /* (3*a^3 - 5*a^2 + 10*a - 3)/(3a + 1) in a: quotient a^2 - 2a + 4, remainder -7. */
{
    const BasePtr divisor = Sum::create(Product::create(three, a), one);
    const BasePtr expectedRemainder = Numeric::create(-7);
    BasePtr expectedQuotient;
    BasePtr dividend;
    BasePtrList res;

    dividend = Sum::create({ Product::create(three, Power::create(a, three)),
        Product::create(Numeric::create(-5), Power::create(a, two)), Product::create(ten, a),
        Numeric::create(-3) });

    res = poly::divide(dividend, divisor);

    expectedQuotient = Sum::create({ Power::create(a, two), Product::minus(two, a), four });

    CHECK_EQUAL(expectedQuotient, res.front());
    CHECK_EQUAL(expectedRemainder, res.back());
}

TEST(PolyDivide, noQuotient)
    /* (a^2 - 2*a - b)/(a*b - 1) in a and b: no quotient. */
{
    const BasePtr divisor = Sum::create(Product::create(a, b), Numeric::create(-1));
    const BasePtr dividend = Sum::create(Power::create(a, two), Sum::create(Product::create(a,
                    Numeric::create(-2)), Product::create(Numeric::create(-1), b)));
    BasePtrList res;

    res = poly::divide(dividend, divisor, { a, b });

    CHECK(res.front()->isZero());
    CHECK_EQUAL(dividend, res.back());
}

TEST(PolyDivide, quotientWithRationalCoeff)
    /* (-a^2*b + b^3)/(-2*b) = 1/2*a^2 - 1/2*b^2. */
{
    const BasePtr& half = Numeric::half();
    const BasePtr u = Sum::create(Product::minus(a, a, b), Power::create(b, three));
    const BasePtr v = Product::minus(two, b);
    const BasePtr expected = Sum::create(Product::create(half, a, a), Product::minus(half, b, b));
    BasePtrList res;

    res = poly::divide(u, v, { a, b });

    CHECK_EQUAL(expected, res.front());
    CHECK(res.back()->isZero());
}

TEST(PolyDivide, multipleVarsNoRemainder)
{
    const BasePtr p1 = Sum::create(Product::create(a, Power::create(b, two)),
            Product::create(a, c));
    const BasePtr p2 = Sum::create(Power::create(d, three), Product::create(two, e));
    const BasePtr dividend = Product::create(p1, p2)->expand();
    const BasePtrList vars { a, b, c, d, e };
    const BasePtrList res = poly::divide(dividend, p2, vars);

    CHECK_EQUAL(p1, res.front());
    CHECK(res.back()->isZero());
}

TEST(PolyDivide, hugeExpandedPolynomials)
{
    const BasePtr p1 = Sum::create(Product::create(two, a), Product::create(b, c));
    const BasePtr p2 = Sum::create(Power::create(d, five), Power::create(e, four));
    const BasePtr p3 = Sum::create(Product::create(Numeric::create(17), Power::create(b, two)),
            Product::create(Numeric::mOne(), five, b, a));
    const BasePtr p4 = Sum::create(Product::create(Power::create(c, three), a, b, Product::create(d,
                    e)), Power::create(e, two), Product::minus(Power::create(d, five)));
    const BasePtr p12 = Product::create(p1, p2)->expand();
    const BasePtr p34 = Product::create(p3, p4)->expand();
    const BasePtr p1234 = Product::create(p12, p34)->expand();
    const BasePtrList vars { a, b, c, d, e };
    BasePtrList res;

    res = poly::divide(p1234, p12, vars);

    CHECK_EQUAL(p34, res.front());
    CHECK_EQUAL(zero, res.back());

    res = poly::divide(p1234, p34, vars);

    CHECK_EQUAL(p12, res.front());
    CHECK_EQUAL(zero, res.back());
}

TEST_GROUP(PolyPseudoDivide) {};

TEST(PolyPseudoDivide, cohenExample01)
{
    const BasePtr u = a;
    const BasePtr v = Sum::create(Product::create(a, b), b);
    const BasePtrList result = poly::pseudoDivide(u, v, a);

    CHECK_EQUAL(one, result.front());
    CHECK_EQUAL(Product::minus(b), result.back());
}

TEST(PolyPseudoDivide, cohenExample02)
{
    const BasePtr expectedQuotient = Product::create(ten, a, Power::create(b, four));
    const BasePtr expectedRemainder = Sum::create(
            Product::create(Numeric::create(-20), a, a, Power::create(b, four)),
            Product::create(Numeric::create(-30), a, Power::create(b, four)),
            Product::create(Numeric::create(12), a, Power::create(b, three)),
            Product::create(eight, b, b));
    const BasePtr u = Sum::create(
            Product::create(five, Power::create(a, four), Power::create(b, three)),
            Product::create(three, a, b), two);
    const BasePtr v = Sum::create(Product::create(two, Power::create(a, three), b),
            Product::create(two, a), three);
    const BasePtrList result = poly::pseudoDivide(u, v, a);

    CHECK_EQUAL(expectedQuotient, result.front());
    CHECK_EQUAL(expectedRemainder, result.back());
}

TEST(PolyPseudoDivide, illegalInput)
{
    const BasePtr u = Power::create(a, Numeric::create(1.23456789));
    const BasePtr v = Trigonometric::createSin(a);
    BasePtrList result;

    disableLog();
    result = poly::pseudoDivide(u, v, a);
    enableLog();

    CHECK(result.front()->isUndefined());
    CHECK(result.back()->isUndefined());
}
