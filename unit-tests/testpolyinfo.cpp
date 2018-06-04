
#include <algorithm>
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

struct PolyInfoFixture : public AbcFixture {
    const BasePtr abSum = Sum::create(a, b);
    PolyInfo info{};
};

namespace {
    bool contains(const BasePtrList& symbolList, const BasePtr& symbol)
    {
        return std::any_of(
          cbegin(symbolList), cend(symbolList), [&symbol](const auto& item) { return item->isEqual(symbol); });
    }
}

BOOST_FIXTURE_TEST_SUITE(TestPolyInfo, PolyInfoFixture)

BOOST_AUTO_TEST_CASE(bothZero)
{
    info.set(zero, zero);

    BOOST_TEST(!info.isInputValid());
}

BOOST_AUTO_TEST_CASE(invalidInputFirstArgUndefined)
{
    info.set(undefined, a);

    BOOST_TEST(!info.isInputValid());
}

BOOST_AUTO_TEST_CASE(invalidInputSecondArgUndefined)
{
    info.set(a, undefined);

    BOOST_TEST(!info.isInputValid());
}

BOOST_AUTO_TEST_CASE(validInputFraction)
{
    info.set(a, Numeric::create(1, 7));

    BOOST_TEST(info.isInputValid());
}

BOOST_AUTO_TEST_CASE(invalidInputSumWithNumericPower)
{
    const BasePtr numPow = Power::create(three, Numeric::create(1, 17));
    const BasePtr sum = Sum::create(a, numPow);

    info.set(sum, b);

    BOOST_TEST(!info.isInputValid());
}

BOOST_AUTO_TEST_CASE(invalidInputPowerWithSymbolExp)
{
    info.set(a, Power::create(b, c));

    BOOST_TEST(!info.isInputValid());
}

BOOST_AUTO_TEST_CASE(invalidInputPowerWithValidSumExp)
{
    const BasePtr exp = Sum::create(two, a);

    info.set(a, Power::create(b, exp));

    BOOST_TEST(!info.isInputValid());
}

BOOST_AUTO_TEST_CASE(validInputTwoInteger)
{
    info.set(two, eight);

    BOOST_TEST(info.isInputValid());
}

BOOST_AUTO_TEST_CASE(validInputSumAndProduct)
{
    const BasePtr sum = Sum::create(two, a);
    const BasePtr product = Product::create(b, c);

    info.set(sum, product);

    BOOST_TEST(info.isInputValid());
}

BOOST_AUTO_TEST_CASE(emptySymbolList)
{
    info.set(three, four);

    BOOST_TEST(info.listOfSymbols().empty());
}

BOOST_AUTO_TEST_CASE(symbolListForOneSymbol)
{
    info.set(a, a);

    BOOST_CHECK_EQUAL(1, info.listOfSymbols().size());
    BOOST_TEST(contains(info.listOfSymbols(), a));
}

BOOST_AUTO_TEST_CASE(symbolListTwoSymbols)
{
    BasePtrList list;

    info.set(a, b);
    list = info.listOfSymbols();

    BOOST_CHECK_EQUAL(2, list.size());
    BOOST_TEST(contains(list, a));
    BOOST_TEST(contains(list, b));
}

BOOST_AUTO_TEST_CASE(symbolListMultipleSymbols)
{
    const BasePtr expected[] = {a, b, c, d, e};
    const size_t nSymbols = sizeof(expected) / sizeof(expected[0]);
    const BasePtr pow1 = Power::create(d, three);
    const BasePtr sum1 = Sum::create(Product::create(two, a), b, pow1);
    const BasePtr pow2 = Power::create(sum1, five);
    const BasePtr sum2 = Sum::create(pow2, b, Power::create(c, two));
    const BasePtr product = Product::create({five, a, b, Sum::create(c, d), e, sum1});
    BasePtrList list;

    info.set(product, sum2);

    list = info.listOfSymbols();

    BOOST_CHECK_EQUAL(nSymbols, list.size());

    for (const auto& symbol : expected)
        BOOST_TEST(contains(list, symbol));
}

BOOST_AUTO_TEST_CASE(simpleMainSymbol01)
{
    const BasePtr pow = Power::create(a, nine);

    info.set(abSum, pow);

    BOOST_CHECK_EQUAL(a, info.mainSymbol());
}

BOOST_AUTO_TEST_CASE(simpleMainSymbol02)
{
    const BasePtr power = Power::create(Sum::create(two, a), four);

    info.set(abSum, power);

    BOOST_CHECK_EQUAL(a, info.mainSymbol());
}

BOOST_AUTO_TEST_CASE(failingMainSymbolRequest)
{
    const BasePtr product = Product::create(a, Power::create(b, two));
    const BasePtr sum = Sum::create(c, four);

    info.set(product, sum);

    BOOST_TEST(info.mainSymbol()->isUndefined());
}

BOOST_AUTO_TEST_CASE(mainSymbolOnlyOneCommon)
/* Main symbol of a*(b + c) and c*d^2: c. */
{
    const BasePtr arg1 = Product::create(a, Sum::create(b, c));
    const BasePtr arg2 = Product::create(c, Power::create(d, two));

    info.set(arg1, arg2);

    BOOST_CHECK_EQUAL(c, info.mainSymbol());
}

BOOST_AUTO_TEST_CASE(mainSymbolBothZeroMinDegree)
/* Main symbol of 2 + a and 3 + a + b: a. */
{
    const BasePtr arg1 = Sum::create(two, a);
    const BasePtr arg2 = Sum::create(three, abSum);

    info.set(arg1, arg2);

    BOOST_CHECK_EQUAL(a, info.mainSymbol());
}

BOOST_AUTO_TEST_CASE(mainSymbol)
/* Main symbol of a^3 + b^2 + c*d^4 and a^2*b*c^2: b. */
{
    const BasePtr arg1 =
      Sum::create({Power::create(a, three), Power::create(b, two), Product::create(c, Power::create(d, four))});
    const BasePtr arg2 = Product::create({Power::create(a, two), b, Power::create(c, two)});

    info.set(arg1, arg2);

    BOOST_CHECK_EQUAL(b, info.mainSymbol());
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

    info.set(arg1, arg2);

    BOOST_CHECK_EQUAL(d, info.mainSymbol());
}

BOOST_AUTO_TEST_SUITE_END()
