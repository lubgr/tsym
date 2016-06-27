
#include "abc.h"
#include "sum.h"
#include "trigonometric.h"
#include "product.h"
#include "constant.h"
#include "power.h"
#include "cpputest.h"

using namespace tsym;

TEST_GROUP(Coeff) {};

TEST(Coeff, equalSymbolsExpOne)
{
    CHECK_EQUAL(one, a->coeff(a, 1));
}

TEST(Coeff, equalSymbolsExpZero)
{
    CHECK_EQUAL(zero, a->coeff(a, 0));
}

TEST(Coeff, differentSymbolsExpOne)
{
    CHECK_EQUAL(zero, a->coeff(b, 1));
}

TEST(Coeff, differentSymbolExpZero)
{
    CHECK_EQUAL(a, a->coeff(b, 0));
}

TEST(Coeff, symbolAndProductExpZero)
{
    const BasePtr product = Product::create(a, b, c);

    CHECK_EQUAL(a, a->coeff(product, 0));
}

TEST(Coeff, numericEqualArgExpZero)
{
    CHECK_EQUAL(two, two->coeff(two, 0));
}

TEST(Coeff, numericEqualArgExpNotOne)
{
    const BasePtr arg = Sum::create(a, b);

    CHECK_EQUAL(zero, two->coeff(two, 12));
}

TEST(Coeff, numericDifferentArgExpZero)
{
    const BasePtr arg = Power::create(a, b);

    CHECK_EQUAL(two, two->coeff(arg, 0));
}

TEST(Coeff, numericDifferentArgExpNotOne)
{
    const BasePtr arg = Sum::create(a, b);

    CHECK_EQUAL(zero, two->coeff(arg, -3));
}

TEST(Coeff, powerEqualArgExpOne)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr pow = Power::create(b, sinA);

    CHECK_EQUAL(one, pow->coeff(pow, 1));
}

TEST(Coeff, powerEqualArgExpZero)
{
    const BasePtr pow = Power::create(a, three);

    CHECK_EQUAL(zero, pow->coeff(pow, 0));
}

TEST(Coeff, powerDifferentArgExpNonZero)
{
    const BasePtr pow = Power::create(a, b);
    const BasePtr arg = Power::create(c, d);

    CHECK_EQUAL(zero, pow->coeff(arg, 3));
}

TEST(Coeff, powerDifferentArgExpZero)
{
    const BasePtr pow = Power::create(a, Sum::create(b, c));
    const BasePtr arg = Power::create(d, Product::create(e, f));

    CHECK_EQUAL(pow, pow->coeff(arg, 0));
}

TEST(Coeff, powerArgsToPowerMatches)
{
    const BasePtr sum = Sum::create(a, ten);
    const BasePtr pow = Power::create(sum, Numeric::create(-2));

    CHECK_EQUAL(one, pow->coeff(sum, -2));
}

TEST(Coeff, productEqualArgExpZero)
{
    const BasePtr product = Product::create(a, b, c);

    CHECK_EQUAL(zero, product->coeff(product, 0));
}

TEST(Coeff, productEqualArgExpOne)
{
    const BasePtr product = Product::create(two, a);

    CHECK_EQUAL(one, product->coeff(product, 1));
}

TEST(Coeff, productDifferentArgExpZero)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr arg = Product::create(c, d);

    CHECK_EQUAL(product, product->coeff(arg, 0));
}

TEST(Coeff, productDifferentArgExpNonZero)
{
    const BasePtr product = Product::create(a, Sum::create(b, c));
    const BasePtr arg = Power::create(c, d);

    CHECK_EQUAL(zero, product->coeff(arg, 3));
}

TEST(Coeff, productFactorMatchesExpOne)
{
    const BasePtr aPlusB = Sum::create(a, b);
    const BasePtr expected = Product::create(four, c);
    const BasePtr product = Product::create(aPlusB, expected);

    CHECK_EQUAL(expected, product->coeff(aPlusB, 1));
}

TEST(Coeff, productFactorMatchesArgsToPower)
{
    const BasePtr asinA = Trigonometric::createAsin(a);
    const BasePtr expected = Product::create(asinA, b);
    const BasePtr pow = Power::create(c, Numeric::create(-3));
    const BasePtr product = Product::create(pow, expected);

    CHECK_EQUAL(expected, product->coeff(c, -3));
}

TEST(Coeff, productFactorIsSubexpressionExpOne)
{
    const BasePtr sub = Product::create(b, c);
    const BasePtr product = Product::create(a, sub, d);

    CHECK_EQUAL(zero, product->coeff(sub, 1));
}

TEST(Coeff, productFactorArgsToSubexpression)
{
    const BasePtr sub = Product::create(Power::create(b, four), Power::create(c, four));
    const BasePtr product = Product::create(a, sub, d);

    CHECK_EQUAL(zero, product->coeff(Product::create(b, c), 4));
}

TEST(Coeff, sumEqualArgExpZero)
{
    const BasePtr sum = Product::create(two, a);

    CHECK_EQUAL(one, sum->coeff(sum, 1));
}

TEST(Coeff, sumDifferentArgExpZero)
{
    const BasePtr sum = Product::create(a, b);
    const BasePtr arg = Product::create(c, d);

    CHECK_EQUAL(sum, sum->coeff(arg, 0));
}

TEST(Coeff, sumDifferentArgExpNonZero)
{
    const BasePtr sum = Sum::create(a, Product::create(b, c));
    const BasePtr arg = Power::create(c, d);

    CHECK_EQUAL(zero, sum->coeff(arg, 3));
}

TEST(Coeff, summandMatchesExpOne)
{
    const BasePtr aTimesB = Product::create(a, b);
    const BasePtr sum = Sum::create(aTimesB, four, c);

    CHECK_EQUAL(one, sum->coeff(aTimesB, 1));
}

TEST(Coeff, sumBaseHasVariable)
    /* Coeff((a + b)^2, a, 0) = (a + b)^2. The result is different from Mathematica, which evaluates
     * the coefficient of the expanded power (resulting in b^2). In practice, this difference
     * shouldn't cause problems, as in polynomial division/gcd/normalization, terms are always
     * exapanded. */
{
    const BasePtr pow = Power::create(Sum::create(a, b), two);

    CHECK_EQUAL(pow, pow->coeff(a, 0));
    CHECK_EQUAL(Power::create(b, two), pow->expand()->coeff(a, 0));
}

TEST(Coeff, simpleMonomial)
    /* Coeff(-a^2*b, a, 0) = 0. */
{
    const BasePtr monomial = Product::minus(a, a, b);

    CHECK_EQUAL(zero, monomial->coeff(a, 0));
}

TEST(Coeff, simplePolynomial)
    /* Coeff(-a^2*b + b^3, a, 0) = b^3. */
{
    const BasePtr expected = Power::create(b, three);
    const BasePtr poly = Sum::create(Product::minus(a, a, b), expected);

    CHECK_EQUAL(expected, poly->coeff(a, 0));
}

TEST(Coeff, summandMatchesArgsToPower)
    /* Coeff(10 + asin(a)*c^(-3) + b*c^(-3)*d + e, c, -3) = asin(a) + b*d. */
{
    const BasePtr asinA = Trigonometric::createAsin(a);
    const BasePtr pow = Power::create(c, Numeric::create(-3));
    const BasePtr summand2 = Product::create(asinA, pow);
    const BasePtr summand3 = Product::create(b, d, pow);
    const BasePtr sum = Sum::create(ten, summand2, summand3, e);
    const BasePtr expected = Sum::create(asinA, Product::create(b, d));

    CHECK_EQUAL(expected, sum->coeff(c, -3));
}

TEST(Coeff, summandIsSubexpressionExpOne)
    /* Coeff(a + b*c*d, b*c, 1) = 0. */
{
    const BasePtr sub = Product::create(b, c);
    const BasePtr sum = Sum::create(a, Product::create(sub, d));

    CHECK_EQUAL(zero, sum->coeff(sub, 1));
}

TEST(Coeff, summandArgsToSubexpression)
    /* Coeff(a + a^3 + a^2*b*(d + e), a, 2) = b*(d + e). */
{
    const BasePtr expected = Product::create(b, Sum::create(d, e));
    const BasePtr aCubic = Power::create(a, three);
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr sum = Sum::create(a, aCubic, Product::create(aSquare, expected));

    CHECK_EQUAL(expected, sum->coeff(a, 2));
}

TEST(Coeff, sumIncludingFunctionTermsAndPowers)
    /* Coeff((3*sin(a))*a^2 + (2*cos(a))*a + 4, a, 2) = 3*sin(a). */
{
    const BasePtr threeTimesSinA = Product::create(three, Trigonometric::createSin(a));
    const BasePtr twoTimesCosA = Product::create(two, Trigonometric::createCos(a));
    const BasePtr aSquare = Power::create(a, two);
    BasePtrList summands;
    BasePtr sum;

    summands.push_back(Product::create(threeTimesSinA, aSquare));
    summands.push_back(Product::create(twoTimesCosA, a));
    summands.push_back(four);

    sum = Sum::create(summands);

    CHECK_EQUAL(threeTimesSinA, sum->coeff(a, 2));
}
