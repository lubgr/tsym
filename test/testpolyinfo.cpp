
#include <algorithm>
#include "abc.h"
#include "polyinfo.h"
#include "sum.h"
#include "product.h"
#include "power.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(PolyInfo)
{
    const BasePtr undefined = Undefined::create();
    BasePtr abSum;
    PolyInfo info;

    void setup()
    {
        abSum = Sum::create(a, b);
    }

    bool contains(const BasePtrList& symbolList, const BasePtr& symbol)
    {
        for (const auto& symbolFromList : symbolList)
            if (symbolFromList->isEqual(symbol))
                return true;

        return false;
    }
};

TEST(PolyInfo, bothZero)
{
    info.set(zero, zero);

    CHECK_FALSE(info.isInputValid());
}

TEST(PolyInfo, invalidInputFirstArgUndefined)
{
    info.set(undefined, a);

    CHECK_FALSE(info.isInputValid());
}

TEST(PolyInfo, invalidInputSecondArgUndefined)
{
    info.set(a, undefined);

    CHECK_FALSE(info.isInputValid());
}

TEST(PolyInfo, validInputFraction)
{
    info.set(a, Numeric::create(1, 7));

    CHECK(info.isInputValid());
}

TEST(PolyInfo, invalidInputSumWithNumericPower)
{
    const BasePtr numPow = Power::create(three, Numeric::create(1, 17));
    const BasePtr sum = Sum::create(a, numPow);

    info.set(sum, b);

    CHECK_FALSE(info.isInputValid());
}

TEST(PolyInfo, invalidInputPowerWithSymbolExp)
{
    info.set(a, Power::create(b, c));

    CHECK_FALSE(info.isInputValid());
}

TEST(PolyInfo, invalidInputPowerWithValidSumExp)
{
    const BasePtr exp = Sum::create(two, a);

    info.set(a, Power::create(b, exp));

    CHECK_FALSE(info.isInputValid());
}

TEST(PolyInfo, validInputTwoInteger)
{
    info.set(two, eight);

    CHECK(info.isInputValid());
}

TEST(PolyInfo, validInputSumAndProduct)
{
    const BasePtr sum = Sum::create(two, a);
    const BasePtr product = Product::create(b, c);

    info.set(sum, product);

    CHECK(info.isInputValid());
}

TEST(PolyInfo, emptySymbolList)
{
    info.set(three, four);

    CHECK(info.listOfSymbols().empty());
}

TEST(PolyInfo, symbolListForOneSymbol)
{
    info.set(a, a);

    CHECK_EQUAL(1, info.listOfSymbols().size());
    CHECK(contains(info.listOfSymbols(), a));
}

TEST(PolyInfo, symbolListTwoSymbols)
{
    BasePtrList list;

    info.set(a, b);
    list = info.listOfSymbols();

    CHECK_EQUAL(2, list.size());
    CHECK(contains(list, a));
    CHECK(contains(list, b));
}

TEST(PolyInfo, symbolListMultipleSymbols)
{
    const BasePtr expected[] = { a, b, c, d, e };
    const size_t nSymbols = sizeof(expected)/sizeof(expected[0]);
    const BasePtr pow1 = Power::create(d, three);
    const BasePtr sum1 = Sum::create(Product::create(two, a), b, pow1);
    const BasePtr pow2 = Power::create(sum1, five);
    const BasePtr sum2 = Sum::create(pow2, b, Power::create(c, two));
    const BasePtr product = Product::create({ five, a, b, Sum::create(c, d), e, sum1 });
    BasePtrList list;

    info.set(product, sum2);

    list = info.listOfSymbols();

    CHECK_EQUAL(nSymbols, list.size());

    for (size_t i = 0; i < nSymbols; ++i)
        CHECK(contains(list, expected[i]));
}

TEST(PolyInfo, simpleMainSymbol01)
{
    const BasePtr pow = Power::create(a, nine);

    info.set(abSum, pow);

    CHECK_EQUAL(a, info.mainSymbol());
}

TEST(PolyInfo, simpleMainSymbol02)
{
    const BasePtr power = Power::create(Sum::create(two, a), four);

    info.set(abSum, power);

    CHECK_EQUAL(a, info.mainSymbol());
}

TEST(PolyInfo, failingMainSymbolRequest)
{
    const BasePtr product = Product::create(a, Power::create(b, two));
    const BasePtr sum = Sum::create(c, four);

    info.set(product, sum);

    CHECK(info.mainSymbol()->isUndefined());
}

TEST(PolyInfo, mainSymbolOnlyOneCommon)
    /* Main symbol of a*(b + c) and c*d^2: c. */
{
    const BasePtr arg1 = Product::create(a, Sum::create(b, c));
    const BasePtr arg2 = Product::create(c, Power::create(d, two));

    info.set(arg1, arg2);

    CHECK_EQUAL(c, info.mainSymbol());
}

TEST(PolyInfo, mainSymbolBothZeroMinDegree)
    /* Main symbol of 2 + a and 3 + a + b: a. */
{
    const BasePtr arg1 = Sum::create(two, a);
    const BasePtr arg2 = Sum::create(three, abSum);

    info.set(arg1, arg2);

    CHECK_EQUAL(a, info.mainSymbol());
}

TEST(PolyInfo, mainSymbol)
    /* Main symbol of a^3 + b^2 + c*d^4 and a^2*b*c^2: b. */
{
    const BasePtr arg1 = Sum::create({ Power::create(a, three), Power::create(b, two),
            Product::create(c, Power::create(d, four)) });
    const BasePtr arg2 = Product::create({ Power::create(a, two), b, Power::create(c, two) });

    info.set(arg1, arg2);

    CHECK_EQUAL(b, info.mainSymbol());
}

TEST(PolyInfo, mainSymbolLargeExpressions)
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

    CHECK_EQUAL(d, info.mainSymbol());
}
