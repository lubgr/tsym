
#include <cmath>
#include "globals.h"
#include "cpputest.h"

using namespace tsym;

TEST_GROUP(Functions)
{
    Var a;
    Var b;
    Number half;

    void setup()
    {
        a = Var("a");
        b = Var("b");
        half = Number(1, 2);
    }
};

TEST(Functions, squareRoot)
{
    Var expected(a);
    Var res;

    res = sqrt(a);
    expected = expected.toThe(half);

    CHECK_EQUAL(expected, res);
}

TEST(Functions, resolvableSquareRoot)
{
    Var res;

    res = sqrt(4);

    CHECK_EQUAL(2, res);
}

TEST(Functions, nonResolvableNumericSquareRoot)
{
    Var expected(5);
    Var res;

    res = tsym::sqrt(5);
    expected = expected.toThe(half);

    CHECK_EQUAL(expected, res);
}

TEST(Functions, power)
{
    Var expected(a);
    Var res;

    res = pow(a, b);

    expected = expected.toThe(b);

    CHECK_EQUAL(expected, res);
}

TEST(Functions, powerWithNumericExp)
{
    Var expected(a);
    Var res;

    res = pow(a, Var(1, 4));

    expected = expected.toThe(Number(1, 4));

    CHECK_EQUAL(expected, res);
}

TEST(Functions, powerEulerBaseLogExp)
{
    const Var arg = 2*a*b*b*Pi;
    const Var exp = log(arg);
    const Var result = pow(tsym::e, exp);

    CHECK_EQUAL(arg, result);
}

TEST(Functions, logOfE)
{
    const Var result = log(e);

    CHECK_EQUAL(1, result);
}

TEST(Functions, logOfPowerWithBaseE)
{
    const Var exp = a + b + tsym::sqrt(2);
    const Var result = log(pow(e, exp));

    CHECK_EQUAL(exp, result);
}

TEST(Functions, logOfPower)
{
    const Var exp = tsym::sqrt(5)*a + 1/b;
    const Var power = pow(a, exp);
    const Var result = log(power);
    const Var expected = exp*log(a);

    CHECK_EQUAL(expected, result);
}

TEST(Functions, logOfZero)
{
    Var result;

    disableLog();
    result = tsym::log(0);
    enableLog();

    CHECK_EQUAL("Undefined", result.type());
}

TEST(Functions, sineZero)
{
    const Var zero;

    CHECK_EQUAL(zero, sin(zero));
}

TEST(Functions, sinePiOverSix)
{
    const Var expected(1, 2);
    const Var arg(Pi/6);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Functions, sinePiOverThree)
{
    const Var arg(Pi/3);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(tsym::sqrt(3)/2, res);
}

TEST(Functions, sineSevenPiOverFour)
    /* An angle of 315°. */
{
    const Var arg(7*Pi/4);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

TEST(Functions, cosFivePiOverFour)
    /* An angle of 225°. */
{
    const Var arg(5*Pi/4);
    Var res;

    res = cos(arg);

    CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

TEST(Functions, tanTwoThirdPi)
    /* An angle of 120°. */
{
    const Var arg(2*Pi/3);
    Var res;

    res = tan(arg);

    CHECK_EQUAL(-tsym::sqrt(3), res);
}

TEST(Functions, asinHalf)
{
    Var res;

    res = asin(Number(1, 2));

    CHECK_EQUAL(Pi/6, res);
}

TEST(Functions, acosMinusOneOverSqrtTwo)
{
    const Var arg(-1/sqrt(2));
    Var res;

    res = acos(arg);

    CHECK_EQUAL(3*Pi/4, res);
}

TEST(Functions, atanMinusOneOverSqrtThree)
{
    const Var arg(-1/sqrt(3));
    Var res;

    res = atan(arg);

    CHECK_EQUAL(-Pi/6, res);
}
