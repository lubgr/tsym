
#include <cmath>
#include "number.h"
#include "globals.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

struct GlobalsFixture {
    const Var a = Var("a");
    const Var b = Var("b");
    const Var c = Var("c");
    const Var d = Var("d");
    const Var e = Var("e");
    const Var f = Var("f");
    const Var half = Var(1, 2);
};

BOOST_FIXTURE_TEST_SUITE(TestGlobals, GlobalsFixture)

BOOST_AUTO_TEST_CASE(squareRoot)
{
    Var expected(a);
    Var res;

    res = sqrt(a);
    expected = pow(expected, half);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(resolvableSquareRoot)
{
    Var res;

    res = tsym::sqrt(4);

    BOOST_CHECK_EQUAL(2, res);
}

BOOST_AUTO_TEST_CASE(nonResolvableNumericSquareRoot)
{
    Var expected(5);
    Var res;

    res = tsym::sqrt(5);
    expected = pow(expected, half);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(powerEulerBaseLogExp)
{
    const Var arg = 2*a*b*b*pi();
    const Var exp = log(arg);
    const Var result = tsym::pow(euler(), exp);

    BOOST_CHECK_EQUAL(arg, result);
}

BOOST_AUTO_TEST_CASE(logOfE)
{
    const Var result = log(euler());

    BOOST_CHECK_EQUAL(1, result);
}

BOOST_AUTO_TEST_CASE(logOfPowerWithBaseE)
{
    const Var exp = a + b + tsym::sqrt(2);
    const Var result = log(tsym::pow(euler(), exp));

    BOOST_CHECK_EQUAL(exp, result);
}

BOOST_AUTO_TEST_CASE(logOfPower)
{
    const Var exp = tsym::sqrt(5)*a + 1/b;
    const Var power = tsym::pow(a, exp);
    const Var result = log(power);
    const Var expected = exp*log(a);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(logOfZero, noLogs())
{
    const Var zero(0);
    Var result;

    result = tsym::log(zero);

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, result.type());
}

BOOST_AUTO_TEST_CASE(sineZero)
{
    const Var zero;

    BOOST_CHECK_EQUAL(zero, sin(zero));
}

BOOST_AUTO_TEST_CASE(sinePiOverSix)
{
    const Var expected(1, 2);
    const Var arg(pi()/6);
    Var res;

    res = sin(arg);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sinePiOverThree)
{
    const Var arg(pi()/3);
    Var res;

    res = sin(arg);

    BOOST_CHECK_EQUAL(tsym::sqrt(3)/2, res);
}

BOOST_AUTO_TEST_CASE(sineSevenPiOverFour)
    /* An angle of 315°. */
{
    const Var arg(7*pi()/4);
    Var res;

    res = sin(arg);

    BOOST_CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

BOOST_AUTO_TEST_CASE(cosFivePiOverFour)
    /* An angle of 225°. */
{
    const Var arg(5*pi()/4);
    Var res;

    res = cos(arg);

    BOOST_CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

BOOST_AUTO_TEST_CASE(tanTwoThirdPi)
    /* An angle of 120°. */
{
    const Var arg(2*pi()/3);
    Var res;

    res = tan(arg);

    BOOST_CHECK_EQUAL(-tsym::sqrt(3), res);
}

BOOST_AUTO_TEST_CASE(asinHalf)
{
    Var res;

    res = asin(half);

    BOOST_CHECK_EQUAL(pi()/6, res);
}

BOOST_AUTO_TEST_CASE(acosMinusOneOverSqrtTwo)
{
    const Var arg(-1/tsym::sqrt(2));
    Var res;

    res = acos(arg);

    BOOST_CHECK_EQUAL(3*pi()/4, res);
}

BOOST_AUTO_TEST_CASE(atanMinusOneOverSqrtThree)
{
    const Var arg(-1/tsym::sqrt(3));
    Var res;

    res = atan(arg);

    BOOST_CHECK_EQUAL(-pi()/6, res);
}

BOOST_AUTO_TEST_CASE(successfulParsing)
{
    bool success;
    const Var expected = a*b*tsym::sqrt(2)*tan(a);
    const Var result(parse("a*b*sqrt(2)*tan(a)", &success));

    BOOST_TEST(success);
    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(successfulParsingWithoutFlag)
{
    const Var expected = a*b + sin(a)*cos(b);
    const Var result(parse("a*b + sin(a)*cos(b)"));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(parsingWithError, noLogs())
{
    bool success;
    parse("a*b*sqrt(2)*[[[tan(a)", &success);

    BOOST_TEST(!success);
}

BOOST_AUTO_TEST_CASE(parsingEmptyString, noLogs())
{
    bool success;
    const Var result = parse("", &success);

    BOOST_TEST(!success);
    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, result.type());
}

BOOST_AUTO_TEST_SUITE_END()
