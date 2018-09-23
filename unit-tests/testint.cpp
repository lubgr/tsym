
#include <exception>
#include <limits>
#include <sstream>
#include "int.h"
#include "tsymtests.h"

using namespace tsym;

struct IntFixture {
    const Int zero;
};

BOOST_FIXTURE_TEST_SUITE(TestInt, IntFixture)

BOOST_AUTO_TEST_CASE(defaultConstruction)
{
    BOOST_CHECK_EQUAL(0, zero);
}

BOOST_AUTO_TEST_CASE(equality)
{
    const Int one(1);

    BOOST_TEST(one == 1);
    BOOST_TEST(1 == one);
}

BOOST_AUTO_TEST_CASE(initWithDoubleCeil)
{
    const Int n(12345.6789);

    BOOST_CHECK_EQUAL(12345, n);
}

BOOST_AUTO_TEST_CASE(initWithDoubleFloor)
{
    const Int n(98765.4321);

    BOOST_CHECK_EQUAL(98765, n);
}

BOOST_AUTO_TEST_CASE(changeOfSign)
{
    const Int expected(213094234);
    const Int n(-213094234);
    Int result;

    result = abs(n);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(gcdBothOperandsZero)
{
    const Int result = gcd(zero, zero);

    BOOST_CHECK_EQUAL(0, result);
}

BOOST_AUTO_TEST_CASE(trivialGcd)
{
    const Int four(4);
    const Int result = gcd(four, 3);

    BOOST_CHECK_EQUAL(1, result);
}

BOOST_AUTO_TEST_CASE(largeGcd)
{
    const Int n1("2268768101928008863115135358527391507");
    const Int n2("471097608789240594631830432");
    const Int expected("2309482093840923");
    const Int result = gcd(n1, n2);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(lcmBothZero)
{
    const Int result = lcm(zero, zero);

    BOOST_CHECK_EQUAL(0, result);
}

BOOST_AUTO_TEST_CASE(largeLcm)
{
    const Int expected("47116200935874995263669584298436552275884098991145738935519645420");
    const Int n1("2309820438092849280938402209384209");
    const Int n2("20398209384092840982094382094380");
    const Int result = lcm(n1, n2);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(constructFromMaxLong)
{
    const long maxLong = std::numeric_limits<long>::max();

    Int n(maxLong);

    BOOST_CHECK_EQUAL(maxLong, n);
}

BOOST_AUTO_TEST_CASE(powerWithZeroExp)
{
    const Int base(12345);
    const Int result = pow(base, 0);

    BOOST_CHECK_EQUAL(1, result);
}

BOOST_AUTO_TEST_CASE(powerZeroBaseAndExp)
{
    const Int result = pow(zero, 0);

    BOOST_CHECK_EQUAL(1, result);
}

BOOST_AUTO_TEST_CASE(powerWithBaseOne)
{
    const Int base(1);
    const Int result = pow(base, 843098208u);

    BOOST_CHECK_EQUAL(1, result);
}

BOOST_AUTO_TEST_CASE(power)
{
    const Int base(12345);
    const Int result = pow(base, 9);

    BOOST_CHECK_EQUAL(Int("6659166111488656281486807152009765625"), result);
}

BOOST_AUTO_TEST_CASE(incrementDecrementOperators)
{
    Int n(1);
    Int res;

    res = ++n;
    BOOST_CHECK_EQUAL(2, res);

    res = n++;
    BOOST_CHECK_EQUAL(2, res);
    BOOST_CHECK_EQUAL(3, n);

    res = --n;
    BOOST_CHECK_EQUAL(2, res);

    res = n--;
    BOOST_CHECK_EQUAL(2, res);
    BOOST_CHECK_EQUAL(1, n);
}

BOOST_AUTO_TEST_CASE(unaryPlusOperator)
{
    const Int five(5);
    Int res;

    res = +five;

    BOOST_CHECK_EQUAL(five, res);
}

BOOST_AUTO_TEST_CASE(sign)
{
    const Int neg(-2134);
    const Int pos(2345);

    BOOST_CHECK_EQUAL(-1, neg.sign());
    BOOST_CHECK_EQUAL(1, pos.sign());
}

BOOST_AUTO_TEST_CASE(fitsIntoInt)
{
    BOOST_TEST(fitsInto<int>(Int(1234567)));
    BOOST_TEST(fitsInto<int>(Int(-7654321)));
    BOOST_TEST(!fitsInto<int>(Int("230894203489028394082903849092340")));
    BOOST_TEST(!fitsInto<int>(Int("-29304209843902894308290384203989")));
}

BOOST_AUTO_TEST_CASE(fitsIntoLong)
{
    BOOST_TEST(fitsInto<long>(Int(1234567)));
    BOOST_TEST(fitsInto<long>(Int(-7654321)));
    BOOST_TEST(!fitsInto<long>(Int("230894203489028394082903849092340")));
    BOOST_TEST(!fitsInto<long>(Int("-29304209843902894308290384203989")));
}

BOOST_AUTO_TEST_CASE(toPrimitiveInt)
{
    const int orig = 12345;
    const Int n(orig);

    BOOST_TEST(fitsInto<int>(n));
    BOOST_CHECK_EQUAL(orig, static_cast<int>(n));
}

BOOST_AUTO_TEST_CASE(toDoubleLargeNumber)
{
    const Int n("2309420938209384092834902839408209420");

    BOOST_CHECK_CLOSE(2.309420938209384e+36, static_cast<double>(n), 1e-8);
}

BOOST_AUTO_TEST_CASE(comparisonOperators)
{
    const Int one(1);
    const Int mTwo(-2);
    const Int four(4);

    BOOST_TEST(one < four);
    BOOST_TEST(one <= four);
    BOOST_TEST(one > mTwo);
    BOOST_TEST(one >= mTwo);
}

BOOST_AUTO_TEST_CASE(comparisonLargeNumbers)
{
    const Int n1("12092309420932039482093840920940293");
    const Int n2("9234823429340293409243092093840928");

    BOOST_TEST(n1 > n2);
    BOOST_TEST(n1 >= n2);
    BOOST_TEST(n2 < n1);
    BOOST_TEST(n2 <= n1);
}

BOOST_AUTO_TEST_CASE(addition)
{
    const Int expected(123456l + 654321l);
    const Int n1(123456l);
    const Int n2(654321l);
    Int result;

    result = n1 + n2;

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(multiplication)
{
    const Int expected(123 * 654);
    const Int n1(123);
    const Int n2(654);
    Int result;

    result = n1 * n2;

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(exactDivision)
{
    const Int numerator(45 * 123);
    const Int denominator(45);
    Int result;

    result = numerator / denominator;

    BOOST_CHECK_EQUAL(123, result);
}

BOOST_AUTO_TEST_CASE(divisionWithRemainder)
{
    const Int numerator(12345);
    const Int denominator(321);
    Int result;

    result = numerator / denominator;

    BOOST_CHECK_EQUAL(38, result);
}

BOOST_AUTO_TEST_CASE(moduloPositiveNumber)
{
    const Int n(5);

    BOOST_CHECK_EQUAL(1, n % 2);
    BOOST_CHECK_EQUAL(1, n % -2);
}

BOOST_AUTO_TEST_CASE(moduloLargeNegativeNumber)
{
    const Int n("-2938749832749832749283749832749823");

    BOOST_CHECK_EQUAL(-58, n % 123);
    BOOST_CHECK_EQUAL(-58, n % -123);
}

BOOST_AUTO_TEST_CASE(moduloZeroPosAndNegNumber)
{
    const Int n(164);

    BOOST_CHECK_EQUAL(0, n % 2);
    BOOST_CHECK_EQUAL(0, -n % 2);
    BOOST_CHECK_EQUAL(0, n % -2);
    BOOST_CHECK_EQUAL(0, -n % -2);
}

BOOST_AUTO_TEST_CASE(streamOperator)
{
    const std::string expected("-12309812038209340942385930859034");
    const Int n(expected.c_str());
    std::stringstream stream;

    stream << n;

    BOOST_CHECK_EQUAL(expected, stream.str());
}

BOOST_AUTO_TEST_CASE(streamOperatorVeryLongNumber)
{
    const std::string expected("-1230029380928409280920984098230948099812038209340942385930859034"
                               "203984092384092843092809483209482093840928430928309482093480923840928340928309402394"
                               "029348092384092834092830942803482039482039482039482093840293840293840293840928430928");
    const Int n(expected.c_str());
    std::stringstream stream;

    stream << n;

    BOOST_CHECK_EQUAL(expected, stream.str());
}

BOOST_AUTO_TEST_SUITE_END()
