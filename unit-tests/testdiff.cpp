
#include "basefct.h"
#include "constant.h"
#include "fixtures.h"
#include "logarithm.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

namespace {
    void checkWrongDiffToUndefined(const BasePtr& arg1, const BasePtr& arg2)
    {
        const BasePtr result = arg1->diff(*arg2);
        BOOST_TEST(isUndefined(*result));
    }
}

BOOST_FIXTURE_TEST_SUITE(TestDiff, AbcFixture)

BOOST_AUTO_TEST_CASE(numeric)
{
    const BasePtr num = Numeric::create(11, 17);
    const BasePtr result = num->diff(*a);

    BOOST_TEST(isZero(*result));
}

BOOST_AUTO_TEST_CASE(constant)
{
    const BasePtr result = pi->diff(*a);

    BOOST_TEST(isZero(*result));
}

BOOST_AUTO_TEST_CASE(undefinedWrtSymbol)
{
    const BasePtr result = undefined->diff(*a);

    BOOST_TEST(isUndefined(*result));
}

BOOST_AUTO_TEST_CASE(symbolWrtItself)
{
    const BasePtr result = a->diff(*a);

    BOOST_CHECK_EQUAL(one, result);
}

BOOST_AUTO_TEST_CASE(symbolWrtOtherSymbol)
{
    const BasePtr result = a->diff(*b);

    BOOST_TEST(isZero(*result));
}

BOOST_AUTO_TEST_CASE(wrongArguments, noLogs())
{
    const BasePtr product = Product::create(seven, b);
    const BasePtr sum = Sum::create(a, b);
    const BasePtr power = Power::sqrt(a);
    BasePtr result;

    checkWrongDiffToUndefined(sum, sum);
    checkWrongDiffToUndefined(a, pi);
    checkWrongDiffToUndefined(four, product);
    checkWrongDiffToUndefined(product, power);
}

BOOST_AUTO_TEST_CASE(powerWithPosIntExp)
{
    const BasePtr expected = Product::create(five, Power::create(a, four));
    const BasePtr pow = Power::create(a, five);
    const BasePtr result = pow->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(powerWithNegIntExp)
{
    const BasePtr expected = Product::create(Numeric::create(-1234), Power::create(a, Numeric::create(-1235)));
    const BasePtr pow = Power::create(a, Numeric::create(-1234));
    const BasePtr result = pow->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(powerWithPosRationalExp)
{
    const BasePtr expected = Product::create(Numeric::create(3, 7), Power::create(a, Numeric::create(-4, 7)));
    const BasePtr pow = Power::create(a, Numeric::create(3, 7));
    const BasePtr result = pow->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(powerOfEuler)
/* de^a/da = e^a. */
{
    const BasePtr pow = Power::create(Constant::createE(), a);
    const BasePtr result = pow->diff(*a);

    BOOST_CHECK_EQUAL(pow, result);
}

BOOST_AUTO_TEST_CASE(symbolPower)
{
    const BasePtr pow = Power::create(a, b);
    const BasePtr expected = Product::create(pow, b, Power::oneOver(a));
    const BasePtr result = pow->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(logOfSymbol)
{
    const BasePtr expected = Power::oneOver(a);
    const BasePtr log = Logarithm::create(a);
    const BasePtr result = log->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(logOfSymbolWrtToDifferentSymbol)
{
    const BasePtr log = Logarithm::create(a);
    const BasePtr result = log->diff(*b);

    BOOST_TEST(isZero(*result));
}

BOOST_AUTO_TEST_CASE(sinOfSum)
{
    const BasePtr sum = Sum::create(Product::create(Numeric::half(), a, a), b, Product::create(two, c));
    const BasePtr expected = Product::create(a, Trigonometric::createCos(sum));
    const BasePtr sin = Trigonometric::createSin(sum);
    const BasePtr result = sin->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(cosOfLogarithmOfProduct)
{
    const BasePtr arg = Logarithm::create(Product::create(four, a, b, c));
    const BasePtr expected = Product::minus(Trigonometric::createSin(arg), Power::oneOver(a));
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr result = cos->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(tanOfSinOfSymbol)
/* d/da(tan(sin(a))) = cos(a)*(1 + tan(sin(a))^2). */
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr tan = Trigonometric::createTan(sinA);
    const BasePtr expected = Product::create(Trigonometric::createCos(a), Sum::create(one, Power::create(tan, two)));
    const BasePtr result = tan->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(asinOfCosOfPower)
/* d/da(asin(cos(a^2))) = -2*a*sin(a^2)/sqrt(1 - cos(a^2)^2). */
{
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr cosASquare = Trigonometric::createCos(aSquare);
    const BasePtr asin = Trigonometric::createAsin(cosASquare);
    const BasePtr result = asin->diff(*a);
    const BasePtr expected = Product::minus(two, a, Trigonometric::createSin(aSquare),
      Power::create(Sum::create(one, Product::minus(cosASquare, cosASquare)), Numeric::create(-1, 2)));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(acosOfSum)
/* d/da(acos(a + 2*a^2 + c)) = (-4*a - 1)/sqrt(1 - (a + 2*a^2 + c)^2). */
{
    const BasePtr arg = Sum::create(a, Product::create(two, a, a), c);
    const BasePtr acos = Trigonometric::createAcos(arg);
    const BasePtr result = acos->diff(*a);
    const BasePtr expected = Product::create(Sum::create(Numeric::mOne(), Product::minus(four, a)),
      Power::create(Sum::create(one, Product::minus(Power::create(arg, two))), Numeric::create(-1, 2)));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(atanOfPower)
/* d/da(atan(a^10)) = 10*a^9/(1 + a^20). */
{
    const BasePtr atan = Trigonometric::createAtan(Power::create(a, ten));
    const BasePtr result = atan->diff(*a);
    const BasePtr expected = Product::create(
      ten, Power::create(a, nine), Power::oneOver(Sum::create(one, Power::create(a, Numeric::create(20)))));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2OfSymbols)
/* d/da(atan2(b, a)) = -b/(a^2 + b^2). */
{
    const BasePtr atan2 = Trigonometric::createAtan2(b, a);
    const BasePtr result = atan2->diff(*a);
    const BasePtr expected =
      Product::minus(b, Power::oneOver(Sum::create(Power::create(a, two), Power::create(b, two))));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(mixedTerm01)
/* d/da(2*a*b*log(4) + 17*a^5*b - 12/13*a^(-1/5) + a*log(a)) = * 1 + 12/65*a^(-6/5) + 85*a^4*b +
 * 2*b*log(4) + log(a). */
{
    const BasePtr expected = Sum::create(Sum::create(one, Logarithm::create(a)),
      Product::create(Numeric::create(12, 65), Power::create(a, Numeric::create(-6, 5))),
      Product::create(Numeric::create(85), b, Power::create(a, four)),
      Product::create(two, b, Logarithm::create(four)));
    const BasePtr term1 = Product::create(two, a, b, Logarithm::create(four));
    const BasePtr term2 = Product::create(Numeric::create(17), Power::create(a, five), b);
    const BasePtr term3 = Product::create(Numeric::create(-12, 13), Power::create(a, Numeric::create(-1, 5)));
    const BasePtr term4 = Product::create(a, Logarithm::create(a));
    const BasePtr sum = Sum::create(term1, term2, term3, term4);
    const BasePtr result = sum->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(mixedTerm02)
/* d/da(a*b*c*log(sin(sqrt(2)*a^4*b + log(sin(2)*a)))) = ... */
{
    const BasePtr sinTwo = Trigonometric::createSin(two);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr aToTheFour = Power::create(a, four);
    const BasePtr abc = Product::create(a, b, c);
    const BasePtr sinArg =
      Sum::create(Product::create(sqrtTwo, aToTheFour, b), Logarithm::create(Product::create(sinTwo, a)));
    const BasePtr logArg = Trigonometric::createSin(sinArg);
    const BasePtr orig = Product::create(abc, Logarithm::create(logArg));
    const BasePtr bTimesCos = Product::create(b, Trigonometric::createCos(sinArg));
    const BasePtr numSummand1 = Trigonometric::createCos(sinArg);
    const BasePtr numSummand2 = Product::create(four, sqrtTwo, aToTheFour, bTimesCos);
    const BasePtr expectedNum = Product::create(b, c, Sum::create(numSummand1, numSummand2));
    const BasePtr expectedDenom = Trigonometric::createSin(sinArg);
    const BasePtr summand1 = Product::create(expectedNum, Power::oneOver(expectedDenom));
    const BasePtr summand2 = Product::create(b, c, Logarithm::create(logArg));
    const BasePtr expected = Sum::create(summand1, summand2);
    const BasePtr result = orig->diff(*a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_SUITE_END()
