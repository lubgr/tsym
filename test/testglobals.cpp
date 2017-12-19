
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
    const Var half = Var(1, 2);

    void checkSolveFailure(const Matrix& A, const Vector& rhs, Vector& x)
    {
        disableLog();
        const bool res = solve(A, rhs, x);
        enableLog();

        CHECK_FALSE(res);
    }
};

TEST(Globals, squareRoot)
{
    Var expected(a);
    Var res;

    res = tsym::sqrt(a);
    expected = expected.toThe(half);

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
    expected = expected.toThe(half);

    CHECK_EQUAL(expected, res);
}

TEST(Globals, power)
{
    Var expected(a);
    Var res;

    res = tsym::pow(a, b);

    expected = expected.toThe(b);

    CHECK_EQUAL(expected, res);
}

TEST(Globals, powerWithNumericExp)
{
    Var expected(a);
    Var res;

    res = tsym::pow(a, Var(1, 4));

    expected = expected.toThe(Var(1, 4));

    CHECK_EQUAL(expected, res);
}

TEST(Globals, powerEulerBaseLogExp)
{
    const Var arg = 2*a*b*b*Pi;
    const Var exp = log(arg);
    const Var result = tsym::pow(Euler, exp);

    CHECK_EQUAL(arg, result);
}

TEST(Globals, logOfE)
{
    const Var result = log(Euler);

    CHECK_EQUAL(1, result);
}

TEST(Globals, logOfPowerWithBaseE)
{
    const Var exp = a + b + tsym::sqrt(2);
    const Var result = log(tsym::pow(Euler, exp));

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
    Var result;

    disableLog();
    result = tsym::log(0);
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
    const Var arg(Pi/6);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Globals, sinePiOverThree)
{
    const Var arg(Pi/3);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(tsym::sqrt(3)/2, res);
}

TEST(Globals, sineSevenPiOverFour)
    /* An angle of 315°. */
{
    const Var arg(7*Pi/4);
    Var res;

    res = sin(arg);

    CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

TEST(Globals, cosFivePiOverFour)
    /* An angle of 225°. */
{
    const Var arg(5*Pi/4);
    Var res;

    res = cos(arg);

    CHECK_EQUAL(-1/tsym::sqrt(2), res);
}

TEST(Globals, tanTwoThirdPi)
    /* An angle of 120°. */
{
    const Var arg(2*Pi/3);
    Var res;

    res = tan(arg);

    CHECK_EQUAL(-tsym::sqrt(3), res);
}

TEST(Globals, asinHalf)
{
    Var res;

    res = asin(half);

    CHECK_EQUAL(Pi/6, res);
}

TEST(Globals, acosMinusOneOverSqrtTwo)
{
    const Var arg(-1/tsym::sqrt(2));
    Var res;

    res = acos(arg);

    CHECK_EQUAL(3*Pi/4, res);
}

TEST(Globals, atanMinusOneOverSqrtThree)
{
    const Var arg(-1/tsym::sqrt(3));
    Var res;

    res = atan(arg);

    CHECK_EQUAL(-Pi/6, res);
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

TEST(Globals, solveWithSingularMatrix)
{
    const Var bCosA = tsym::pow(b, cos(a));
    Matrix A(2, 2);
    Vector rhs(2);
    Vector x;
    bool res;

    A(0, 0) = 12*a;
    A(0, 1) = a*bCosA + a*c;
    A(1, 0) = 12;
    A(1, 1) = bCosA + c;

    rhs(0) = 10;
    rhs(1) = b;

    disableLog();
    res = solve(A, rhs, x);
    enableLog();

    CHECK_FALSE(res);
    CHECK_EQUAL(0, x.size());
}

TEST(Globals, solveLinearSystemDim3)
{
    Matrix A(3, 3);
    Vector rhs(3);
    Vector x;
    bool res;

    A(0, 0) = a;
    A(0, 1) = 17*b/29;
    A(1, 1) = 1/(a*b*c);
    A(1, 2) = tsym::pow(12, d);
    A(2, 0) = 1;
    A(2, 1) = 4*a;

    rhs(0) = a*d + 17*a*b/116;
    rhs(1) = b*tsym::pow(12, d) + 1/(b*c*4);
    rhs(2) = d + a*a;

    res = solve(A, rhs, x);

    CHECK(res);
    CHECK_EQUAL(3, x.size());
    CHECK_EQUAL(d, x(0));
    CHECK_EQUAL(a/4, x(1));
    CHECK_EQUAL(b, x(2));
}

TEST(Globals, solveLinearSystemDim4)
{
    Matrix A(4, 4);
    Vector rhs(4);
    Vector x;
    bool res;

    A(0, 0) = a;
    A(0, 1) = 1;
    A(1, 1) = b;
    A(1, 0) = tsym::pow(a, 3);
    A(1, 3) = 2;
    A(2, 2) = c;
    A(3, 0) = a;
    A(3, 2) = b;

    rhs(0) = 1;
    rhs(1) = 2;
    rhs(2) = 3;
    rhs(3) = 4;

    res = solve(A, rhs, x);

    CHECK(res);
    CHECK_EQUAL((4*c - 3*b)/(a*c), x(0));
    CHECK_EQUAL(3*(b - c)/c, x(1));
    CHECK_EQUAL(3/c, x(2));
    CHECK_EQUAL((2*c + 3*b*c - 4*c*a*a - 3*b*b + 3*a*a*b)/(2*c), x(3));
}

TEST(Globals, solveWithoutRhs)
{
    const Matrix A(4, 4);
    Vector empty;
    Vector x;

    checkSolveFailure(A, empty, x);
}

TEST(Globals, solveWithoutCoeffMatrix)
{
    const Matrix empty;
    const Vector rhs(10);
    Vector x;

    checkSolveFailure(empty, rhs, x);
}

TEST(Globals, solveWithNonSquareMatrix)
{
    const Matrix A(2, 3);
    const Vector rhs(2);
    Vector x;

    checkSolveFailure(A, rhs, x);
}

TEST(Globals, solveWithNonMatchingDimensions)
{
    const Matrix A(3, 3);
    const Vector rhs(4);
    Vector x;

    checkSolveFailure(A, rhs, x);
}

TEST(Globals, zeroDimension)
{
    const Matrix A;
    const Vector rhs;
    Vector x;

    checkSolveFailure(A, rhs, x);
}
