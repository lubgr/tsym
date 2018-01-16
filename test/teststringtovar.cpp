
#include <cassert>
#include <limits>
#include "stringtovar.h"
#include "globals.h"
#include "sum.h"
#include "product.h"
#include "undefined.h"
#include "numeric.h"
#include "symbol.h"
#include "globals.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(StringToVar)
{
    const Var a = Var("a");
    const Var b = Var("b");
    const Var c = Var("c");
    const Var d = Var("d");

    Var getSymbol(const char *name, const char *subscript = "", const char *superscript = "")
    {
        const Name symbolName(name, subscript, superscript);
        const BasePtr symbol(Symbol::create(symbolName));

        return Var(symbol);
    }

    void checkSuccess(const Var& expected, const StringToVar& stv)
    {
        CHECK(stv.success());

        CHECK_EQUAL(expected, stv.get());

        CHECK(stv.errorMessages().empty());
    }

    void checkTotalFailure(const StringToVar& stv, unsigned expectedErrorIndex)
    {
        CHECK_FALSE(stv.success());

        CHECK_EQUAL(Var::Type::UNDEFINED, stv.get().type());

        CHECK_EQUAL(expectedErrorIndex, stv.firstErrorIndex());

        CHECK_FALSE(stv.errorMessages().empty());
    }

    void checkFailure(const Var& expected, const StringToVar& stv, unsigned expectedErrorIndex)
    {
        CHECK_FALSE(stv.success());

        assert(expected.type() != Var::Type::UNDEFINED);
        assert(stv.get().type() != Var::Type::UNDEFINED);

        CHECK_EQUAL(expectedErrorIndex, stv.firstErrorIndex());

        CHECK_EQUAL(expected, stv.get());

        CHECK_FALSE(stv.errorMessages().empty());
    }
};

TEST(StringToVar, symbol)
{
    const StringToVar stv("a");

    checkSuccess(a, stv);
}

TEST(StringToVar, symbolInWhitespace)
{
    const StringToVar stv(" \n  \b  a  \t   ");

    checkSuccess(a, stv);
}

TEST(StringToVar, wrongSymbolWithNumberStart)
{
    const Var expected(1);
    disableLog();
    const StringToVar stv("1a");
    enableLog();

    checkFailure(expected, stv, 1);
}

TEST(StringToVar, symbolWithShortSupscript)
{
    const StringToVar stv("aBc123_a");
    const Name name("aBc123", "a");
    const Var expected(Symbol::create(name));

    checkSuccess(expected, stv);
}

TEST(StringToVar, symbolWithLongSupscript)
{
    const StringToVar stv("aBc123_{aA321}");
    const Name name("aBc123", "aA321");
    const Var expected(Symbol::create(name));

    checkSuccess(expected, stv);
}

TEST(StringToVar, symbolWithShortSubscriptInBraces)
{
    const StringToVar stv("abcdefghijk_{1}");
    const Name name("abcdefghijk", "1");
    const Var expected(Symbol::create(name));

    checkSuccess(expected, stv);
}

TEST(StringToVar, symbolWithEmptySubscript)
{
    disableLog();
    const StringToVar stv("a_");
    enableLog();

    checkFailure(a, stv, 1);
}

TEST(StringToVar, symbolWithEmptySubscriptInBraces)
{
    disableLog();
    const StringToVar stv("a_{}");
    enableLog();

    checkFailure(a, stv, 1);
}

TEST(StringToVar, symbolWithLongSubscriptWithoutBraces)
{
    const Var expected(getSymbol("aBc123", "a"));

    disableLog();
    const StringToVar stv("aBc123_abc");
    enableLog();

    checkFailure(expected, stv, 8);
}

TEST(StringToVar, symbolWithLongSubscriptWithUnrecognizedCharacters)
{
    disableLog();
    const StringToVar stv("a_{1a[ü}");
    enableLog();

    checkFailure(a, stv, 1);
}

TEST(StringToVar, symbolWithUnrecognizedCharactersInBetween)
{
    disableLog();
    const StringToVar stv("a{7z_2");
    enableLog();

    checkFailure(a, stv, 1);
}

TEST(StringToVar, symbolWithUnrecognizedCharacterInSubscript)
{
    disableLog();
    const StringToVar stv("a_[");
    enableLog();

    checkFailure(a, stv, 1);
}

TEST(StringToVar, symbolWithSubscriptErrorInProduct)
{
    const Var expected(getSymbol("aBc123", "a"));
    disableLog();
    const StringToVar stv("aBc123_abc*3*sin(a)");
    enableLog();

    checkFailure(expected, stv, 8);
}

TEST(StringToVar, symbolWithSubscriptErrorRecovery)
{
    const Var expected = a*Var(getSymbol("aBc123", "a"));
    disableLog();
    const StringToVar stv("a*aBc123_abc*2");
    enableLog();

    checkFailure(expected, stv, 10);
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
    const StringToVar stv("1.234E-56");
    const Var expected(1.234e-56);

    checkSuccess(expected, stv);
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

TEST(StringToVar, sinWrongSpelling)
{
    const Var expected = 2*tsym::sqrt(2)*Var("sinn")*(a + b);
    disableLog();
    const StringToVar stv("2*sqrt(2)*sinn(a)*(a + b)");
    enableLog();

    checkFailure(expected, stv, 16);
}

TEST(StringToVar, asinOfProduct)
{
    const Var expected = tsym::asin(a*b*c);
    const StringToVar stv("asin(a*b*c)");

    checkSuccess(expected, stv);
}

TEST(StringToVar, acosResolvableArg)
{
    const Var expected = tsym::pi()/6;
    const StringToVar stv("acos(sqrt(3)/2)");

    checkSuccess(expected, stv);
}

TEST(StringToVar, atan2OfSymbols)
{
    const Var expected = tsym::atan2(b, a);
    const StringToVar stv("atan2(b, a)");

    checkSuccess(expected, stv);
}

TEST(StringToVar, atan2Resolvable)
{
    const Var expected(5*tsym::pi()/4);
    const StringToVar stv("atan2(-sqrt(10), -sqrt(10))");

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

TEST(StringToVar, powerOfSymbolsWithUselessParentheses)
{
    const StringToVar stv("(((((a)))))^((b))");
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

TEST(StringToVar, posLargeInteger)
{
    const std::string intStr("12039182309810923809182093021938409283409820394");
    const BasePtr expected = Numeric::create(Int(intStr.c_str()));
    const StringToVar stv(intStr);
    Var result;

    result = stv.get();

    checkSuccess(Var(expected), stv);
}

TEST(StringToVar, negLargeInteger)
{
    const std::string intStr("-2039384092840928309482309480980928309482093480923840928309420938");
    const BasePtr expected = Numeric::create(Int(intStr.c_str()));
    const StringToVar stv(intStr);
    Var result;

    result = stv.get();

    checkSuccess(Var(expected), stv);
}

TEST(StringToVar, posDoubleOutsideOfRange)
{
    disableLog();
    const StringToVar stv("123.e928377489274892798347982");
    enableLog();
    Var result;

    result = stv.get();

    checkFailure(Var(Numeric::create(std::numeric_limits<double>::max())), stv, 0);
}

TEST(StringToVar, negDoubleOutsideOfRange)
{
    disableLog();
    const StringToVar stv("-123.e928377489274892798347982");
    enableLog();
    Var result;

    result = stv.get();

    checkFailure(Var(Numeric::create(-std::numeric_limits<double>::max())), stv, 1);
}

TEST(StringToVar, parenthesesAroundSymbol)
{
    const StringToVar stv("(a)");

    checkSuccess(a, stv);
}

TEST(StringToVar, multipleParenthesesInSum)
{
    const StringToVar stv("(((a + (b + c) + (15))))");
    const Var expected = a + b + c + 15;

    checkSuccess(expected, stv);
}

TEST(StringToVar, productWithSumsInParentheses)
{
    const StringToVar stv("a*(b + c) + 4*(a + d)");
    const Var expected = a*(b + c) + 4*a + 4*d;

    checkSuccess(expected, stv);
}

TEST(StringToVar, productWithSumInParentheses)
{
    const StringToVar stv("a*(b + c)*5*d");
    const Var expected = a*(b + c)*5*d;

    checkSuccess(expected, stv);
}

TEST(StringToVar, unrecognizedTokensWithSyntaxError)
{
    disableLog();
    const StringToVar stv("-{}*12*sin(b)");
    enableLog();

    checkTotalFailure(stv, 1);
}

TEST(StringToVar, unrecognizedToken)
{
    const Var expected = 12*sin(b);
    disableLog();
    const StringToVar stv("{12*sin(b)");
    enableLog();

    checkFailure(expected, stv, 0);
}

TEST(StringToVar, unrecognizedTokensAfterValidExpression)
{
    const Var expected = 123*atan(a + b);
    disableLog();
    const StringToVar stv("123*atan(a + b){\a}[[");
    enableLog();

    checkFailure(expected, stv, 15);
}

TEST(StringToVar, unrecognizedTokensInsideValidExpression)
{
    const Var expected = a*b*tsym::sqrt(12*tsym::pow(c, 2) - c) - 40;
    disableLog();
    const StringToVar stv("[äüa*b*sqrt(12*c^2 - &c) - 40üä]\\");
    enableLog();

    checkFailure(expected, stv, 0);
}

TEST(StringToVar, unrecognizedTokensInsideParentheses)
{
    const Var expected = 123*atan(a + b);
    disableLog();
    const StringToVar stv("({=}[[123*atan(a + b))");
    enableLog();

    checkFailure(expected, stv, 1);
}

TEST(StringToVar, symbolsAndComma)
{
    const Var expected = a;
    disableLog();
    const StringToVar stv("a,b,c,,d");
    enableLog();

    checkFailure(expected, stv, 1);
}

TEST(StringToVar, onlyCommaSigns)
{
    disableLog();
    const StringToVar stv(",,,");
    enableLog();

    checkTotalFailure(stv, 0);
}

TEST(StringToVar, emptyParenthesesAfterValidExpressionInProduct)
{
    const Var expected = 123*atan(a + b);
    disableLog();
    const StringToVar stv("123*atan(a + b)*()");
    enableLog();

    checkFailure(expected, stv, 17);
}

TEST(StringToVar, syntaxErrorAfterValidExpressionInProduct)
{
    const Var expected = 123*atan(a + b);
    disableLog();
    const StringToVar stv("123*atan(a + b)*({)}[[");
    enableLog();

    checkFailure(expected, stv, 17);
}

TEST(StringToVar, emptyParentheses)
{
    disableLog();
    const StringToVar stv("(())");
    enableLog();

    checkTotalFailure(stv, 2);
}

TEST(StringToVar, emptyParenthesesBeforeValidPart)
{
    disableLog();
    const StringToVar stv("(())a*b + c");
    enableLog();

    checkTotalFailure(stv, 2);
}

TEST(StringToVar, mixedTerm01)
{
    const StringToVar stv("-a*sin(b)*(c + d)*12*b");
    const Var expected = -a*sin(b)*(c + d)*12*b;

    checkSuccess(expected, stv);
}

TEST(StringToVar, mixedTerm02)
{
    const StringToVar stvFrac("a*atan(1/sqrt(17))*cos(c*d)*sin(a*b)^2*tan(a*b)");
    const StringToVar stvNoFrac("a*atan(17^(-1/2))*cos(c*d)*sin(a*b)^2*tan(a*b)");
    const Var expected = a*atan(1/tsym::sqrt(17))*cos(c*d)*tsym::pow(sin(a*b), 2)*tan(a*b);

    checkSuccess(expected, stvFrac);
    checkSuccess(expected, stvNoFrac);
}

TEST(StringToVar, mixedTerm03)
{
    const StringToVar stv("-a^(2/3)*b^(2/3*c - d)*c^((a + b)^2)");
    const Var expected =
        -tsym::pow(a, Var(2, 3))*tsym::pow(b, 2*c/3 - d)*pow(c, tsym::pow(a + b, 2));

    checkSuccess(expected, stv);
}

TEST(StringToVar, powerOperator)
{
    const StringToVar stv("a^(b^2 + c)^2^3");
    const Var expected = tsym::pow(a, tsym::pow(b*b + c, 8));

    checkSuccess(expected, stv);
}

TEST(StringToVar, onlyPowerOperator)
{
    disableLog();
    const StringToVar stv("^^^");
    enableLog();

    checkTotalFailure(stv, 0);
}

TEST(StringToVar, onlyPowerOperatorWithParentheses)
{
    disableLog();
    const StringToVar stv("^(a + b)");
    enableLog();

    checkTotalFailure(stv, 0);
}

TEST(StringToVar, misspelledFunction)
{
    const Var expected("alksdjflkasjf");
    disableLog();
    const StringToVar stv("alksdjflkasjf(a + b)");
    enableLog();

    checkFailure(expected, stv, 19);
}

TEST(StringToVar, pi)
{
    const StringToVar stv1("pi");
    const StringToVar stv2("Pi");
    const StringToVar stv3("PI");
    const StringToVar stv4("pI");

    checkSuccess(pi(), stv1);
    checkSuccess(pi(), stv2);
    checkSuccess(pi(), stv3);
    checkSuccess(pi(), stv4);
}

TEST(StringToVar, piInMixedTerm)
{
    const StringToVar stv("2*sin(pi) + pi*cos(pI)*sqrt(PI)");
    const Var expected = -pi()*tsym::sqrt(pi());

    checkSuccess(expected, stv);
}

TEST(StringToVar, euler)
{
    const StringToVar stv1("euler");
    const StringToVar stv2("Euler");
    const StringToVar stv3("EULER");
    const StringToVar stv4("euLEr");
    const StringToVar stv5("EuleR");

    checkSuccess(euler(), stv1);
    checkSuccess(euler(), stv2);
    checkSuccess(euler(), stv3);
    checkSuccess(euler(), stv4);
    checkSuccess(euler(), stv5);
}

TEST(StringToVar, symbolTimesFunction)
{
    const StringToVar stv("a*tan(b)");
    const Var expected = a*tan(b);

    checkSuccess(expected, stv);
}

TEST(StringToVar, logOfEulerEvaluesToOne)
{
    const StringToVar stv("a*log(euler)");

    checkSuccess(a, stv);
}

TEST(StringToVar, eulerInMixedTerm)
{
    const StringToVar stv("10*Euler + EULER^2 - a*b*log(euler)");
    const Var expected = 10*euler() + euler()*euler() - a*b;

    checkSuccess(expected, stv);
}
