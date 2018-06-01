
#include <cmath>
#include <limits>
#include "fixtures.h"
#include "number.h"
#include "tsymtests.h"

using namespace tsym;

const double TOL = 1.e-8;

struct RationalFixture {
    const Number a{2, 33};
    const Number b{5, 67};
    const Number c{43, 7};
};

BOOST_FIXTURE_TEST_SUITE(TestRational, RationalFixture)

BOOST_AUTO_TEST_CASE(defaultZero)
{
    const Number n;

    BOOST_TEST(n == 0);
    BOOST_TEST(n.isInt());
}

BOOST_AUTO_TEST_CASE(one)
{
    const Number n(1);

    BOOST_TEST(n.isOne());
    BOOST_TEST(n == 1);
    BOOST_TEST(1 == n);
}

BOOST_AUTO_TEST_CASE(isRationalNumber)
{
    BOOST_TEST(a.isRational());
}

BOOST_AUTO_TEST_CASE(initialCancelation)
{
    const Number frac(12, 3);

    BOOST_CHECK_EQUAL(4, frac.numerator());
    BOOST_CHECK_EQUAL(1, frac.denominator());

    BOOST_TEST(frac == 4);
}

BOOST_AUTO_TEST_CASE(negativeDenominator)
{
    const Number n(1, -2);

    BOOST_CHECK_EQUAL(-1, n.numerator());
    BOOST_CHECK_EQUAL(2, n.denominator());
}

BOOST_AUTO_TEST_CASE(posDoubleToFraction)
{
    const Number frac = Number(0.125);

    BOOST_TEST(frac.isFrac());
    BOOST_CHECK_EQUAL(Number(1, 8), frac);
}

BOOST_AUTO_TEST_CASE(zeroDoubleToFraction)
{
    const Number zero = Number(0.0);

    BOOST_CHECK_EQUAL(0, zero);
}

BOOST_AUTO_TEST_CASE(negDoubleToFraction)
{
    const Number frac = Number(-0.125);

    BOOST_TEST(frac.isFrac());
    BOOST_CHECK_EQUAL(Number(-1, 8), frac);
}

BOOST_AUTO_TEST_CASE(simpleFractionSum)
{
    const Number res = Number(1, 2) + Number(3, 8);

    BOOST_TEST(res.isFrac());
    BOOST_CHECK_EQUAL(7, res.numerator());
    BOOST_CHECK_EQUAL(8, res.denominator());
}

BOOST_AUTO_TEST_CASE(primeFractionSum)
{
    const Number res = Number(1, 7) + Number(1, 17);

    BOOST_TEST(res.isFrac());
    BOOST_CHECK_EQUAL(24, res.numerator());
    BOOST_CHECK_EQUAL(119, res.denominator());
}

BOOST_AUTO_TEST_CASE(sumWithOtherFraction)
{
    const Number res = a + b;

    BOOST_CHECK_EQUAL(299, res.numerator());
    BOOST_CHECK_EQUAL(2211, res.denominator());
}

BOOST_AUTO_TEST_CASE(sumWithDouble)
{
    const Number n(1.23456789);
    const Number res = a + n;

    BOOST_TEST(res.isDouble());

    BOOST_CHECK_CLOSE(1.2951739506, res.toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(difference)
{
    const Number res = b - c;

    BOOST_CHECK_EQUAL(-2846, res.numerator());
    BOOST_CHECK_EQUAL(469, res.denominator());
}

BOOST_AUTO_TEST_CASE(zeroByIdentityDifference)
{
    const Number res = a - a;

    BOOST_CHECK_EQUAL(0, res.numerator());
    BOOST_CHECK_EQUAL(1, res.denominator());
}

BOOST_AUTO_TEST_CASE(simpleProduct)
{
    const Number minusTwo(-2);
    const Number half(1, 2);
    const Number res = minusTwo * half;

    BOOST_CHECK_EQUAL(-1, res);
}

BOOST_AUTO_TEST_CASE(productWithOtherFraction)
{
    const Number res = a * b * c;

    BOOST_CHECK_EQUAL(430, res.numerator());
    BOOST_CHECK_EQUAL(15477, res.denominator());
}

BOOST_AUTO_TEST_CASE(productWithDouble)
{
    const Number n(1.23456789);
    const Number res = b * n;

    BOOST_TEST(n.isDouble());
    BOOST_CHECK_CLOSE(0.09213193208955223, res.toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(divisionByOtherFraction)
{
    const Number res = a / b;

    BOOST_CHECK_EQUAL(134, res.numerator());
    BOOST_CHECK_EQUAL(165, res.denominator());
}

BOOST_AUTO_TEST_CASE(greaterThan)
{
    BOOST_TEST(c > a);
    BOOST_TEST(c >= a);
}

BOOST_AUTO_TEST_CASE(lessThan)
{
    BOOST_TEST(a < b);
    BOOST_TEST(a <= b);
}

BOOST_AUTO_TEST_CASE(lessThanForEqualFraction)
{
    const Number oneThird(1, 3);

    BOOST_TEST(!(oneThird < oneThird));
    BOOST_TEST(!(oneThird > oneThird));
}

BOOST_AUTO_TEST_CASE(absValue)
{
    const Number positive(2, 3);
    const Number negative(-2, 3);

    BOOST_CHECK_EQUAL(positive, negative.abs());
    BOOST_CHECK_EQUAL(positive, positive.abs());
}

BOOST_AUTO_TEST_CASE(zeroSign)
{
    const Number zero(0);

    BOOST_CHECK_EQUAL(0, zero.sign());
}

BOOST_AUTO_TEST_CASE(positiveSign)
{
    const Number positive(4, 17);

    BOOST_CHECK_EQUAL(1, positive.sign());
}

BOOST_AUTO_TEST_CASE(negativeSign)
{
    const Number negative(-4, 17);

    BOOST_CHECK_EQUAL(-1, negative.sign());
}

BOOST_AUTO_TEST_SUITE_END()

struct NumberPowerFixture {
    const Number zero = Number(0);
    const Number one = Number(1);
    const Number two = Number(2);
    const Number three = Number(3);
    const Number half = Number(1, 2);
    const Number third = Number(1, 3);
    const Number fourth = Number(1, 4);
};

BOOST_FIXTURE_TEST_SUITE(TestNumberPower, NumberPowerFixture)

BOOST_AUTO_TEST_CASE(exponentOne)
{
    Number res(half);

    res = res.toThe(one);

    BOOST_CHECK_EQUAL(half, res);
}

BOOST_AUTO_TEST_CASE(baseOne)
{
    Number pow = one.toThe(fourth);
    BOOST_CHECK_EQUAL(1, pow);

    pow = one.toThe(three);
    BOOST_CHECK_EQUAL(1, pow);
}

BOOST_AUTO_TEST_CASE(baseMinusOne)
{
    const Number base(-1);
    Number res;

    res = base.toThe(2);
    BOOST_CHECK_EQUAL(1, res);

    BOOST_CHECK_THROW(base.toThe(-half), std::overflow_error);
    BOOST_CHECK_THROW(base.toThe(half), std::overflow_error);
    BOOST_CHECK_THROW(base.toThe(third), std::overflow_error);
    BOOST_CHECK_THROW(base.toThe(Number(4, 3)), std::overflow_error);
    BOOST_CHECK_THROW(base.toThe(Number(3, 4)), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(baseZero)
{
    const Number pow = zero.toThe(two);

    BOOST_CHECK_EQUAL(0, pow);
}

BOOST_AUTO_TEST_CASE(exponentZero)
{
    Number res;

    res = three.toThe(zero);
    BOOST_CHECK_EQUAL(1, res);

    res = Number(-0.12345).toThe(zero);
    BOOST_CHECK_EQUAL(1, res);
}

BOOST_AUTO_TEST_CASE(invertInteger)
{
    Number res(three);

    res = res.toThe(-1);

    BOOST_CHECK_EQUAL(third, res);
}

BOOST_AUTO_TEST_CASE(invertPositiveFraction)
{
    Number res(169, 25);

    res = res.toThe(-1);

    BOOST_CHECK_EQUAL(Number(25, 169), res);
}

BOOST_AUTO_TEST_CASE(invertNegativeFraction)
{
    Number res(-1, 2);

    res = res.toThe(-1);

    BOOST_CHECK_EQUAL(-2, res);
}

BOOST_AUTO_TEST_CASE(illegalPower, noLogs())
{
    const Number zero(0);

    BOOST_CHECK_THROW(zero.toThe(-1), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(intExponent)
{
    Number frac(23, 27);

    frac = frac.toThe(4);

    BOOST_TEST(frac.isFrac());
    BOOST_CHECK_EQUAL(279841, frac.numerator());
    BOOST_CHECK_EQUAL(531441, frac.denominator());
}

BOOST_AUTO_TEST_CASE(doubleExponent)
{
    Number n(0.12345);
    Number res(two);

    res = res.toThe(n);

    BOOST_TEST(res.isDouble());
    BOOST_CHECK_CLOSE(1.0893367441616877, res.toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(doubleBase)
{
    Number n(1.23456789);

    n = n.toThe(three);

    BOOST_TEST(n.isDouble());
    BOOST_CHECK_CLOSE(1.8816763717891543, n.toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(squareRoot)
{
    Number res(4);

    res = res.toThe(half);

    BOOST_CHECK_EQUAL(2, res);
}

BOOST_AUTO_TEST_CASE(thirdRoot)
{
    Number res(8);

    res = res.toThe(third);

    BOOST_CHECK_EQUAL(2, res);
}

BOOST_AUTO_TEST_CASE(largeFracRoot)
{
    Number res(268435456, 410338673);
    Number exp(1, 7);

    res = res.toThe(exp);

    BOOST_TEST(res.isFrac());
    BOOST_CHECK_EQUAL(16, res.numerator());
    BOOST_CHECK_EQUAL(17, res.denominator());
}

BOOST_AUTO_TEST_CASE(squareRootOfThree)
{
    Number res(3);

    res = res.toThe(half);

    BOOST_TEST(res.isDouble());
    BOOST_CHECK_CLOSE(std::sqrt(3.0), res.toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(irrationalBaseFracRoot)
{
    Number res(12.3456789);
    Number expected(2.311204240824796);

    res = res.toThe(third);

    BOOST_TEST(res.isDouble());
    BOOST_CHECK_CLOSE(2.311204240824796, res.toDouble(), TOL);
    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(negBaseFractionExpOddDenominator)
{
    BOOST_CHECK_THROW(Number(-4).toThe(Number(2, 3)), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(negBaseFractionExpEvenDenominator)
{
    BOOST_CHECK_THROW(Number(-4, 9).toThe(Number(3, 4)), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(negativeBaseFractionExp)
{
    Number res(-8);

    BOOST_CHECK_THROW(res.toThe(Number(1, 3)), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(negativeBaseIntExp)
/* (-2)^5 = -32. */
{
    Number res(-2);

    res = res.toThe(5);

    BOOST_CHECK_EQUAL(-32, res);
}

BOOST_AUTO_TEST_CASE(negativeBaseToPositive)
/* (-5/7)^4 = 635/2401. */
{
    Number res(-5, 7);

    res = res.toThe(4);

    BOOST_CHECK_EQUAL(Number(625, 2401), res);
}

BOOST_AUTO_TEST_CASE(negativeBaseToUndefined)
{
    const Number exp(-1, 8);

    BOOST_CHECK_THROW(Number(-13).toThe(exp), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(positiveBaseToDouble)
/* 5^(-1/8) -> numeric evaluation. */
{
    Number res(5);

    res = res.toThe(Number(-1, 8));

    BOOST_CHECK_CLOSE(0.8177654339579425, res.toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(negativeBaseDoubleExp)
{
    BOOST_CHECK_THROW(Number(-2).toThe(1.234567), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(resolvableNumSquareRoot)
{
    const Number four(4);
    const Number res = four.toThe(Number(1, 2));

    BOOST_CHECK_EQUAL(2, res);
}

BOOST_AUTO_TEST_SUITE_END()

struct IntegerFixture {
    const Number five = Number(5);
};

BOOST_FIXTURE_TEST_SUITE(TestInteger, IntegerFixture)

BOOST_AUTO_TEST_CASE(simpleAddition)
{
    const Number expected(7);
    const Number n1(2);
    const Number n2(5);
    Number result;

    result = n1 + n2;

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleSubtraction)
{
    const Number expected(4);
    const Number n1(10);
    const Number n2(6);
    Number result;

    result = n1 - n2;

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleComparison)
{
    const Number six(6);

    BOOST_TEST(five < six);
}

BOOST_AUTO_TEST_CASE(comparisonWithOtherInt)
{
    const Number six(6);

    BOOST_TEST(five < six);
    BOOST_TEST(six > five);
}

BOOST_AUTO_TEST_CASE(isRational)
{
    BOOST_TEST(five.isRational());
}

BOOST_AUTO_TEST_CASE(comparisonWithDouble)
{
    const Number n(5.123456);

    BOOST_TEST(five < n);
    BOOST_TEST(n > five);
}

BOOST_AUTO_TEST_CASE(absValue)
{
    const Number negative(-123);
    const Number positive(123);

    BOOST_CHECK_EQUAL(positive, negative.abs());
    BOOST_CHECK_EQUAL(positive, positive.abs());
}

BOOST_AUTO_TEST_CASE(maxAbsValue)
{
    const long maxLong = std::numeric_limits<long>::max();
    const long minLong = -maxLong;
    const Number expected((Int(maxLong)));
    const Number n((Int(minLong)));
    Number result;

    result = n.abs();

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(negativeSign)
{
    const Number negative(-20);

    BOOST_CHECK_EQUAL(-1, negative.sign());
}

BOOST_AUTO_TEST_CASE(positiveSign)
{
    const Number positive(20);

    BOOST_CHECK_EQUAL(1, positive.sign());
}

BOOST_AUTO_TEST_CASE(multiplication)
{
    Number n;

    n = five * five;

    BOOST_CHECK_EQUAL(25, n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(TestDouble)

BOOST_AUTO_TEST_CASE(setAndGet, noLogs())
{
    const Number n(1.23456789);

    BOOST_TEST(n.isDouble());
    BOOST_CHECK_CLOSE(1.23456789, n.toDouble(), TOL);

    /* The (incorrect) integer return value of a double number should be zero. */
    BOOST_CHECK_EQUAL(0, n.numerator());
}

BOOST_AUTO_TEST_CASE(isNotRational)
{
    const Number n(123.4567890);

    BOOST_TEST(!n.isRational());
}

BOOST_AUTO_TEST_CASE(equality)
{
    const Number n1(1.23456789);
    const Number n2(1.23456789);

    BOOST_TEST(n1 == n2);
}

BOOST_AUTO_TEST_CASE(absValue)
{
    const double value = 1.234567;
    const Number negative(-value);
    const Number positive(value);

    BOOST_CHECK_EQUAL(positive, negative.abs());
    BOOST_CHECK_EQUAL(positive, positive.abs());
}

BOOST_AUTO_TEST_CASE(negativeSign)
{
    const Number neg(-9.87654321);

    BOOST_CHECK_EQUAL(-1, neg.sign());
}

BOOST_AUTO_TEST_CASE(positiveSign)
{
    const Number pos(9.87654321);

    BOOST_CHECK_EQUAL(1, pos.sign());
}

BOOST_AUTO_TEST_SUITE_END()

struct OperatorFixture {
    const Number two = Number(2);
    const Number ten = Number(10);
    Number res{};
};

BOOST_FIXTURE_TEST_SUITE(TestOperators, OperatorFixture)

BOOST_AUTO_TEST_CASE(unaryPlus)
{
    res = +two;

    BOOST_CHECK_EQUAL(two, res);
}

BOOST_AUTO_TEST_CASE(unaryMinus)
{
    res = -two;

    BOOST_TEST(res.isInt());
    BOOST_CHECK_EQUAL(-2, res.numerator());
}

BOOST_AUTO_TEST_CASE(plusEqual)
{
    const Number twelve(12);

    res = two;
    res += ten;

    BOOST_CHECK_EQUAL(twelve, res);
}

BOOST_AUTO_TEST_CASE(minusEqual)
{
    const Number minusEight(-8);

    res = two;
    res -= ten;

    BOOST_CHECK_EQUAL(minusEight, res);
}

BOOST_AUTO_TEST_CASE(multiplyEqual)
{
    const Number twenty(20);

    res = ten;
    res *= two;

    BOOST_CHECK_EQUAL(twenty, res);
}

BOOST_AUTO_TEST_CASE(divisionEqual)
{
    const Number five(5);

    res = ten;
    res /= two;

    BOOST_CHECK_EQUAL(five, res);
}

BOOST_AUTO_TEST_SUITE_END()
