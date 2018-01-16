
#include <limits>
#include <exception>
#include <sstream>
#include "int.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Int)
{
    const long maxLong = std::numeric_limits<long>::max();
    const int maxInt = std::numeric_limits<int>::max();
    const Int zero;
};

TEST(Int, defaultConstruction)
{
    CHECK_EQUAL(0, zero);
}

TEST(Int, equality)
{
    const Int one(1);

    CHECK(one == 1);
    CHECK(1 == one);
}

TEST(Int, initWithDoubleCeil)
{
    const Int n(12345.6789);

    CHECK_EQUAL(12345, n);
}

TEST(Int, initWithDoubleFloor)
{
    const Int n(98765.4321);

    CHECK_EQUAL(98765, n);
}

TEST(Int, changeOfSign)
{
    const Int expected(213094234);
    const Int n(-213094234);
    Int result;

    result = integer::abs(n);

    CHECK_EQUAL(expected, result);
}

TEST(Int, gcdBothOperandsZero)
{
    const Int result = integer::gcd(zero, zero);

    CHECK_EQUAL(0, result);
}

TEST(Int, trivialGcd)
{
    const Int four(4);
    const Int result = integer::gcd(four, 3);

    CHECK_EQUAL(1, result);
}

TEST(Int, largeGcd)
{
    const Int n1("2268768101928008863115135358527391507");
    const Int n2("471097608789240594631830432");
    const Int expected("2309482093840923");
    const Int result = integer::gcd(n1, n2);

    CHECK_EQUAL(expected, result);
}

TEST(Int, lcmBothZero)
{
    const Int result = integer::lcm(zero, zero);

    CHECK_EQUAL(0, result);
}

TEST(Int, largeLcm)
{
    const Int expected("47116200935874995263669584298436552275884098991145738935519645420");
    const Int n1("2309820438092849280938402209384209");
    const Int n2("20398209384092840982094382094380");
    const Int result = integer::lcm(n1, n2);

    CHECK_EQUAL(expected, result);
}

TEST(Int, constructFromMaxLong)
{
    Int n(maxLong);

    CHECK_EQUAL(maxLong, n);
}

TEST(Int, powerWithZeroExp)
{
    const Int base(12345);
    const Int result = integer::pow(base, 0);

    CHECK_EQUAL(1, result);
}

TEST(Int, powerZeroBaseAndExp)
{
    const Int result = integer::pow(zero, 0);

    CHECK_EQUAL(1, result);
}

TEST(Int, powerWithBaseOne)
{
    const Int base(1);
    const Int result = integer::pow(base, 843098208u);

    CHECK_EQUAL(1, result);
}

TEST(Int, power)
{
    const Int base(12345);
    const Int result = integer::pow(base, 9);

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
    CHECK(integer::fitsInto<int>(Int(1234567)));
    CHECK(integer::fitsInto<int>(Int(-7654321)));
    CHECK_FALSE(integer::fitsInto<int>(Int("230894203489028394082903849092340")));
    CHECK_FALSE(integer::fitsInto<int>(Int("-29304209843902894308290384203989")));
}

TEST(Int, fitsIntoLong)
{
    CHECK(integer::fitsInto<long>(Int(1234567)));
    CHECK(integer::fitsInto<long>(Int(-7654321)));
    CHECK_FALSE(integer::fitsInto<long>(Int("230894203489028394082903849092340")));
    CHECK_FALSE(integer::fitsInto<long>(Int("-29304209843902894308290384203989")));
}

TEST(Int, toPrimitiveInt)
{
    const int orig = 12345;
    const Int n(orig);

    CHECK(integer::fitsInto<int>(n));
    CHECK_EQUAL(orig, static_cast<int>(n));
}

TEST(Int, toDoubleLargeNumber)
{
    const Int n("2309420938209384092834902839408209420");

    DOUBLES_EQUAL(2.309420938209384e+36, static_cast<double>(n), 1e-8);
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

TEST(Int, streamOperator)
{
    const std::string expected("-12309812038209340942385930859034");
    const Int n(expected.c_str());
    std::stringstream stream;

    stream << n;

    CHECK_EQUAL(expected, stream.str());
}

TEST(Int, streamOperatorVeryLongNumber)
{
    const std::string expected("-1230029380928409280920984098230948099812038209340942385930859034"
            "203984092384092843092809483209482093840928430928309482093480923840928340928309402394"
            "029348092384092834092830942803482039482039482039482093840293840293840293840928430928");
    const Int n(expected.c_str());
    std::stringstream stream;

    stream << n;

    CHECK_EQUAL(expected, stream.str());
}
