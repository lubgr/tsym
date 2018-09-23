
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/range/adaptors.hpp>
#include "fixtures.h"
#include "numeric.h"
#include "polyinfo.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct polyinfoFixture : public AbcFixture {
    const BasePtr abSum = Sum::create(a, b);
};

namespace {
    bool contains(const BasePtrList& symbolList, const Base& symbol)
    {
        using boost::adaptors::indirected;

        return boost::algorithm::any_of(
          symbolList | indirected, [&symbol](const auto& item) { return item.isEqual(symbol); });
    }
}

BOOST_FIXTURE_TEST_SUITE(Testpolyinfo, polyinfoFixture)

BOOST_AUTO_TEST_CASE(bothZero)
{
    BOOST_TEST(!poly::isInputValid(*zero, *zero));
}

BOOST_AUTO_TEST_CASE(invalidInputFirstArgUndefined)
{
    BOOST_TEST(!poly::isInputValid(*undefined, *a));
}

BOOST_AUTO_TEST_CASE(invalidInputSecondArgUndefined)
{
    BOOST_TEST(!poly::isInputValid(*a, *undefined));
}

BOOST_AUTO_TEST_CASE(validInputFraction)
{
    const auto frac = Numeric::create(1, 7);

    BOOST_TEST(poly::isInputValid(*a, *frac));
}

BOOST_AUTO_TEST_CASE(invalidInputSumWithNumericPower)
{
    const BasePtr numPow = Power::create(three, Numeric::create(1, 17));
    const BasePtr sum = Sum::create(a, numPow);

    BOOST_TEST(!poly::isInputValid(*sum, *b));
}

BOOST_AUTO_TEST_CASE(invalidInputPowerWithSymbolExp)
{
    const auto pow = Power::create(b, c);

    BOOST_TEST(!poly::isInputValid(*a, *pow));
}

BOOST_AUTO_TEST_CASE(invalidInputPowerWithValidSumExp)
{
    const auto exp = Sum::create(two, a);
    const auto pow = Power::create(b, exp);

    BOOST_TEST(!poly::isInputValid(*a, *pow));
}

BOOST_AUTO_TEST_CASE(validInputTwoInteger)
{
    BOOST_TEST(poly::isInputValid(*two, *eight));
}

BOOST_AUTO_TEST_CASE(validInputSumAndProduct)
{
    const BasePtr sum = Sum::create(two, a);
    const BasePtr product = Product::create(b, c);

    BOOST_TEST(poly::isInputValid(*sum, *product));
}

BOOST_AUTO_TEST_CASE(emptySymbolList)
{
    BOOST_TEST(poly::listOfSymbols(*three, *four).empty());
}

BOOST_AUTO_TEST_CASE(symbolListForOneSymbol)
{
    const BasePtrList expected{a};

    BOOST_TEST(expected == poly::listOfSymbols(*a, *a), per_element());
}

BOOST_AUTO_TEST_CASE(symbolListTwoSymbols)
{
    const BasePtrList expected{a, b};

    auto list = poly::listOfSymbols(*a, *b);

    BOOST_TEST(expected == list, per_element());
}

BOOST_AUTO_TEST_CASE(symbolListMultipleSymbols)
{
    const BasePtrList expected{a, b, c, d, e};
    const BasePtr pow1 = Power::create(d, three);
    const BasePtr sum1 = Sum::create(Product::create(two, a), b, pow1);
    const BasePtr pow2 = Power::create(sum1, five);
    const BasePtr sum2 = Sum::create(pow2, b, Power::create(c, two));
    const BasePtr product = Product::create({five, a, b, Sum::create(c, d), e, sum1});
    using boost::adaptors::indirected;
    const auto list = poly::listOfSymbols(*product, *sum2);

    BOOST_CHECK_EQUAL(expected.size(), list.size());

    for (const auto& symbol : expected | indirected)
        BOOST_TEST(contains(list, symbol));
}

BOOST_AUTO_TEST_CASE(simpleMainSymbol01)
{
    const BasePtr pow = Power::create(a, nine);
    const auto list = poly::listOfSymbols(*abSum, *pow);

    BOOST_CHECK_EQUAL(a, poly::mainSymbol(list, *abSum, *pow));
}

BOOST_AUTO_TEST_CASE(simpleMainSymbol02)
{
    const BasePtr power = Power::create(Sum::create(two, a), four);
    const auto list = poly::listOfSymbols(*abSum, *power);

    BOOST_CHECK_EQUAL(a, poly::mainSymbol(list, *abSum, *power));
}

BOOST_AUTO_TEST_CASE(failingMainSymbolRequest)
{
    const BasePtr product = Product::create(a, Power::create(b, two));
    const BasePtr sum = Sum::create(c, four);
    const auto list = poly::listOfSymbols(*product, *sum);

    BOOST_TEST(poly::mainSymbol(list, *product, *sum)->isUndefined());
}

BOOST_AUTO_TEST_CASE(mainSymbolOnlyOneCommon)
/* Main symbol of a*(b + c) and c*d^2: c. */
{
    const BasePtr arg1 = Product::create(a, Sum::create(b, c));
    const BasePtr arg2 = Product::create(c, Power::create(d, two));
    const auto list = poly::listOfSymbols(*arg1, *arg2);

    BOOST_CHECK_EQUAL(c, poly::mainSymbol(list, *arg1, *arg2));
}

BOOST_AUTO_TEST_CASE(mainSymbolBothZeroMinDegree)
/* Main symbol of 2 + a and 3 + a + b: a. */
{
    const BasePtr arg1 = Sum::create(two, a);
    const BasePtr arg2 = Sum::create(three, abSum);
    const auto list = poly::listOfSymbols(*arg1, *arg2);

    BOOST_CHECK_EQUAL(a, poly::mainSymbol(list, *arg1, *arg2));
}

BOOST_AUTO_TEST_CASE(mainSymbol)
/* Main symbol of a^3 + b^2 + c*d^4 and a^2*b*c^2: b. */
{
    const BasePtr arg1 =
      Sum::create({Power::create(a, three), Power::create(b, two), Product::create(c, Power::create(d, four))});
    const BasePtr arg2 = Product::create({Power::create(a, two), b, Power::create(c, two)});
    const auto list = poly::listOfSymbols(*arg1, *arg2);

    BOOST_CHECK_EQUAL(b, poly::mainSymbol(list, *arg1, *arg2));
}

BOOST_AUTO_TEST_CASE(mainSymbolLargeExpressions)
/* Main symbol of a^2*b^2*c^3 + b^3*c^4*d + d^3*e*f and a^4*b^5*d + e^3*f^3: d. */
{
    const BasePtr s1 = Product::create(Power::create(a, two), b, b, Power::create(c, three));
    const BasePtr s2 = Product::create(Power::create(b, three), Power::create(c, four), d);
    const BasePtr s3 = Product::create(Power::create(d, three), e, f);
    const BasePtr arg1 = Sum::create(s1, s2, s3);
    const BasePtr s4 = Product::create(Power::create(a, four), Power::create(b, five), d);
    const BasePtr s5 = Product::create(Power::create(e, three), Power::create(f, three));
    const BasePtr arg2 = Sum::create(s4, s5);
    const auto list = poly::listOfSymbols(*arg1, *arg2);

    BOOST_CHECK_EQUAL(d, poly::mainSymbol(list, *arg1, *arg2));
}

BOOST_AUTO_TEST_SUITE_END()
