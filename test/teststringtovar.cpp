
#include <limits>
#include "stringtovar.h"
#include "globals.h"
#include "sum.h"
#include "product.h"
#include "undefined.h"
#include "abc.h"
#include "cpputest.h"

using namespace tsym;

TEST_GROUP(StringToVar)
{
    Var a;
    Var b;
    Var c;
    Var d;
    Var e;

    void setup()
    {
        a = Var("a");
        b = Var("b");
        c = Var("c");
        d = Var("d");
        e = Var("e");
    }

    void checkSuccess(const Var& expected, const StringToVar& stv)
    {
        CHECK(stv.success());

        CHECK_EQUAL(expected, stv.get());

        CHECK(stv.errorMessages().empty());
    }

    void checkFailure(const Var& expected, const StringToVar& stv)
    {
        CHECK(!stv.success());

        CHECK_EQUAL(expected, stv.get());

        CHECK(!stv.errorMessages().empty());
    }
};

TEST(StringToVar, symbol)
{
    const StringToVar stv("a");

    checkSuccess(a, stv);
}

TEST(StringToVar, symbolWithUnaryPlus)
{
    const StringToVar stv("+a");
    
    checkSuccess(a, stv);
}

TEST(StringToVar, symbolWithUnaryMinus)
{
    const StringToVar stv("-a");

    checkSuccess(-a, stv);
}

TEST(StringToVar, numericPosInteger)
{
    const StringToVar stv("123");
    const Var expected(123);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericNegInteger)
{
    const StringToVar stv("-65432");
    const Var expected(-65432);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericPosFloat)
{
    const StringToVar stv("1.23456789");
    const Var expected(1.23456789);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericPosFloatNoDigitsAfterPeriod)
{
    const StringToVar stv("987.");
    const Var expected(987);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericPosScientificLowerEPosExp)
{
    const StringToVar stv("1.234e56");
    const Var expected(1.234e56);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericPosScientificUpperEPosExp)
{
    const StringToVar stv(".34E+12");
    const Var expected(0.34e12);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericPosScientificZeroExp)
{
    const StringToVar stv(".34E+0");
    const Var expected(0.34);
    
    checkSuccess(expected, stv);
}

TEST(StringToVar, numericPosScientificLowerENegExp)
{
    const StringToVar stv("1.234E56");
    const Var expected(1.234e56);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericPosScientificUpperENegExp)
{
}

TEST(StringToVar, numericPosScientificUpperEPosExpNoPeriod)
{
    const StringToVar stv("123E2");
    const Var expected(12300.0);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericNegFloat)
{
    const StringToVar stv("-123456.789");
    const Var expected(-123456.789);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericNegScientificLowerEPosExp)
{
    const StringToVar stv("-123.E-2");
    const Var expected(-1.23);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericNegScientificLowerENegExp)
{
    const StringToVar stv("-2.345e-2");
    const Var expected(-2.345e-2);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericNegScientificUpperEPosExp)
{
    const StringToVar stv("-543.210e3");
    const Var expected(-543.210e3);

    checkSuccess(expected, stv);
}

TEST(StringToVar, numericNegScientificLowerEPosExpNoPeriod)
{
    const StringToVar stv("-2e2");
    const Var expected(-200);

    checkSuccess(expected, stv);
}

TEST(StringToVar, simpleSumTwoSummands)
{
    const StringToVar stv("a + b");
    const Var expected = a + b;

    checkSuccess(expected, stv);
}

TEST(StringToVar, simpleDifferenceTwoSummands)
{
    const StringToVar stv("a - b");
    const Var expected = a - b;

    checkSuccess(expected, stv);
}

TEST(StringToVar, simpleSumFourSummands)
{
    const StringToVar stv("a + b + c + 10");
    const Var expected = 10 + a + b + c;

    checkSuccess(expected, stv);
}

TEST(StringToVar, sinOfSymbol)
{
    const StringToVar stv("sin(a)");
    const Var expected = tsym::sin(a);

    checkSuccess(expected, stv);
}

TEST(StringToVar, sqrtTwo)
{
    const StringToVar stv("sqrt(2)");
    const Var expected = tsym::sqrt(2);
    
    checkSuccess(expected, stv);
}

TEST(StringToVar, powerOfSymbols)
{
    const StringToVar stv("a^b");
    const Var expected = tsym::pow(a, b);

    checkSuccess(expected, stv);
}

TEST(StringToVar, precedenceOfUnaryPlus)
{
    const StringToVar stv("a^+b - +4 +1 - 2*+c");
    Var expected;

    expected = tsym::pow(a, b) - 3 - 2*c;

    checkSuccess(expected, stv);
}

TEST(StringToVar, precedenceOfUnaryMinus)
{
    const StringToVar stv("-23*a + b^-2 + c + -d + 2 +-4");
    Var expected;

    expected = Var(-23)*a + tsym::pow(b, -2) + c + (-1)*d + Var(-2);

    checkSuccess(expected, stv);
}

TEST(StringToVar, posIntegerOutsideRange)
{
    disableLog();
    const StringToVar stv("12039182309810923809182093021938409283409820394");
    enableLog();
    Var result;

    result = stv.get();

    checkFailure(Var(Numeric::create(Int::max())), stv);
}

TEST(StringToVar, negIntegerOutsideRange)
{
    disableLog();
    const StringToVar stv("-20394802984092843098209384092384092840924353");
    enableLog();
    Var result;

    result = stv.get();

    checkFailure(Var(Numeric::create(Int::min())), stv);
}

TEST(StringToVar, posDoubleOutsideOfRange)
{
    disableLog();
    const StringToVar stv("123.e928377489274892798347982");
    enableLog();
    Var result;

    result = stv.get();

    checkFailure(Var(Numeric::create(std::numeric_limits<double>::max())), stv);
}

TEST(StringToVar, negDoubleOutsideOfRange)
{
    disableLog();
    const StringToVar stv("-123.e928377489274892798347982");
    enableLog();
    Var result;

    result = stv.get();

    checkFailure(Var(Numeric::create(-std::numeric_limits<double>::max())), stv);
}
