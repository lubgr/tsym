
#include "constant.h"
#include "fixtures.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct ExpansionFixture : public AbcFixture {
    const BasePtr abSum = Sum::create(a, b);
    const BasePtr cdSum = Sum::create(c, d);
    const BasePtr abProduct = Product::create(a, b);
    const BasePtr i = Symbol::create("i");
};

BOOST_FIXTURE_TEST_SUITE(TestExpansion, ExpansionFixture)

BOOST_AUTO_TEST_CASE(trivialNumeric)
/* Numerics aren't modified during expansion. */
{
    const BasePtr orig = Numeric::create(2, 7);
    const BasePtr expanded = orig->expand();

    BOOST_CHECK_EQUAL(orig, expanded);
}

BOOST_AUTO_TEST_CASE(trivialSum)
/* Sums aren't modified during expansion. */
{
    const BasePtr orig = Sum::create(abSum, c);
    const BasePtr expanded = orig->expand();

    BOOST_CHECK_EQUAL(orig, expanded);
}

BOOST_AUTO_TEST_CASE(trivialUndefined)
/* Undefined expressions aren't modified during expansion. */
{
    const BasePtr expanded = undefined->expand();

    BOOST_TEST(expanded->isUndefined());
}

BOOST_AUTO_TEST_CASE(trivialConstant)
/* The same as above for constants. */
{
    const BasePtr expanded = pi->expand();

    BOOST_CHECK_EQUAL(pi, expanded);
}

BOOST_AUTO_TEST_CASE(trivialSymbol)
/* Symbols aren't modified during expansion. */
{
    const BasePtr expanded = a->expand();

    BOOST_CHECK_EQUAL(a, expanded);
}

BOOST_AUTO_TEST_CASE(powerOfSumPosIntExp)
/* (a + b)^2 = a^2 + 2*a*b + b^2. */
{
    const BasePtr expected =
      Sum::create({Power::create(a, two), Power::create(b, two), Product::create(two, abProduct)});
    const BasePtr orig = Power::create(abSum, two);

    BOOST_CHECK_EQUAL(expected, orig->expand());
}

BOOST_AUTO_TEST_CASE(powerOfSumNegIntExp)
/* (a + b)^(-2) = (a^2 + 2*a*b + b^2)^(-1). */
{
    const BasePtr orig = Power::create(abSum, Numeric::create(-2));
    const BasePtrList summands{Power::create(a, two), Power::create(b, two), Product::create(two, abProduct)};
    const BasePtr expected = Power::oneOver(Sum::create(summands));

    BOOST_CHECK_EQUAL(expected, orig->expand());
}

BOOST_AUTO_TEST_CASE(powerOfSumPosFracExp)
/* No expansions of (a + b)^(2/3). */
{
    const BasePtr orig = Power::create(abSum, Numeric::create(2, 3));
    BasePtr expanded;

    expanded = orig->expand();

    BOOST_CHECK_EQUAL(orig, expanded);
}

BOOST_AUTO_TEST_CASE(powerOfSumNegFracExp)
/* No expansions of (a + b)^(-4/5). */
{
    const BasePtr orig = Power::create(abSum, Numeric::create(-4, 5));
    BasePtr expanded;

    expanded = orig->expand();

    BOOST_CHECK_EQUAL(orig, expanded);
}

BOOST_AUTO_TEST_CASE(powerOfSymbolicSumExp)
/* No expansion of a^(b + c). */
{
    const BasePtr orig = Power::create(a, Sum::create(b, c));
    BasePtr expanded;

    expanded = orig->expand();

    BOOST_CHECK_EQUAL(orig, expanded);
}

BOOST_AUTO_TEST_CASE(numericPower)
/* No expansion of a numeric power. */
{
    const BasePtr orig = Power::create(two, Numeric::create(1, 5));
    const BasePtr expanded = orig->expand();

    BOOST_CHECK_EQUAL(orig, expanded);
}

BOOST_AUTO_TEST_CASE(symbolTimesSum)
/* a*(b + c) = a*b + a*c. */
{
    const BasePtr expected = Sum::create(abProduct, Product::create(a, c));
    const BasePtr orig = Product::create(a, Sum::create(b, c));
    BasePtr expanded;

    expanded = orig->expand();

    BOOST_CHECK_EQUAL(expected, expanded);
}

BOOST_AUTO_TEST_CASE(symbolSumTimesSum)
/* (a + b)*(c + d) = a*c + b*c + a*d + b*d. */
{
    const BasePtr orig = Product::create(abSum, cdSum);
    const BasePtr expected =
      Sum::create({Product::create(a, c), Product::create(b, c), Product::create(a, d), Product::create(b, d)});

    BOOST_CHECK_EQUAL(expected, orig->expand());
}

BOOST_AUTO_TEST_CASE(noExpansionOfFunctionArg)
/* Sin(a*(b + c)) stays constant. */
{
    const BasePtr arg = Product::create(a, Sum::create(b, c));
    const BasePtr orig = Trigonometric::createSin(arg);
    BasePtr res;

    res = orig->expand();

    BOOST_CHECK_EQUAL(orig, res);
}

BOOST_AUTO_TEST_CASE(symbolSumProduct)
/* (a + b)*(c + d)*(e + f + g)*(h + i) = ... */
{
    const BasePtr orig = Product::create({abSum, Sum::create(c, d), Sum::create(e, f, g), Sum::create(h, i)});
    BasePtrList summands;
    BasePtr expected;

    summands.push_back(Product::create(b, d, g, i));
    summands.push_back(Product::create(a, d, g, i));
    summands.push_back(Product::create(b, c, g, i));
    summands.push_back(Product::create(a, c, g, i));
    summands.push_back(Product::create(b, d, f, i));
    summands.push_back(Product::create(a, d, f, i));
    summands.push_back(Product::create(b, c, f, i));
    summands.push_back(Product::create(a, c, f, i));
    summands.push_back(Product::create(b, d, e, i));
    summands.push_back(Product::create(a, d, e, i));
    summands.push_back(Product::create(b, c, e, i));
    summands.push_back(Product::create(a, c, e, i));
    summands.push_back(Product::create(b, d, g, h));
    summands.push_back(Product::create(a, d, g, h));
    summands.push_back(Product::create(b, c, g, h));
    summands.push_back(Product::create(a, c, g, h));
    summands.push_back(Product::create(b, d, f, h));
    summands.push_back(Product::create(a, d, f, h));
    summands.push_back(Product::create(b, c, f, h));
    summands.push_back(Product::create(a, c, f, h));
    summands.push_back(Product::create(b, d, e, h));
    summands.push_back(Product::create(a, d, e, h));
    summands.push_back(Product::create(b, c, e, h));
    summands.push_back(Product::create(a, c, e, h));

    expected = Sum::create(summands);

    BOOST_CHECK_EQUAL(expected, orig->expand());
}

BOOST_AUTO_TEST_CASE(mixedSumTimesSum)
/* (a + 1)*(b + c) = b + c + a*b + a*c. */
{
    const BasePtr orig = Product::create(Sum::create(a, one), Sum::create(b, c));
    const BasePtr expected = Sum::create(b, c, abProduct, Product::create(a, c));
    BasePtr expanded;

    expanded = orig->expand();

    BOOST_CHECK_EQUAL(expected, expanded);
}

BOOST_AUTO_TEST_CASE(polynomial)
/* (a + b)/(c + d) = a/(c + d) + b/(c + d). */
{
    const BasePtr cdDenom = Power::oneOver(cdSum);
    const BasePtr orig = Product::create(abSum, cdDenom);
    const BasePtr expected = Sum::create(Product::create(a, cdDenom), Product::create(b, cdDenom));
    BasePtr expanded;

    expanded = orig->expand();

    BOOST_CHECK_EQUAL(expected, expanded);
}

BOOST_AUTO_TEST_CASE(sumWithTermsToExpand)
/* 2 + a*(b + c) = 2 + a*b + a*c. */
{
    const BasePtr expected = Sum::create(two, abProduct, Product::create(a, c));
    const BasePtr orig = Sum::create(two, Product::create(a, Sum::create(b, c)));
    BasePtr expanded;

    expanded = orig->expand();

    BOOST_CHECK_EQUAL(expected, expanded);
}

BOOST_AUTO_TEST_CASE(expansionLeadsToZero)
/* a*b - b*c + b(c - a) = 0. */
{
    const BasePtr orig =
      Sum::create({Product::create(a, b), Product::minus(b, c), Product::create(b, Sum::create(c, Product::minus(a)))});

    BOOST_TEST(!orig->isZero());
    BOOST_TEST(orig->expand()->isZero());
}

BOOST_AUTO_TEST_CASE(sumPowerOfThree)
/* (a + b + c)^3 = a^3 + 3*b^2*a + b^3 + 3*c^2*a + 3*b*c^2 + 3*b*a^2 + c^3 + 6*b*c*a + 3*b^2*c +
 * 3*c*a^2. */
{
    const BasePtr orig = Power::create(Sum::create(a, b, c), three);
    BasePtrList summands;
    BasePtr expected;

    summands.push_back(Power::create(a, three));
    summands.push_back(Power::create(b, three));
    summands.push_back(Power::create(c, three));
    summands.push_back(Product::create(three, a, b, b));
    summands.push_back(Product::create(three, a, c, c));
    summands.push_back(Product::create(three, b, c, c));
    summands.push_back(Product::create(three, a, a, b));
    summands.push_back(Product::create(three, b, b, c));
    summands.push_back(Product::create(three, a, a, c));
    summands.push_back(Product::create(six, a, b, c));

    expected = Sum::create(summands);

    BOOST_CHECK_EQUAL(expected, orig->expand());
}

BOOST_AUTO_TEST_CASE(productOfPowersWithIntExp)
/* a^2*(b + c)^2 = a^2*b^2 + 2*a^2*b*c + a^2*c^2. */
{
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr orig = Product::create(aSquare, Power::create(Sum::create(b, c), two));
    const BasePtr expected = Sum::create(Product::create(aSquare, Power::create(b, two)),
      Product::create(two, aSquare, b, c), Product::create(aSquare, Power::create(c, two)));
    BasePtr result;

    result = orig->expand();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(fractionsWithSumInDenominator)
/* According to Cohen [2003], a term is expanded (after automatic simplification), if it doesn't
 * contain any sums. Thus, 1/(a*(b + c)) is considered expanded. */
{
    const BasePtr orig = Power::oneOver(Product::create(a, Sum::create(b, c)));
    BasePtr result;

    result = orig->expand();

    BOOST_CHECK_EQUAL(orig, result);
}

BOOST_AUTO_TEST_CASE(sumsOfPowersWithIntExp)
/* The same as above, but for a more involved expression: (b^2*c)/(a^2*(d - b*c/a)) =
 * b^2*c/(a^2*d - a*b*c)). */
{
    const BasePtr num = Product::create(b, b, c);
    const BasePtr denom = Product::create(a, a, Sum::create(d, Product::minus(b, c, Power::oneOver(a))));
    const BasePtr orig = Product::create(num, Power::oneOver(denom));
    BasePtr result;

    result = orig->expand();

    BOOST_CHECK_EQUAL(orig, result);
}

BOOST_AUTO_TEST_SUITE_END()
