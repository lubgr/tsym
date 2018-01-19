
#include <cmath>
#include <limits>
#include "number.h"
#include "tsymtests.h"

using namespace tsym;

const double TOL = 1.e-10;

TEST_GROUP(Rational)
{
    const Number a = Number(2, 33);
    const Number b = Number(5, 67);
    const Number c = Number(43, 7);
};

TEST(Rational, defaultZero)
{
    const Number n;

    CHECK(n == 0);
    CHECK(n.isInt());
}

TEST(Rational, one)
{
    const Number n(1);

    CHECK(n.isOne());
    CHECK(n == 1);
    CHECK(1 == n);
}

TEST(Rational, isRationalNumber)
{
    CHECK(a.isRational());
}

TEST(Rational, initialCancelation)
{
    const Number frac(12, 3);

    CHECK_EQUAL(4, frac.numerator());
    CHECK_EQUAL(1, frac.denominator());

    CHECK(frac == 4);
}

TEST(Rational, negativeDenominator)
{
    const Number n(1, -2);

    CHECK_EQUAL(-1, n.numerator());
    CHECK_EQUAL(2, n.denominator());
}

TEST(Rational, zeroDenominator)
{
    CHECK_THROWS(std::overflow_error, Number(2, 0));
}

TEST(Rational, posDoubleToFraction)
{
    const Number frac = Number(0.125);

    CHECK(frac.isFrac());
    CHECK_EQUAL(Number(1, 8), frac);
}

TEST(Rational, zeroDoubleToFraction)
{
    const Number zero = Number(0.0);

    CHECK_EQUAL(0, zero);
}

TEST(Rational, negDoubleToFraction)
{
    const Number frac = Number(-0.125);

    CHECK(frac.isFrac());
    CHECK_EQUAL(Number(-1, 8), frac);
}

TEST(Rational, simpleFractionSum)
{
    const Number res = Number(1, 2) + Number(3, 8);

    CHECK(res.isFrac());
    CHECK_EQUAL(7, res.numerator());
    CHECK_EQUAL(8, res.denominator());
}

TEST(Rational, primeFractionSum)
{
    const Number res = Number(1, 7) + Number(1, 17);

    CHECK(res.isFrac());
    CHECK_EQUAL(24, res.numerator());
    CHECK_EQUAL(119, res.denominator());
}

TEST(Rational, sumWithOtherFraction)
{
    const Number res = a + b;

    CHECK_EQUAL(299, res.numerator());
    CHECK_EQUAL(2211, res.denominator());
}

TEST(Rational, sumWithDouble)
{
    const Number n(1.23456789);
    const Number res = a + n;

    CHECK(res.isDouble());

    DOUBLES_EQUAL(1.2951739506, res.toDouble(), TOL);
}

TEST(Rational, difference)
{
    const Number res = b - c;

    CHECK_EQUAL(-2846, res.numerator());
    CHECK_EQUAL(469, res.denominator());
}

TEST(Rational, zeroByIdentityDifference)
{
    const Number res = a - a;

    CHECK_EQUAL(0, res.numerator());
    CHECK_EQUAL(1, res.denominator());
}

TEST(Rational, simpleProduct)
{
    const Number minusTwo(-2);
    const Number half(1, 2);
    const Number res = minusTwo*half;

    CHECK_EQUAL(-1, res);
}

TEST(Rational, productWithOtherFraction)
{
    const Number res = a*b*c;

    CHECK_EQUAL(430, res.numerator());
    CHECK_EQUAL(15477, res.denominator());
}

TEST(Rational, productWithDouble)
{
    const Number n(1.23456789);
    const Number res = b*n;

    CHECK(n.isDouble());
    DOUBLES_EQUAL(0.09213193208955223, res.toDouble(), TOL);
}

TEST(Rational, divisionByOtherFraction)
{
    const Number res = a/b;

    CHECK_EQUAL(134, res.numerator());
    CHECK_EQUAL(165, res.denominator());
}

TEST(Rational, greaterThan)
{
    CHECK(c > a);
    CHECK(c >= a);
}

TEST(Rational, lessThan)
{
    CHECK(a < b);
    CHECK(a <= b);
}

TEST(Rational, lessThanForEqualFraction)
{
    const Number oneThird(1, 3);

    CHECK_FALSE((oneThird < oneThird));
    CHECK_FALSE((oneThird > oneThird));
}

TEST(Rational, absValue)
{
    const Number positive(2, 3);
    const Number negative(-2, 3);

    CHECK_EQUAL(positive, negative.abs());
    CHECK_EQUAL(positive, positive.abs());
}

TEST(Rational, zeroSign)
{
    const Number zero(0);

    CHECK_EQUAL(0, zero.sign());
}

TEST(Rational, positiveSign)
{
    const Number positive(4, 17);

    CHECK_EQUAL(1, positive.sign());
}

TEST(Rational, negativeSign)
{
    const Number negative(-4, 17);

    CHECK_EQUAL(-1, negative.sign());
}

TEST_GROUP(NumberPower)
{
    const Number zero = Number(0);
    const Number one = Number(1);
    const Number two = Number(2);
    const Number three = Number(3);
    const Number half = Number(1, 2);
    const Number third = Number(1, 3);
    const Number fourth = Number(1, 4);
};

TEST(NumberPower, exponentOne)
{
    Number res(half);

    res = res.toThe(one);

    CHECK_EQUAL(half, res);
}

TEST(NumberPower, baseOne)
{
    Number pow = one.toThe(fourth);
    CHECK_EQUAL(1, pow);

    pow = one.toThe(three);
    CHECK_EQUAL(1, pow);
}

TEST(NumberPower, baseMinusOne)
{
    const Number base(-1);
    Number res;

    res = base.toThe(2);
    CHECK_EQUAL(1, res);

    CHECK_THROWS(std::overflow_error, base.toThe(-half));
    CHECK_THROWS(std::overflow_error, base.toThe(half));
    CHECK_THROWS(std::overflow_error, base.toThe(third));
    CHECK_THROWS(std::overflow_error, base.toThe(Number(4, 3)));
    CHECK_THROWS(std::overflow_error, base.toThe(Number(3, 4)));
}

TEST(NumberPower, baseZero)
{
    const Number pow = zero.toThe(two);

    CHECK_EQUAL(0, pow);
}

TEST(NumberPower, exponentZero)
{
    Number res;

    res = three.toThe(zero);
    CHECK_EQUAL(1, res);

    res = Number(-0.12345).toThe(zero);
    CHECK_EQUAL(1, res);
}

TEST(NumberPower, invertInteger)
{
    Number res(three);

    res = res.toThe(-1);

    CHECK_EQUAL(third, res);
}

TEST(NumberPower, invertPositiveFraction)
{
    Number res(169, 25);

    res = res.toThe(-1);

    CHECK_EQUAL(Number(25, 169), res);
}

TEST(NumberPower, invertNegativeFraction)
{
    Number res(-1, 2);

    res = res.toThe(-1);

    CHECK_EQUAL(-2, res);
}

TEST(NumberPower, illegalPower)
{
    const Number zero(0);

    disableLog();
    CHECK_THROWS(std::overflow_error, zero.toThe(-1));
    enableLog();
}

TEST(NumberPower, intExponent)
{
    Number frac(23, 27);

    frac = frac.toThe(4);

    CHECK(frac.isFrac());
    CHECK_EQUAL(279841, frac.numerator());
    CHECK_EQUAL(531441, frac.denominator());
}

TEST(NumberPower, doubleExponent)
{
    Number n(0.12345);
    Number res(two);

    res = res.toThe(n);

    CHECK(res.isDouble());
    DOUBLES_EQUAL(1.0893367441616877, res.toDouble(), TOL);
}

TEST(NumberPower, doubleBase)
{
    Number n(1.23456789);

    n = n.toThe(three);

    CHECK(n.isDouble());
    DOUBLES_EQUAL(1.8816763717891543, n.toDouble(), TOL);
}

TEST(NumberPower, squareRoot)
{
    Number res(4);

    res = res.toThe(half);

    CHECK_EQUAL(2, res);
}

TEST(NumberPower, thirdRoot)
{
    Number res(8);

    res = res.toThe(third);

    CHECK_EQUAL(2, res);
}

TEST(NumberPower, largeFracRoot)
{
    Number res(268435456, 410338673);
    Number exp(1, 7);

    res = res.toThe(exp);

    CHECK(res.isFrac());
    CHECK_EQUAL(16, res.numerator());
    CHECK_EQUAL(17, res.denominator());
}

TEST(NumberPower, squareRootOfThree)
{
    Number res(3);

    res = res.toThe(half);

    CHECK(res.isDouble());
    DOUBLES_EQUAL(std::sqrt(3.0), res.toDouble(), TOL);
}

TEST(NumberPower, irrationalBaseFracRoot)
{
    Number res(12.3456789);
    Number expected(2.311204240824796);

    res = res.toThe(third);

    CHECK(res.isDouble());
    DOUBLES_EQUAL(2.311204240824796, res.toDouble(), TOL);
    CHECK_EQUAL(expected, res);
}

TEST(NumberPower, negBaseFractionExpOddDenominator)
{
    CHECK_THROWS(std::overflow_error, Number(-4).toThe(Number(2, 3)));
}

TEST(NumberPower, negBaseFractionExpEvenDenominator)
{
    CHECK_THROWS(std::overflow_error, Number(-4, 9).toThe(Number(3, 4)));
}

TEST(NumberPower, negativeBaseFractionExp)
{
    Number res(-8);

    CHECK_THROWS(std::overflow_error, res.toThe(Number(1, 3)));
}

TEST(NumberPower, negativeBaseIntExp)
    /* (-2)^5 = -32. */
{
    Number res(-2);

    res = res.toThe(5);

    CHECK_EQUAL(-32, res);
}

TEST(NumberPower, negativeBaseToPositive)
    /* (-5/7)^4 = 635/2401. */
{
    Number res(-5, 7);

    res = res.toThe(4);

    CHECK_EQUAL(Number(625, 2401), res);
}

TEST(NumberPower, negativeBaseToUndefined)
{
    const Number exp(-1, 8);

    CHECK_THROWS(std::overflow_error, Number(-13).toThe(exp));
}

TEST(NumberPower, positiveBaseToDouble)
    /* 5^(-1/8) -> numeric evaluation. */
{
    Number res(5);

    res = res.toThe(Number(-1, 8));

    DOUBLES_EQUAL(0.8177654339579425, res.toDouble(), TOL);
}

TEST(NumberPower, negativeBaseDoubleExp)
{
    CHECK_THROWS(std::overflow_error, Number(-2).toThe(1.234567));
}

TEST(NumberPower, resolvableNumSquareRoot)
{
    const Number four(4);
    const Number res = four.toThe(Number(1, 2));

    CHECK_EQUAL(2, res);
}

TEST_GROUP(Integer)
{
    const Number five = Number(5);
};

TEST(Integer, simpleAddition)
{
    const Number expected(7);
    const Number n1(2);
    const Number n2(5);
    Number result;

    result = n1 + n2;

    CHECK_EQUAL(expected, result);
}

TEST(Integer, simpleSubtraction)
{
    const Number expected(4);
    const Number n1(10);
    const Number n2(6);
    Number result;

    result = n1 - n2;

    CHECK_EQUAL(expected, result);
}

TEST(Integer, simpleComparison)
{
    const Number six(6);

    CHECK(five < six);
}

TEST(Integer, comparisonWithOtherInt)
{
    const Number six(6);

    CHECK(five < six);
    CHECK(six > five);
}

TEST(Integer, isRational)
{
    CHECK(five.isRational());
}

TEST(Integer, comparisonWithDouble)
{
    const Number n(5.123456);

    CHECK(five < n);
    CHECK(n > five);
}

TEST(Integer, absValue)
{
    const Number negative(-123);
    const Number positive(123);

    CHECK_EQUAL(positive, negative.abs());
    CHECK_EQUAL(positive, positive.abs());
}

TEST(Integer, maxAbsValue)
{
    const long maxLong = std::numeric_limits<long>::max();
    const long minLong = -maxLong;
    const Number expected((Int(maxLong)));
    const Number n((Int(minLong)));
    Number result;

    result = n.abs();

    CHECK_EQUAL(expected, result);
}

TEST(Integer, negativeSign)
{
    const Number negative(-20);

    CHECK_EQUAL(-1, negative.sign());
}

TEST(Integer, positiveSign)
{
    const Number positive(20);

    CHECK_EQUAL(1, positive.sign());
}

TEST(Integer, multiplication)
{
    Number n;

    n = five * five;

    CHECK_EQUAL(25, n);
}

TEST_GROUP(Double) {};

TEST(Double, setAndGet)
{
    const Number n(1.23456789);

    CHECK(n.isDouble());
    DOUBLES_EQUAL(1.23456789, n.toDouble(), TOL);

    /* The (incorrect) integer return value of a double number should be zero. */
    disableLog();
    CHECK_EQUAL(0, n.numerator());
    enableLog();
}

TEST(Double, isNotRational)
{
    const Number n(123.4567890);

    CHECK_FALSE(n.isRational());
}

TEST(Double, equality)
{
    const Number n1(1.23456789);
    const Number n2(1.23456789);

    CHECK(n1 == n2);
}

TEST(Double, absValue)
{
    const double value = 1.234567;
    const Number negative(-value);
    const Number positive(value);

    CHECK_EQUAL(positive, negative.abs());
    CHECK_EQUAL(positive, positive.abs());
}

TEST(Double, negativeSign)
{
    const Number neg(-9.87654321);

    CHECK_EQUAL(-1, neg.sign());
}

TEST(Double, positiveSign)
{
    const Number pos(9.87654321);

    CHECK_EQUAL(1, pos.sign());
}

TEST_GROUP(Operators)
{
    const Number two = Number(2);
    const Number ten = Number(10);
    Number res;

    void teardown()
    {
        res = Number();
    }
};

TEST(Operators, unaryPlus)
{
    res = +two;

    CHECK_EQUAL(two, res);
}

TEST(Operators, unaryMinus)
{
    res = -two;

    CHECK(res.isInt());
    CHECK_EQUAL(-2, res.numerator());
}

TEST(Operators, plusEqual)
{
    const Number twelve(12);

    res = two;
    res += ten;

    CHECK_EQUAL(twelve, res);
}

TEST(Operators, minusEqual)
{
    const Number minusEight(-8);

    res = two;
    res -= ten;

    CHECK_EQUAL(minusEight, res);
}

TEST(Operators, multiplyEqual)
{
    const Number twenty(20);

    res = ten;
    res *= two;

    CHECK_EQUAL(twenty, res);
}

TEST(Operators, divisionEqual)
{
    const Number five(5);

    res = ten;
    res /= two;

    CHECK_EQUAL(five, res);
}
