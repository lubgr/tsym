
#include <cmath>
#include "number.h"
#include "globals.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Globals)
{
    const Var a = Var("a");
    const Var b = Var("b");
    const Var c = Var("c");
    const Var d = Var("d");
    const Var e = Var("e");
    const Var f = Var("f");
    const Var half = Var(1, 2);
};

TEST(Globals, squareRoot)
{
    Var expected(a);
    Var res;

    res = sqrt(a);
    expected = pow(expected, half);

    CHECK_EQUAL(expected, res);
}

TEST(Globals, resolvableSquareRoot)
{
    Var res;

    res = tsym::sqrt(4);

    CHECK_EQUAL(2, res);
}

TEST(Globals, nonResolvableNumericSquareRoot)
{
    Var expected(5);
    Var res;

    res = tsym::sqrt(5);
    expected = pow(expected, half);

    CHECK_EQUAL(expected, res);
}

TEST(Globals, powerEulerBaseLogExp)
{
    const Var arg = 2*a*b*b*pi();
    const Var exp = log(arg);
    const Var result = tsym::pow(euler(), exp);

    CHECK_EQUAL(arg, result);
}

TEST(Globals, logOfE)
{
    const Var result = log(euler());

    CHECK_EQUAL(1, result);
}

TEST(Globals, logOfPowerWithBaseE)
{
    const Var exp = a + b + tsym::sqrt(2);
    const Var result = log(tsym::pow(euler(), exp));

    CHECK_EQUAL(exp, result);
}

TEST(Globals, logOfPower)
{
    const Var exp = tsym::sqrt(5)*a + 1/b;
    const Var power = tsym::pow(a, exp);
    const Var result = log(power);
    const Var expected = exp*log(a);

    CHECK_EQUAL(expected, result);
}

TEST(Globals, logOfZero)
{
    const Var zero(0);
    Var result;

    disableLog();
    result = tsym::log(zero);
    enableLog();

    CHECK_EQUAL(Var::Type::UNDEFINED, result.type());
}

TEST(Globals, sineZero)
{
    const Var zero;

    CHECK_EQUAL(zero, sin(zero));
}

TEST(Globals, sinePiOverSix)
{
    const Var expected(1, 2);
    const Var arg(pi()/6);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Globals, sinePiOverThree)
{
    const Var arg(pi()/3);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(tsym::sqrt(3)/2, res);
}

TEST(Globals, sineSevenPiOverFour)
    /* An angle of 315°. */
{
    const Var arg(7*pi()/4);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

TEST(Globals, cosFivePiOverFour)
    /* An angle of 225°. */
{
    const Var arg(5*pi()/4);
    Var res;

    res = cos(arg);

    CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

TEST(Globals, tanTwoThirdPi)
    /* An angle of 120°. */
{
    const Var arg(2*pi()/3);
    Var res;

    res = tan(arg);

    CHECK_EQUAL(-tsym::sqrt(3), res);
}

TEST(Globals, asinHalf)
{
    Var res;

    res = asin(half);

    CHECK_EQUAL(pi()/6, res);
}

TEST(Globals, acosMinusOneOverSqrtTwo)
{
    const Var arg(-1/tsym::sqrt(2));
    Var res;

    res = acos(arg);

    CHECK_EQUAL(3*pi()/4, res);
}

TEST(Globals, atanMinusOneOverSqrtThree)
{
    const Var arg(-1/tsym::sqrt(3));
    Var res;

    res = atan(arg);

    CHECK_EQUAL(-pi()/6, res);
}

TEST(Globals, successfulParsing)
{
    bool success;
    const Var expected = a*b*tsym::sqrt(2)*tan(a);
    const Var result(parse("a*b*sqrt(2)*tan(a)", &success));

    CHECK(success);
    CHECK_EQUAL(expected, result);
}

TEST(Globals, successfulParsingWithoutFlag)
{
    const Var expected = a*b + sin(a)*cos(b);
    const Var result(parse("a*b + sin(a)*cos(b)"));

    CHECK_EQUAL(expected, result);
}

TEST(Globals, parsingWithError)
{
    bool success;
    disableLog();
    parse("a*b*sqrt(2)*[[[tan(a)", &success);
    enableLog();

    CHECK_FALSE(success);
}

TEST(Globals, parsingEmptyString)
{
    bool success;
    disableLog();
    const Var result = parse("", &success);
    enableLog();

    CHECK_FALSE(success);
    CHECK_EQUAL(Var::Type::UNDEFINED, result.type());
}
