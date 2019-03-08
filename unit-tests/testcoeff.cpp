
#include "constant.h"
#include "fixtures.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestCoeff, AbcFixture)

BOOST_AUTO_TEST_CASE(equalSymbolsExpOne)
{
    BOOST_CHECK_EQUAL(one, a->coeff(*a, 1));
}

BOOST_AUTO_TEST_CASE(equalSymbolsExpZero)
{
    BOOST_CHECK_EQUAL(zero, a->coeff(*a, 0));
}

BOOST_AUTO_TEST_CASE(differentSymbolsExpOne)
{
    BOOST_CHECK_EQUAL(zero, a->coeff(*b, 1));
}

BOOST_AUTO_TEST_CASE(differentSymbolExpZero)
{
    BOOST_CHECK_EQUAL(a, a->coeff(*b, 0));
}

BOOST_AUTO_TEST_CASE(symbolAndProductExpZero)
{
    const BasePtr product = Product::create(a, b, c);

    BOOST_CHECK_EQUAL(a, a->coeff(*product, 0));
}

BOOST_AUTO_TEST_CASE(numericEqualArgExpZero)
{
    BOOST_CHECK_EQUAL(two, two->coeff(*two, 0));
}

BOOST_AUTO_TEST_CASE(numericEqualArgExpNotOne)
{
    const BasePtr arg = Sum::create(a, b);

    BOOST_CHECK_EQUAL(zero, two->coeff(*two, 12));
}

BOOST_AUTO_TEST_CASE(numericDifferentArgExpZero)
{
    const BasePtr arg = Power::create(a, b);

    BOOST_CHECK_EQUAL(two, two->coeff(*arg, 0));
}

BOOST_AUTO_TEST_CASE(numericDifferentArgExpNotOne)
{
    const BasePtr arg = Sum::create(a, b);

    BOOST_CHECK_EQUAL(zero, two->coeff(*arg, -3));
}

BOOST_AUTO_TEST_CASE(powerEqualArgExpOne)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr pow = Power::create(b, sinA);

    BOOST_CHECK_EQUAL(one, pow->coeff(*pow, 1));
}

BOOST_AUTO_TEST_CASE(powerEqualArgExpZero)
{
    const BasePtr pow = Power::create(a, three);

    BOOST_CHECK_EQUAL(zero, pow->coeff(*pow, 0));
}

BOOST_AUTO_TEST_CASE(powerDifferentArgExpNonZero)
{
    const BasePtr pow = Power::create(a, b);
    const BasePtr arg = Power::create(c, d);

    BOOST_CHECK_EQUAL(zero, pow->coeff(*arg, 3));
}

BOOST_AUTO_TEST_CASE(powerDifferentArgExpZero)
{
    const BasePtr pow = Power::create(a, Sum::create(b, c));
    const BasePtr arg = Power::create(d, Product::create(e, f));

    BOOST_CHECK_EQUAL(pow, pow->coeff(*arg, 0));
}

BOOST_AUTO_TEST_CASE(powerArgsToPowerMatches)
{
    const BasePtr sum = Sum::create(a, ten);
    const BasePtr pow = Power::create(sum, Numeric::create(-2));

    BOOST_CHECK_EQUAL(one, pow->coeff(*sum, -2));
}

BOOST_AUTO_TEST_CASE(productEqualArgExpZero)
{
    const BasePtr product = Product::create(a, b, c);

    BOOST_CHECK_EQUAL(zero, product->coeff(*product, 0));
}

BOOST_AUTO_TEST_CASE(productEqualArgExpOne)
{
    const BasePtr product = Product::create(two, a);

    BOOST_CHECK_EQUAL(one, product->coeff(*product, 1));
}

BOOST_AUTO_TEST_CASE(productDifferentArgExpZero)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr arg = Product::create(c, d);

    BOOST_CHECK_EQUAL(product, product->coeff(*arg, 0));
}

BOOST_AUTO_TEST_CASE(productDifferentArgExpNonZero)
{
    const BasePtr product = Product::create(a, Sum::create(b, c));
    const BasePtr arg = Power::create(c, d);

    BOOST_CHECK_EQUAL(zero, product->coeff(*arg, 3));
}

BOOST_AUTO_TEST_CASE(productFactorMatchesExpOne)
{
    const BasePtr aPlusB = Sum::create(a, b);
    const BasePtr expected = Product::create(four, c);
    const BasePtr product = Product::create(aPlusB, expected);

    BOOST_CHECK_EQUAL(expected, product->coeff(*aPlusB, 1));
}

BOOST_AUTO_TEST_CASE(productFactorMatchesArgsToPower)
{
    const BasePtr asinA = Trigonometric::createAsin(a);
    const BasePtr expected = Product::create(asinA, b);
    const BasePtr pow = Power::create(c, Numeric::create(-3));
    const BasePtr product = Product::create(pow, expected);

    BOOST_CHECK_EQUAL(expected, product->coeff(*c, -3));
}

BOOST_AUTO_TEST_CASE(productFactorIsSubexpressionExpOne)
{
    const BasePtr sub = Product::create(b, c);
    const BasePtr product = Product::create(a, sub, d);

    BOOST_CHECK_EQUAL(zero, product->coeff(*sub, 1));
}

BOOST_AUTO_TEST_CASE(productFactorArgsToSubexpression)
{
    const BasePtr sub = Product::create(Power::create(b, four), Power::create(c, four));
    const BasePtr product = Product::create(a, sub, d);

    BOOST_CHECK_EQUAL(zero, product->coeff(*Product::create(b, c), 4));
}

BOOST_AUTO_TEST_CASE(sumEqualArgExpZero)
{
    const BasePtr sum = Product::create(two, a);

    BOOST_CHECK_EQUAL(one, sum->coeff(*sum, 1));
}

BOOST_AUTO_TEST_CASE(sumDifferentArgExpZero)
{
    const BasePtr sum = Product::create(a, b);
    const BasePtr arg = Product::create(c, d);

    BOOST_CHECK_EQUAL(sum, sum->coeff(*arg, 0));
}

BOOST_AUTO_TEST_CASE(sumDifferentArgExpNonZero)
{
    const BasePtr sum = Sum::create(a, Product::create(b, c));
    const BasePtr arg = Power::create(c, d);

    BOOST_CHECK_EQUAL(zero, sum->coeff(*arg, 3));
}

BOOST_AUTO_TEST_CASE(summandMatchesExpOne)
{
    const BasePtr aTimesB = Product::create(a, b);
    const BasePtr sum = Sum::create(aTimesB, four, c);

    BOOST_CHECK_EQUAL(one, sum->coeff(*aTimesB, 1));
}

BOOST_AUTO_TEST_CASE(sumBaseHasVariable)
/* Coeff((a + b)^2, a, 0) = (a + b)^2. The result is different from Mathematica, which evaluates
 * the coefficient of the expanded power (resulting in b^2). In practice, this difference
 * shouldn't cause problems, as in polynomial division/gcd/normalization, terms are always
 * exapanded. */
{
    const BasePtr pow = Power::create(Sum::create(a, b), two);

    BOOST_CHECK_EQUAL(pow, pow->coeff(*a, 0));
    BOOST_CHECK_EQUAL(Power::create(b, two), pow->expand()->coeff(*a, 0));
}

BOOST_AUTO_TEST_CASE(simpleMonomial)
/* Coeff(-a^2*b, a, 0) = 0. */
{
    const BasePtr monomial = Product::minus(a, a, b);

    BOOST_CHECK_EQUAL(zero, monomial->coeff(*a, 0));
}

BOOST_AUTO_TEST_CASE(simplePolynomial)
/* Coeff(-a^2*b + b^3, a, 0) = b^3. */
{
    const BasePtr expected = Power::create(b, three);
    const BasePtr poly = Sum::create(Product::minus(a, a, b), expected);

    BOOST_CHECK_EQUAL(expected, poly->coeff(*a, 0));
}

BOOST_AUTO_TEST_CASE(summandMatchesArgsToPower)
/* Coeff(10 + asin(a)*c^(-3) + b*c^(-3)*d + e, c, -3) = asin(a) + b*d. */
{
    const BasePtr asinA = Trigonometric::createAsin(a);
    const BasePtr pow = Power::create(c, Numeric::create(-3));
    const BasePtr summand2 = Product::create(asinA, pow);
    const BasePtr summand3 = Product::create(b, d, pow);
    const BasePtr sum = Sum::create(ten, summand2, summand3, e);
    const BasePtr expected = Sum::create(asinA, Product::create(b, d));

    BOOST_CHECK_EQUAL(expected, sum->coeff(*c, -3));
}

BOOST_AUTO_TEST_CASE(summandIsSubexpressionExpOne)
/* Coeff(a + b*c*d, b*c, 1) = 0. */
{
    const BasePtr sub = Product::create(b, c);
    const BasePtr sum = Sum::create(a, Product::create(sub, d));

    BOOST_CHECK_EQUAL(zero, sum->coeff(*sub, 1));
}

BOOST_AUTO_TEST_CASE(summandArgsToSubexpression)
/* Coeff(a + a^3 + a^2*b*(d + e), a, 2) = b*(d + e). */
{
    const BasePtr expected = Product::create(b, Sum::create(d, e));
    const BasePtr aCubic = Power::create(a, three);
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr sum = Sum::create(a, aCubic, Product::create(aSquare, expected));

    BOOST_CHECK_EQUAL(expected, sum->coeff(*a, 2));
}

BOOST_AUTO_TEST_CASE(sumIncludingFunctionTermsAndPowers)
/* Coeff((3*sin(a))*a^2 + (2*cos(a))*a + 4, a, 2) = 3*sin(a). */
{
    const BasePtr threeTimesSinA = Product::create(three, Trigonometric::createSin(a));
    const BasePtr twoTimesCosA = Product::create(two, Trigonometric::createCos(a));
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr sum = Sum::create({Product::create(threeTimesSinA, aSquare), Product::create(twoTimesCosA, a), four});

    BOOST_CHECK_EQUAL(threeTimesSinA, sum->coeff(*a, 2));
}

BOOST_AUTO_TEST_SUITE_END()
