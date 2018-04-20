
#include "sum.h"
#include "product.h"
#include "power.h"
#include "primitivegcd.h"
#include "subresultantgcd.h"
#include "undefined.h"
#include "numeric.h"
#include "poly.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

struct GcdFixture : public AbcFixture {
    const BasePtr zeroAfterExpansion = Sum::create({ Product::create(a, b), Product::minus(b, c),
            Product::create(b, Sum::create(c, Product::minus(a))) });
    const BasePtr undefined = Undefined::create();

    void check(const BasePtr& expected, const BasePtr& u, const BasePtr& v)
    {
        checkPrimitive(expected, u, v);
        checkSubresultant(expected, u, v);
    }

    void checkPrimitive(const BasePtr& expected, const BasePtr& u, const BasePtr& v)
    {
        PrimitiveGcd pGcd;

        check(pGcd, expected, u, v);
    }

    void checkSubresultant(const BasePtr& expected, const BasePtr& u, const BasePtr& v)
    {
        SubresultantGcd srGcd;

        check(srGcd, expected, u, v);
    }

    void check(Gcd& gcd, const BasePtr& expected, const BasePtr& u, const BasePtr& v)
    {
        const BasePtr result = poly::gcd(u, v, gcd);

        if (expected->isUndefined()) {
            BOOST_TEST(result->isUndefined());
        } else {
            BOOST_CHECK_EQUAL(expected, result);
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(TestGcd, GcdFixture)

BOOST_AUTO_TEST_CASE(invalidInput, noLogs())
    /* Only integer Numerics or Symblics or composition of these types are allowed. */
{
    check(undefined, Numeric::create(1.23456789), undefined);
}

BOOST_AUTO_TEST_CASE(twoIntegerNumerics)
{
    check(three, three, nine);
}

BOOST_AUTO_TEST_CASE(twoRationalNumerics)
    /* Gcd(1/3, 1/15) = 1. */
{
    const BasePtr oneOverFifteen = Numeric::create(1, 15);

    check(one, Numeric::third(), oneOverFifteen);
}

BOOST_AUTO_TEST_CASE(twoIntegerNumericsWithGcdOne)
{
    check(one, three, seven);
}

BOOST_AUTO_TEST_CASE(equalSums)
    /* The gcd of two equal arguments is obviously the same expression. */
{
    const BasePtr sum = Sum::create(a, b);

    check(sum, sum, sum);
}

BOOST_AUTO_TEST_CASE(bothZero, noLogs())
    /* Shall return an Undefined, because the request doesn't make any sense. */
{
    check(undefined, zero, zero);
}

BOOST_AUTO_TEST_CASE(bothZeroAfterExpansion)
    /* Same as above. */
{
    check(undefined, zeroAfterExpansion, zeroAfterExpansion);
}

BOOST_AUTO_TEST_CASE(firstArgZeroAfterExpansion)
{
    check(a, zeroAfterExpansion, a);
}

BOOST_AUTO_TEST_CASE(secondArgZeroAfterExpansion)
{
    check(a, zeroAfterExpansion, a);
}

BOOST_AUTO_TEST_CASE(firstArgOneAfterExpansion)
{
    const BasePtr u = Sum::create(one, zeroAfterExpansion);

    check(one, u, Sum::create(a, b));
}

BOOST_AUTO_TEST_CASE(secondArgOneAfterExpansion)
{
    const BasePtr v = Sum::create(one, zeroAfterExpansion);

    check(one, Sum::create(two, b), v);
}

BOOST_AUTO_TEST_CASE(noCommonSymbol)
{
    const BasePtr u = Sum::create(a, Power::create(b, two));
    const BasePtr v = Sum::create(Product::create(c, d), Product::create(four, e));

    check(one, u, v);
}

BOOST_AUTO_TEST_CASE(numericsAfterExpansion)
{
    const BasePtr u = Sum::create(four, Product::create(a, Sum::create(b, two)),
            Product::minus(a, b), Product::minus(two, a));
    const BasePtr v = Numeric::create(16);

    check(four, u, v);
    check(four, v, u);
}

BOOST_AUTO_TEST_CASE(equalAfterExpansion)
{
    const BasePtr unExpanded = Product::create(a, Sum::create(b, c));
    const BasePtr expanded = Sum::create(Product::create(a, b), Product::create(a, c));

    check(expanded, unExpanded, expanded);
    check(expanded, expanded, unExpanded);
}

BOOST_AUTO_TEST_CASE(negAndPosSymbol)
    /* Gcd(-a, a) = a. */
{
    check(a, Product::minus(a), a);
}

BOOST_AUTO_TEST_CASE(posPowerAndNegNumFactor)
    /* Gcd(a^2, -2*a) = a. */
{
    check(a, Power::create(a, two), Product::minus(two, a));
}

BOOST_AUTO_TEST_CASE(negNumFactorAndPosPower)
    /* Gcd(a^2, -2*a) = a. */
{
    check(a, Product::minus(two, a), Power::create(a, two));
}

BOOST_AUTO_TEST_CASE(negSymbolAndSimplePower)
    /* Gcd(-a, a^3) = a. */
{
    check(a, Product::minus(a), Power::create(a, three));
}

BOOST_AUTO_TEST_CASE(simpleUnivarMonomial)
    /* Gcd(b^2, -2*b) = b. */
{
    const BasePtr u = Power::create(b, two);
    const BasePtr v = Product::minus(two, b);

    check(b, u, v);
}

BOOST_AUTO_TEST_CASE(numericAndSimpleMonomial)
    /* Gcd(2, 2*a) = 2. */
{
    const BasePtr v = Product::create(two, a);

    check(two, two, v);
}

BOOST_AUTO_TEST_CASE(simpleMultivarPoly)
    /* Gcd(-a^2*b + b^3, a^2*b + 2*a*b^2) = b. */
{
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr u = Sum::create(Product::minus(b, aSquare), Power::create(b, three));
    const BasePtr v = Sum::create(Product::create(b, aSquare),
            Product::create(two, Power::create(b, two), a));

    check(b, u, v);
}

BOOST_AUTO_TEST_CASE(simpleMultivarPolyCohenExample)
    /* Example 6.62 in Cohen [2003]. */
{
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr bPowerThree = Power::create(b, three);
    const BasePtr expected = Sum::create(Product::create(a, b), bSquare);
    const BasePtr u = Sum::create(Product::minus(b, aSquare), Power::create(b, three));
    const BasePtr v = Sum::create(Product::create(b, aSquare),
            Product::create(two, Power::create(b, two), a), bPowerThree);

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(rationalCoefficients)
    /* Gcd(1/3*a, a) = a/3. */
{
    const BasePtr aThird = Product::create(a, Numeric::third());

    check(a, aThird, a);
}

BOOST_AUTO_TEST_CASE(simpleFractionCoefficients)
    /* Gcd(1/2*a^2*b*d - 1/3*a*c*d + 1/2*a^3*d^2, a*c*d - 2*a^3*d^2 + 2*a*b^7*d) = a*d. The results
     * given by GiNaC, Maxima or Mathematic differ in that they all give a*d/6. Thus, the division
     * of u or v by the gcd gives a polynomial with rational coefficients in our case, and one with
     * integer coefficients in the case of the listed CAS. */
{
    const BasePtr expected = Product::create(a, d);
    BasePtr u;
    BasePtr v;

    u = Sum::create( Product::create(Numeric::half(), Power::create(a, two), b, d),
            Product::create(Numeric::create(-1, 3), a, c, d),
            Product::create(Numeric::half(), Power::create(a, three), Power::create(d, two)));

    v = Sum::create(Product::create(a, c, d),
            Product::minus(two, Power::create(a, three), Power::create(d, two)),
            Product::create(two, a, Power::create(b, seven), d));

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(simpleIntegerCoefficientsAllExpOne)
    /* Gcd(168*a*b, 322*c*b) = 14*b. */
{
    const BasePtr u = Product::create(Numeric::create(168), a, b);
    const BasePtr v = Product::create(Numeric::create(322), c, b);
    const BasePtr expected = Product::create(Numeric::create(14), b);

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(simpleIntegerCoefficientsExpGreaterOne)
    /* Gcd(3*a^3*b + 3*a^2*b*c^5, 14*a^2 + 14*a*c^5) = a^2 + a*c^5). */
{
    const BasePtr u = Sum::create(Product::create(three, Power::create(a, three), b),
            Product::create(three, Power::create(a, two), b, Power::create(c, five)));
    const BasePtr v = Sum::create(Product::create(Numeric::create(14), a, a),
            Product::create(Numeric::create(14), a, Power::create(c, five)));
    const BasePtr expected = Sum::create(Power::create(a, two),
            Product::create(a, Power::create(c, five)));

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(integerCoefficientsLargerIntExp)
    /* Gcd(56*c^5*e + 12*a*b*c^6*e, 14 + 3*a*b*c) = 3*a*b*c + 14. */
{
    const BasePtr u = Sum::create(Product::create(Numeric::create(56), Power::create(c, five), e),
            Product::create(Numeric::create(12), a, b, Product::create(Power::create(c, six), e)));
    const BasePtr v = Sum::create(Numeric::create(14), Product::create(three, a, b, c));
    const BasePtr expected = v;

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(simpleIntegerCoefficients)
{
    const BasePtr u = Product::create(three, a, Power::create(b, four));
    const BasePtr v = Product::create(three, a, Power::create(b, four), Power::create(c, five));
    const BasePtr expected = Product::create(three, a, Power::create(b, four));

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(integerCoefficients)
{
    const BasePtr cefg = Product::create(c, e, f, g);
    const BasePtr u1 = Product::create(Numeric::create(-464), a, b, e);
    const BasePtr u2 = Product::create(Numeric::create(-1972), a, Power::create(b, two), cefg);
    const BasePtr u3 = Product::create(Numeric::create(3364), d, e, h);
    const BasePtr v1 = Product::create(Numeric::create(-3364), d, e);
    const BasePtr v2 = Product::create(Numeric::create(13456), Power::create(a, two), b, cefg);
    const BasePtr u = Sum::create(u1, u2, u3);
    const BasePtr v = Sum::create(v1, v2);
    const BasePtr expected = Product::create(Numeric::create(116), e);

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(largeMultivarPolyCohenExample)
    /* Example 6.63 in Cohen [2003]. */
{
    const BasePtr aCubic = Power::create(a, three);
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr expected = Sum::create(two, a);
    BasePtrList summands;
    BasePtr u;
    BasePtr v;

    summands.push_back(Product::create(aCubic, bSquare));
    summands.push_back(Product::create(six, Power::create(a, four), b));
    summands.push_back(Product::create(nine, Power::create(a, five)));
    summands.push_back(Product::create(four, aSquare, bSquare));
    summands.push_back(Product::create(Numeric::create(24), aCubic, b));
    summands.push_back(Product::create(Numeric::create(36), Power::create(a, four)));
    summands.push_back(Product::create(five, a, bSquare));
    summands.push_back(Product::create(Numeric::create(30), b, aSquare));
    summands.push_back(Product::create(Numeric::create(45), aCubic));
    summands.push_back(Product::create(two, bSquare));
    summands.push_back(Product::create(Numeric::create(12), a, b));
    summands.push_back(Product::create(Numeric::create(18), aSquare));

    u = Sum::create(summands);

    summands.clear();

    summands.push_back(Product::create(Power::create(a, five), bSquare));
    summands.push_back(Product::create(eight, Power::create(a, four), b));
    summands.push_back(Product::create(Numeric::create(16), aCubic));
    summands.push_back(Product::create(Numeric::create(12), Power::create(a, four), bSquare));
    summands.push_back(Product::create(Numeric::create(96), aCubic, b));
    summands.push_back(Product::create(Numeric::create(192), aSquare));
    summands.push_back(Product::create(Numeric::create(45), aCubic, bSquare));
    summands.push_back(Product::create(Numeric::create(360), b, aSquare));
    summands.push_back(Product::create(Numeric::create(720), a));
    summands.push_back(Product::create(Numeric::create(50), aSquare, bSquare));
    summands.push_back(Product::create(Numeric::create(400), a, b));
    summands.push_back(Numeric::create(800));

    v = Sum::create(summands);

    check(expected, u, v);
}

BOOST_AUTO_TEST_CASE(largeMultivarPoly)
{
    const BasePtr gcd = Sum::create(Numeric::create(14), Product::create(three, a, b, c));
    BasePtr aux[4];
    BasePtr u;
    BasePtr v;

    aux[0] = Sum::create(Product::create(five, Power::create(a, two), Power::create(e, four),
                Product::create(d, f)),
            Product::create(three, a, Power::create(g, three), h),
            Product::create(seven, Power::create(a, two), Power::create(c, three),
                Product::create(b, e, h)));
    aux[1] = Sum::create(a, Product::minus(two, b, d), Power::create(h, five));
    aux[2] = Sum::create(Product::create(Product::create(four, e), Power::create(d, three),
                Power::create(c, five), Power::create(d, two)),
            Product::create(Product::create(Numeric::create(-3), c), Power::create(d, three),
                Power::create(e, four), Power::create(h, two)),
            Product::create(two, a));
    aux[3] = Sum::create(Product::create(nine, a, Power::create(b, two), Power::create(c, two)),
            Product::create(Numeric::create(-5), Power::create(g, five), f,
                Power::create(d, three)),
            Product::create(two, Power::create(b, six)));

    u = Product::create(aux[0], aux[1], gcd)->expand();
    v = Product::create(aux[2], aux[3], gcd)->expand();

    check(gcd, u, v);
}

BOOST_AUTO_TEST_SUITE_END()
