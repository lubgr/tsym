
#include <cmath>
#include <limits>
#include "number.h"
#include "tsymtests.h"

using namespace tsym;

const double TOL = 1.e-10;

TEST_GROUP(Rational)
{
    Number a;
    Number b;
    Number c;

    void setup()
    {
        a = Number(2, 33);
        b = Number(5, 67);
        c = Number(43, 7);
    }
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
    Number n;

    disableLog();
    n = Number(2, 0);
    enableLog();

    CHECK(n.isUndefined());
}

TEST(Rational, posDoubleToFraction)
{
    a = Number(0.125);

    CHECK(a.isFrac());
    CHECK_EQUAL(Number(1, 8), a);
}

TEST(Rational, zeroDoubleToFraction)
{
    a = Number(0.0);
    CHECK_EQUAL(0, a);
}

TEST(Rational, negDoubleToFraction)
{
    a = Number(-0.125);

    CHECK(a.isFrac());
    CHECK_EQUAL(Number(-1, 8), a);
}

TEST(Rational, simpleFractionSum)
{
    Number res;

    res = Number(1, 2) + Number(3, 8);

    CHECK(res.isFrac());
    CHECK_EQUAL(7, res.numerator());
    CHECK_EQUAL(8, res.denominator());
}

TEST(Rational, primeFractionSum)
{
    Number res;

    res = Number(1, 7) + Number(1, 17);

    CHECK(res.isFrac());
    CHECK_EQUAL(24, res.numerator());
    CHECK_EQUAL(119, res.denominator());
}

TEST(Rational, sumWithOtherFraction)
{
    Number res;

    res = a + b;

    CHECK_EQUAL(299, res.numerator());
    CHECK_EQUAL(2211, res.denominator());
}

TEST(Rational, sumWithDouble)
{
    const Number n(1.23456789);
    Number res;

    res = a + n;

    CHECK(res.isDouble());

    DOUBLES_EQUAL(1.2951739506, res.toDouble(), TOL);
}

TEST(Rational, difference)
{
    Number res;

    res = b - c;

    CHECK_EQUAL(-2846, res.numerator());
    CHECK_EQUAL(469, res.denominator());
}

TEST(Rational, zeroByIdentityDifference)
{
    Number res;

    res = a - a;

    CHECK_EQUAL(0, res.numerator());
    CHECK_EQUAL(1, res.denominator());
}

TEST(Rational, simpleProduct)
{
    const Number minusTwo(-2);
    const Number half(1, 2);
    Number res;

    res = minusTwo*half;

    CHECK_EQUAL(-1, res);
}

TEST(Rational, productWithOtherFraction)
{
    Number res;

    res = a*b*c;

    CHECK_EQUAL(430, res.numerator());
    CHECK_EQUAL(15477, res.denominator());
}

TEST(Rational, productWithDouble)
{
    const Number n(1.23456789);
    Number res;

    res = b*n;

    CHECK(n.isDouble());
    DOUBLES_EQUAL(0.09213193208955223, res.toDouble(), TOL);
}

TEST(Rational, divisionByOtherFraction)
{
    Number res;

    res = a/b;

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

    CHECK(!(oneThird < oneThird));
    CHECK(!(oneThird > oneThird));
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
    Number zero;
    Number one;
    Number two;
    Number three;

    Number half;
    Number third;
    Number fourth;

    void setup()
    {
        zero = Number(0);

        one = Number(1);
        two = Number(2);
        three = Number(3);

        half = Number(1, 2);
        third = Number(1, 3);
        fourth = Number(1, 4);
    }
};

TEST(NumberPower, exponentOne)
{
    Number res(half);

    res = res.toThe(one);

    CHECK_EQUAL(half, res);
}

TEST(NumberPower, baseOne)
{
    one = one.toThe(fourth);
    CHECK_EQUAL(1, one);

    one = one.toThe(three);
    CHECK_EQUAL(1, one);
}

TEST(NumberPower, baseMinusOne)
{
    Number res;

    res = Number::Pow(-1, 2);
    CHECK_EQUAL(1, res);

    res = Number::Pow(-1, -half);
    CHECK(res.isUndefined());

    res = Number::Pow(-1, half);
    CHECK(res.isUndefined());

    res = Number::Pow(-1, third);
    CHECK(res.isUndefined());

    res = Number::Pow(-1, Number(4, 3));
    CHECK(res.isUndefined());

    res = Number::Pow(-1, Number(3, 4));
    CHECK(res.isUndefined());
}

TEST(NumberPower, baseZero)
{
    zero = zero.toThe(two);

    CHECK_EQUAL(0, zero);
}

TEST(NumberPower, exponentZero)
{
    Number res;

    res = Number::Pow(three, zero);
    CHECK_EQUAL(1, res);

    res = Number::Pow(-0.12345, zero);
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

TEST(NumberPower, undefined)
{
    Number zero;

    disableLog();
    zero = zero.toThe(-1);
    enableLog();

    CHECK(zero.isUndefined());
    CHECK(!zero.isInt());
    CHECK(!zero.isDouble());
    CHECK(!zero.isFrac());

    CHECK(zero != 0);
    CHECK(zero != 1);
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
    /* (-4)^(2/3) is undefined. */
{
    Number res = Number::Pow(-4, Number(2, 3));

    CHECK(res.isUndefined());
}

TEST(NumberPower, negBaseFractionExpEvenDenominator)
    /* (-4/9)^(3/4) is undefined. */
{
    Number res = Number::Pow(Number(-4, 9), Number(3, 4));

    CHECK(res.isUndefined());
}

TEST(NumberPower, negativeBaseFractionExp)
    /* (-8)^(1/3) is undefined. */
{
    Number res(-8);

    res = res.toThe(Number(1, 3));

    CHECK(res.isUndefined());
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
    /* (-13)^(-1/8) = Undefined. */
{
    const Number exp(-1, 8);
    Number res(-13);

    res = res.toThe(exp);

    CHECK(res.isUndefined());
}

TEST(NumberPower, positiveBaseToDouble)
    /* 5^(-1/8) -> numeric evaluation. */
{
    Number res(5);

    res = res.toThe(Number(-1, 8));

    DOUBLES_EQUAL(0.8177654339579425, res.toDouble(), TOL);
}

TEST(NumberPower, negativeBaseDoubleExp)
    /* (-2)^1.234567 = Undefined. */
{
    Number res(-2);

    res = res.toThe(1.234567);

    CHECK(res.isUndefined());
}

TEST(NumberPower, staticSquareRoot)
{
    const Number expected(std::sqrt(2.0));
    Number res;

    res = Number::Sqrt(2);

    CHECK_EQUAL(expected, res);
}

TEST(NumberPower, staticResolvableNumSquareRoot)
{
    Number res;

    res = Number::Sqrt(4);

    CHECK_EQUAL(2, res);
}

TEST(NumberPower, staticPower)
{
    Number expected(2, 3);
    Number base(4, 9);
    Number res;

    res = Number::Pow(base, Number(3, 2));
    expected = expected.toThe(3);

    CHECK_EQUAL(expected, res);
}

TEST_GROUP(Integer)
{
    Number five;

    void setup()
    {
        five = Number(5);
    }
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

    CHECK(!n.isRational());
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

TEST_GROUP(UndefinedNumber)
{
    Number undefined;

    void setup()
    {
        undefined = Number::createUndefined();
    }
};

TEST(UndefinedNumber, typeRequests)
{
    CHECK(undefined.isUndefined());
    CHECK(!undefined.isZero());
    CHECK(!undefined.isOne());
}

TEST(UndefinedNumber, creationDivisionByZero)
{
    const Number zero;
    const Number n(123);
    Number res;

    disableLog();

    res = n/zero;
    CHECK(res.isUndefined());

    enableLog();
}

TEST(UndefinedNumber, sumWithValidNumber)
{
    const Number two(2);
    Number res;

    res = two + undefined;

    CHECK(res.isUndefined());
}

TEST(UndefinedNumber, differenceWithValidNumber)
{
    const Number valid(2, 3);
    Number res;

    res = valid - undefined;

    CHECK(res.isUndefined());
}

TEST(UndefinedNumber, productWithValidNumber)
{
    const Number four(4);
    Number res;

    res = four*undefined;

    CHECK(res.isUndefined());
}

TEST(UndefinedNumber, powerWithValidExponent)
{
    const Number exp(1, 2);

    undefined = undefined.toThe(exp);

    CHECK(undefined.isUndefined());
}

TEST(UndefinedNumber, powerWithValidBase)
{
    Number base(5, 2);

    base = base.toThe(undefined);

    CHECK(base.isUndefined());
}

TEST_GROUP(Operators)
{
    Number two;
    Number ten;
    Number res;

    void setup()
    {
        two = Number(2);
        ten = Number(10);
    }

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
