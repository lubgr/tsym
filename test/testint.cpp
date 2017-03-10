
#include <limits>
#include <sstream>
#include "int.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Int)
{
    long maxLong;
    int maxInt;

    void setup()
    {
        maxInt = std::numeric_limits<int>::max();
        maxLong = std::numeric_limits<long>::max();
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
    Int n(maxLong);

    CHECK_EQUAL(maxLong, n);
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

TEST(Int, powerZeroBaseAndExp)
{
    const Int base(0);
    Int result;

    result = base.toThe(0);

    CHECK_EQUAL(1, result);
}

TEST(Int, powerWithBaseOne)
{
    const Int exp("20938092843098204982043023094");
    const Int base(1);
    Int result;

    result = base.toThe(exp);

    CHECK_EQUAL(1, result);
}

TEST(Int, power)
{
    const Int base(12345);
    Int result;

    result = base.toThe(9);

    CHECK_EQUAL(Int("6659166111488656281486807152009765625"), result);
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

TEST(Int, fitsIntoInt)
{
    CHECK(Int(1234567).fitsIntoInt());
    CHECK(Int(-7654321).fitsIntoInt());
    CHECK(!Int("230894203489028394082903849092340").fitsIntoInt());
    CHECK(!Int("-29304209843902894308290384203989").fitsIntoInt());
}

TEST(Int, fitsIntoLong)
{
    CHECK(Int(1234567).fitsIntoLong());
    CHECK(Int(-7654321).fitsIntoLong());
    CHECK(!Int("230894203489028394082903849092340").fitsIntoLong());
    CHECK(!Int("-29304209843902894308290384203989").fitsIntoLong());
}

TEST(Int, toPrimitiveInt)
{
    const int orig = 12345;
    const Int n(orig);

    CHECK(n.fitsIntoInt());
    CHECK_EQUAL(orig, n.toInt());
}

TEST(Int, toPrimitivePosIntFails)
{
    Int n(maxInt);
    int result;

    n += 2;

    disableLog();
    result = n.toInt();
    disableLog();

    CHECK_EQUAL(maxInt, result);
}

TEST(Int, toPrimitiveNegIntFails)
{
    const int minInt = std::numeric_limits<int>::min();
    Int n(minInt);
    int result;

    n *= 2;

    disableLog();
    result = n.toInt();
    disableLog();

    CHECK_EQUAL(minInt, result);
}

TEST(Int, toPrimitiveLong)
{
    Int n;

    if (maxInt < maxLong) {
        n = Int(maxInt + 123456l);
        CHECK(!n.fitsIntoInt());
        CHECK(n.fitsIntoLong());
    } else {
        n = Int(maxLong);
        CHECK(n.fitsIntoInt());
        CHECK(n.fitsIntoLong());
    }
}

TEST(Int, toPrimitiveLongFails)
{
    Int large("9238947298374892738942389470293809234094");
    long res;

    disableLog();
    res = large.toLong();
    enableLog();

    CHECK_EQUAL(maxLong, res);
}

TEST(Int, toPrimitiveNegLongFails)
{
    Int n("-2309482093489203482039842034");
    long res;

    disableLog();
    res = n.toLong();
    enableLog();

    CHECK_EQUAL(std::numeric_limits<long>::min(), res);
}

TEST(Int, toDoubleSmallNumber)
{
    const Int n(12345);

    DOUBLES_EQUAL(12345.0, n.toDouble(), 1e-8);
}

TEST(Int, toDoubleLargeNumber)
{
    const Int n("2309420938209384092834902839408209420");

    DOUBLES_EQUAL(2.309420938209384e+36, n.toDouble(), 1e-8);
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

TEST(Int, comparisonLargeNumbers)
{
    const Int n1("12092309420932039482093840920940293");
    const Int n2("9234823429340293409243092093840928");

    CHECK(n1 > n2);
    CHECK(n1 >= n2);
    CHECK(n2 < n1);
    CHECK(n2 <= n1);
}

TEST(Int, addition)
{
    const Int expected(123456l + 654321l);
    const Int n1(123456l);
    const Int n2(654321l);
    Int result;

    result = n1 + n2;

    CHECK_EQUAL(expected, result);
}

TEST(Int, multiplication)
{
    const Int expected(123*654);
    const Int n1(123);
    const Int n2(654);
    Int result;

    result = n1*n2;

    CHECK_EQUAL(expected, result);
}

TEST(Int, exactDivision)
{
    const Int numerator(45*123);
    const Int denominator(45);
    Int result;

    result = numerator/denominator;

    CHECK_EQUAL(123, result);
}

TEST(Int, divisionWithRemainder)
{
    const Int numerator(12345);
    const Int denominator(321);
    Int result;

    result = numerator/denominator;

    CHECK_EQUAL(38, result);
}

TEST(Int, moduloPositiveNumber)
{
    const Int n(5);

    CHECK_EQUAL(1, n % 2);
    CHECK_EQUAL(1, n % -2);
}

TEST(Int, moduloLargeNegativeNumber)
{
    const Int n("-2938749832749832749283749832749823");

    CHECK_EQUAL(-58, n % 123);
    CHECK_EQUAL(-58, n % -123);
}

TEST(Int, moduloZeroPosAndNegNumber)
{
    const Int n(164);

    CHECK_EQUAL(0, n % 2);
    CHECK_EQUAL(0, -n % 2);
    CHECK_EQUAL(0, n % -2);
    CHECK_EQUAL(0, -n % -2);
}

TEST(Int, illegalInputStr)
{
    disableLog();
    const Int n("illegal-dummy-string");
    enableLog();

    CHECK_EQUAL(0, n);
}

TEST(Int, streamOperator)
{
    const std::string expected("-12309812038209340942385930859034");
    const Int n(expected.c_str());
    std::stringstream stream;

    stream << n;

    CHECK_EQUAL(expected, stream.str());
}
