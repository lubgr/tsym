
#include <limits>
#include "cpputest.h"
#include "int.h"

using namespace tsym;

TEST_GROUP(Int)
{
    Int overflowed;

    void setup()
    {
        overflowed = 2*Int::max();
    }
};

TEST(Int, equality)
{
    const Int one(1);

    CHECK(one == 1);
    CHECK(1 == one);
}

TEST(Int, changeOfSign)
{
    const Int expected(213094234);
    const Int n(-213094234);
    Int result;

    result = n.abs();

    CHECK_EQUAL(expected, result);
}

TEST(Int, constructFromMaxLong)
{
    const long max = std::numeric_limits<long>::max();
    Int n(max);

    CHECK_EQUAL(max, n);
}

TEST(Int, constructFromMinLong)
{
    const long min = std::numeric_limits<long>::min();
    Int n;

    disableLog();
    n = Int(min);
    enableLog();

    CHECK(n.hasOverflowed());
}

TEST(Int, changeOfSignMinMax)
{
    const Int min(Int::min());
    const Int max(Int::max());
    Int result;

    result = min.abs();

    CHECK_EQUAL(max, result);
}

TEST(Int, powerWithNegativeExp)
{
    const Int base(5);
    Int result;

    disableLog();
    result = base.toThe(-3);
    enableLog();

    CHECK_EQUAL(0, result);
}

TEST(Int, powerWithZeroExp)
{
    const Int base(12345);
    Int result;

    result = base.toThe(0);

    CHECK_EQUAL(1, result);
}

TEST(Int, subtractionWithOverflowedRhs)
{
    Int res;

    disableLog();
    res = 5 - overflowed;
    enableLog();

    CHECK(res.hasOverflowed());
}

TEST(Int, overflowBySubtraction)
{
    Int res;

    res = Int::min() - 12345;

    CHECK(res.hasOverflowed());
}

TEST(Int, overflowBySubtractionNegativeRhs)
{
    const Int rhs(-9876543);
    Int res;

    res = Int::max() - rhs;

    CHECK(res.hasOverflowed());
}

TEST(Int, powerWithOverflowed)
{
    Int res;

    disableLog();
    res = Int(2).toThe(overflowed);
    enableLog();

    CHECK(res.hasOverflowed());
}

TEST(Int, signWithOverflowed)
{
    int res;

    disableLog();
    res = overflowed.sign();
    enableLog();

    CHECK_EQUAL(1, res);
}

TEST(Int, absWithOverflowed)
{
    Int res;

    disableLog();
    res = -overflowed.abs();
    enableLog();

    CHECK(res.hasOverflowed());
}

TEST(Int, moduloWithOverflowed)
{
    Int res;

    disableLog();
    res = overflowed % 2;
    enableLog();

    CHECK(res.hasOverflowed());
}

TEST(Int, divisonByZero)
{
    Int res;

    disableLog();
    res = Int(5)/0;
    enableLog();

    CHECK(res.hasOverflowed());
}

TEST(Int, incrementDecrementOperators)
{
    Int n(1);
    Int res;

    res = ++n;
    CHECK_EQUAL(2, res);

    res = n++;
    CHECK_EQUAL(2, res);
    CHECK_EQUAL(3, n);

    res = --n;
    CHECK_EQUAL(2, res);

    res = n--;
    CHECK_EQUAL(2, res);
    CHECK_EQUAL(1, n);
}

TEST(Int, unaryPlusOperator)
{
    const Int five(5);
    Int res;

    res = +five;

    CHECK_EQUAL(five, res);
}

TEST(Int, sign)
{
    const Int neg(-2134);
    const Int pos(2345);

    CHECK_EQUAL(-1, neg.sign());
    CHECK_EQUAL(1, pos.sign());
}

TEST(Int, toPrimitiveInt)
{
    const int orig = 12345;
    const Int n(orig);

    CHECK(n.fitsIntoInt());
    CHECK_EQUAL(orig, n.toInt());
}

TEST(Int, toPrimitiveIntFails)
{
    const int maxInt = std::numeric_limits<int>::max();
    const long orig = maxInt + 2349556l;
    const Int pos(orig);
    const Int neg(-orig);
    int maxRes;
    int minRes;

    disableLog();
    maxRes = pos.toInt();
    minRes = neg.toInt();
    enableLog();

    CHECK_EQUAL(maxInt, maxRes);
    CHECK_EQUAL(-maxInt, minRes);
}

TEST(Int, toPrimitiveLong)
{
    const long orig = std::numeric_limits<int>::max() + 1234556l;
    const Int n(orig);

    CHECK(!n.fitsIntoInt());
    CHECK(n.fitsIntoLong());
    CHECK_EQUAL(orig, n.toLong());
}

TEST(Int, toPrimitiveLongFails)
{
    long res;

    disableLog();
    res = overflowed.toLong();
    enableLog();

    CHECK_EQUAL(Int::max(), Int(res));
}

TEST(Int, comparisonOperators)
{
    const Int one(1);
    const Int mTwo(-2);
    const Int four(4);

    CHECK(one < four);
    CHECK(one <= four);
    CHECK(one > mTwo);
    CHECK(one >= mTwo);
}
