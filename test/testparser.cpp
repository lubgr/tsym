
#include <string>
#include "parser.h"
#include "numeric.h"
#include "sum.h"
#include "abc.h"
#include "undefined.h"
#include "trigonometric.h"
#include "constant.h"
#include "power.h"
#include "product.h"
#include "symbol.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Parser)
{
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr& pi = Constant::createPi();
    const BasePtr& euler = Constant::createE();

    void checkSuccess(const BasePtr& expected, const parser::Result& result)
    {
        check({ expected, true, true }, result);
    }

    void check(const parser::Result& expected, const parser::Result& result)
    {
        if (expected.value->isUndefined()) {
            CHECK(result.value->isUndefined());
        } else {
            CHECK_EQUAL(expected.value, result.value);
        }

        CHECK_EQUAL(expected.success, result.success);
        CHECK_EQUAL(expected.matchedWholeString, result.matchedWholeString);
    }
};

TEST(Parser, posInteger)
{
    const int n = 123456;
    const parser::Result result = parser::parse(std::to_string(n));
    const BasePtr expected = Numeric::create(n);

    checkSuccess(expected, result);
}

TEST(Parser, posIntegerWithWhitespace)
{
    const parser::Result result = parser::parse("123 456");
    const BasePtr expected = Numeric::create(123);

    check({ expected, true, false }, result);
}

TEST(Parser, negInteger)
{
    const int n = -239224982;
    const parser::Result result = parser::parse(std::to_string(n));
    const BasePtr expected = Numeric::create(n);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosFloat)
{
    const std::string input("1.23456789");
    const BasePtr expected = Numeric::create(1.23456789);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosFloatNoDigitsAfterPeriod)
{
    const std::string input("987.");
    const BasePtr expected = Numeric::create(987);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosScientificLowerEPosExp)
{
    const std::string input("1.234e56");
    const BasePtr expected = Numeric::create(1.234e56);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosScientificUpperEPosExp)
{
    const std::string input(".34E+12");
    const BasePtr expected = Numeric::create(0.34e12);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosScientificZeroExp)
{
    const std::string input(".34E+0");
    const BasePtr expected = Numeric::create(0.34);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosScientificLowerENegExp)
{
    const std::string input("1.234E56");
    const BasePtr expected = Numeric::create(1.234e56);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosScientificUpperENegExp)
{
    const std::string input("1.234E-56");
    const BasePtr expected = Numeric::create(1.234e-56);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericPosScientificUpperEPosExpNoPeriod)
{
    const std::string input("123E2");
    const BasePtr expected = Numeric::create(12300.0);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericNegFloat)
{
    const std::string input("-123456.789");
    const BasePtr expected = Numeric::create(-123456.789);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericNegScientificLowerEPosExp)
{
    const std::string input("-123.E-2");
    const BasePtr expected = Numeric::create(-1.23);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericNegScientificLowerENegExp)
{
    const std::string input("-2.345e-2");
    const BasePtr expected = Numeric::create(-2.345e-2);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericNegScientificUpperEPosExp)
{
    const std::string input("-543.210e3");
    const BasePtr expected = Numeric::create(-543.210e3);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, numericNegScientificLowerEPosExpNoPeriod)
{
    const std::string input("-2e2");
    const BasePtr expected = Numeric::create(-200);
    const parser::Result result = parser::parse(input);

    checkSuccess(expected, result);
}

TEST(Parser, symbol)
{
    const std::string input = "a";
    const parser::Result result = parser::parse(input);

    checkSuccess(a, result);
}

TEST(Parser, symbolInSpaces)
{
    const std::string input = "  a  ";
    const parser::Result result = parser::parse(input);

    checkSuccess(a, result);
}

TEST(Parser, symbolInSpacesAndWhitespace)
{
    const std::string input = " \n    a  \t   ";
    const parser::Result result = parser::parse(input);

    checkSuccess(a, result);
}

TEST(Parser, wrongSymbolWithNumberStart)
{
    const BasePtr& expected = Numeric::one();
    const parser::Result result = parser::parse("1a");

    check({ expected, true, false }, result);
}

TEST(Parser, symbolWithShortSupscript)
{
    const parser::Result result = parser::parse("aBc123_a");
    const Name name("aBc123", "a");
    const BasePtr expected = Symbol::create(name);

    checkSuccess(expected, result);
}

TEST(Parser, symbolWithLongSupscript)
{
    const parser::Result result = parser::parse("aBc123_{aA321}");
    const Name name("aBc123", "aA321");
    const BasePtr expected = Symbol::create(name);

    checkSuccess(expected, result);
}

TEST(Parser, symbolWithShortSubscriptInBraces)
{
    const parser::Result result = parser::parse("abcdefghijk_{1}");
    const Name name("abcdefghijk", "1");
    const BasePtr expected = Symbol::create(name);

    checkSuccess(expected, result);
}

TEST(Parser, symbolWithEmptySubscript)
{
    const parser::Result result = parser::parse("a_");

    check({ a, true, false }, result);
}

TEST(Parser, symbolWithEmptySubscriptInBraces)
{
    const parser::Result result = parser::parse("a_{}");

    check({ a, true, false }, result);
}

TEST(Parser, symbolWithLongSubscriptWithoutBraces)
{
    const parser::Result result = parser::parse("aBc123_abc");
    const BasePtr expected = Symbol::create(Name("aBc123", "a"));

    check({ expected, true, false }, result);
}

TEST(Parser, nonAsciiCharacterAtBeginning)
{
    const parser::Result result = parser::parse("Aüßöabc");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, nonAsciiCharacterAtEnd)
{
    const parser::Result result = parser::parse("a_{1a[ü}");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, symbolWithUnrecognizedCharactersInBetween)
{
    const parser::Result result = parser::parse("a{7z_2");

    check({ a, true, false }, result);
}

TEST(Parser, symbolWithUnrecognizedCharacterInSubscript)
{
    const parser::Result result = parser::parse("a_[");

    check({ a, true, false }, result);
}

TEST(Parser, symbolWithWhitespaceInside)
{
    const parser::Result result = parser::parse("a   bc_2");

    check({ a, true, false }, result);
}

TEST(Parser, sumOfInts)
{
    const std::string input("2+5");
    const parser::Result result = parser::parse(input);

    checkSuccess(seven, result);
}

TEST(Parser, sumOfIntsWithSpaces)
{
    const std::string input("1 + 7");
    const parser::Result result = parser::parse(input);

    checkSuccess(eight, result);
}

TEST(Parser, productOfInts)
{
    const std::string input("3*7");
    const parser::Result result = parser::parse(input);

    checkSuccess(Numeric::create(21), result);
}

TEST(Parser, productOfIntsWithSpaces)
{
    const std::string input("\n 3*  7");
    const parser::Result result = parser::parse(input);

    checkSuccess(Numeric::create(21), result);
}

TEST(Parser, productOfIntsWithParentheses)
{
    const std::string input("3*(7)");
    const parser::Result result = parser::parse(input);

    checkSuccess(Numeric::create(21), result);
}

TEST(Parser, unaryMinusWithInteger)
{
    const parser::Result result = parser::parse("-3");
    const BasePtr expected = Numeric::create(-3);
    
    checkSuccess(expected, result);
}

TEST(Parser, unaryMinusWithSymbol)
{
    const parser::Result result = parser::parse("-a");
    const BasePtr expected = Product::minus(a);

    checkSuccess(expected, result);
}

TEST(Parser, unaryPlusWithInteger)
{
    const parser::Result result = parser::parse("+4");
    
    checkSuccess(four, result);
}

TEST(Parser, unaryPlusWithSymbol)
{
    const parser::Result result = parser::parse("+a");

    checkSuccess(a, result);
}

TEST(Parser, simpleSumTwoSummands)
{
    const BasePtr expected = Sum::create(a, b);
    const parser::Result result = parser::parse("a + b");

    checkSuccess(expected, result);
}

TEST(Parser, simpleDifferenceTwoSummands)
{
    const BasePtr expected = Sum::create(a, Product::minus(b));
    const parser::Result result = parser::parse("a - b");

    checkSuccess(expected, result);
}

TEST(Parser, simpleSumFourSummands)
{
    const parser::Result result = parser::parse("a + b + c + 10");
    const BasePtr expected = Sum::create({ ten, a, b, c });

    checkSuccess(expected, result);
}

TEST(Parser, simpleProductThreeSummands)
{
    const parser::Result result = parser::parse("a*b*c");
    const BasePtr expected = Product::create({ a, b, c });

    checkSuccess(expected, result);
}

TEST(Parser, simpleDivisionTwoSymbols)
{
    const parser::Result result = parser::parse("a/b");
    const BasePtr expected = Product::create(a, Power::oneOver(b));

    checkSuccess(expected, result);
}

TEST(Parser, simpleDivisionThreeSymbolsOneNumber)
{
    const parser::Result result = parser::parse("a/b/c/4");
    const BasePtr expected = Product::create(a, Power::oneOver(Product::create(four, b, c)));

    checkSuccess(expected, result);
}

TEST(Parser, negativeFraction)
{
    const parser::Result result = parser::parse("-3/17");
    const BasePtr expected = Numeric::create(-3, 17);

    checkSuccess(expected, result);
}

TEST(Parser, sumOfNumberAndFraction)
{
    const parser::Result result = parser::parse("2 + 1/5");
    const BasePtr expected = Numeric::create(11, 5);

    checkSuccess(expected, result);
}

TEST(Parser, sinOfSymbol)
{
    const BasePtr expected = Trigonometric::createSin(a);

    checkSuccess(expected, parser::parse("sin(a)"));
    checkSuccess(expected, parser::parse("SIN(a)"));
    checkSuccess(expected, parser::parse("Sin(a)"));
    checkSuccess(expected, parser::parse("siN(a)"));
}

TEST(Parser, sinOfSum)
{
    const parser::Result result = parser::parse("sin(a + b + 10)");
    const BasePtr expected = Trigonometric::createSin(Sum::create(a, b, ten));

    checkSuccess(expected, result);
}

TEST(Parser, sinOfProduct)
{
    const parser::Result result = parser::parse("sin(a*b)");
    const BasePtr expected = Trigonometric::createSin(Product::create(a, b));

    checkSuccess(expected, result);
}

TEST(Parser, sinWrongSpelling)
{
    const BasePtr expected = Product::create(two, sqrtTwo, Symbol::create("sinn"));
    const parser::Result result = parser::parse("2*sqrt(2)*sinn(a)*(a + b)");

    check({ expected, true, false }, result);
}

TEST(Parser, asinOfProduct)
{
    const BasePtr expected = Trigonometric::createAsin(Product::create(a, b, c));
    const parser::Result result = parser::parse("asin(a*b*c)");

    checkSuccess(expected, result);
}

TEST(Parser, acosResolvableArg)
{
    const BasePtr expected = Product::create(Numeric::create(1, 6), pi);
    const parser::Result result = parser::parse("acos(sqrt(3)/2)");

    checkSuccess(expected, result);
}

TEST(Parser, atan2OfSymbols)
{
    const BasePtr expected = Trigonometric::createAtan2(b, a);
    const parser::Result result = parser::parse("atan2(b, a)");

    checkSuccess(expected, result);
}

TEST(Parser, atan2Resolvable)
{
    const BasePtr expected = Product::create(pi, Numeric::create(5, 4));
    const parser::Result result = parser::parse("atan2(-sqrt(10), -sqrt(10))");

    checkSuccess(expected, result);
}

TEST(Parser, sqrtTwo)
{
    checkSuccess(sqrtTwo, parser::parse("sqrt(2)"));
    checkSuccess(sqrtTwo, parser::parse("Sqrt(2)"));
    checkSuccess(sqrtTwo, parser::parse("SQRT(2)"));
}

TEST(Parser, powerOfInteger)
{
    const parser::Result result = parser::parse("2^3");

    checkSuccess(eight, result);
}

TEST(Parser, powerOfSymbols)
{
    const parser::Result result = parser::parse("a^b");
    const BasePtr expected = Power::create(a, b);

    checkSuccess(expected, result);
}

TEST(Parser, powerOfSymbolBaseWithUnaryMinus)
{
    const parser::Result result = parser::parse("-a^2");
    const BasePtr expected = Product::minus(Power::create(a, two));

    checkSuccess(expected, result);
}

TEST(Parser, powerOfSymbolsWithUselessParentheses)
{
    const parser::Result result = parser::parse("(((((a)))))^((b))");
    const BasePtr expected = Power::create(a, b);

    checkSuccess(expected, result);
}

TEST(Parser, unaryPlusInParentheses)
{
    const parser::Result result = parser::parse("a^(+b) - (+4) + 1 - 2*(+c)");
    const BasePtr expected = Sum::create(Power::create(a, b), Product::minus(three), Product::minus(two, c));

    checkSuccess(expected, result);
}

TEST(Parser, unaryMinusWithSumOperator)
{
    const parser::Result result = parser::parse("a +-b");
    const BasePtr expected = Sum::create(a, Product::minus(b));

    checkSuccess(expected, result);
}

TEST(Parser, unaryPlusWithNumberInSum)
{
    const parser::Result result = parser::parse("a ++2");

    check({ a, true, false }, result);
}

TEST(Parser, unaryPlusWithNumberAndParenthesesInSum)
{
    const parser::Result result = parser::parse("a +(+2)");
    const BasePtr expected = Sum::create(a, two);

    checkSuccess(expected, result);
}

TEST(Parser, unaryPlusWithSymbolInSum)
{
    const parser::Result result = parser::parse("a +(+b) ++c");

    check({ Sum::create(a, b), true, false }, result);
}

TEST(Parser, unaryMinusWithPower)
{
    const parser::Result result = parser::parse("a^-2");

    check({ a, true, false }, result);
}

TEST(Parser, unaryPlusWithPower)
{
    const parser::Result result = parser::parse("a^+2");

    check({ a, true, false }, result);
}

TEST(Parser, unaryMinusWithPowerInParentheses)
{
    const parser::Result result = parser::parse("a^(-2)");
    const BasePtr expected = Power::create(a, Numeric::create(-2));

    checkSuccess(expected, result);
}

TEST(Parser, sumWithUnaryMinusFirstNumericSummand)
{
    const parser::Result result = parser::parse("-1 + b");
    const BasePtr expected = Sum::create(Numeric::mOne(), b);

    checkSuccess(expected, result);
}

TEST(Parser, sumWithUnaryMinusFirstSymbolSummand)
{
    const parser::Result result = parser::parse("-a + b");
    const BasePtr expected = Sum::create(Product::minus(a), b);

    checkSuccess(expected, result);
}

TEST(Parser, sumWithPowerSummand)
{
    const parser::Result result = parser::parse("-a + a^(-2)");
    const BasePtr expected = Sum::create(Product::minus(a), Power::create(a, Numeric::create(-2)));

    checkSuccess(expected, result);
}

TEST(Parser, unaryMinusBeforeProduct)
{
    const parser::Result result = parser::parse("-2*a*b");
    const BasePtr expected = Product::minus(two, a, b);

    checkSuccess(expected, result);
}

TEST(Parser, unaryMinusInParentheses)
{
    const parser::Result result = parser::parse("-23*a + b^(-2) + c + (-d) + 2 +(-4)");
    const BasePtr expected = Sum::create({ Product::create(Numeric::create(-23), a),
            Power::create(b, Numeric::create(-2)), c, Product::minus(d), Numeric::create(-2) });

    checkSuccess(expected, result);
}

TEST(Parser, posLargeInteger)
{
    const std::string intStr("12039182309810923809182093021938409283409820394");
    const BasePtr expected = Numeric::create(Int(intStr.c_str()));
    const parser::Result result = parser::parse(intStr);

    checkSuccess(expected, result);
}

TEST(Parser, negLargeInteger)
{
    const std::string intStr("-2039384092840928309482309480980928309482093480923840928309420938");
    const BasePtr expected = Numeric::create(Int(intStr.c_str()));
    const parser::Result result = parser::parse(intStr);

    checkSuccess(expected, result);
}

TEST(Parser, posDoubleOutsideOfRange)
{
    const parser::Result result = parser::parse("123.e928377489274892798347982");
    const BasePtr expected = Numeric::create(123);

    check({ expected, true, false }, result);
}

TEST(Parser, negDoubleOutsideOfRange)
{
    const parser::Result result = parser::parse("-123.e928377489274892798347982");
    const BasePtr expected = Numeric::create(-123);

    check({ expected, true, false }, result);
}

TEST(Parser, parenthesesAroundSymbol)
{
    const parser::Result result = parser::parse("(a)");

    checkSuccess(a, result);
}

TEST(Parser, parenthesesAroundNumber)
{
    const parser::Result result = parser::parse("(123)");

    checkSuccess(Numeric::create(123), result);
}

TEST(Parser, parenthesesAroundSum)
{
    const parser::Result result = parser::parse("(a + b)");

    checkSuccess(Sum::create(a, b), result);
}

TEST(Parser, parenthesesAroundFunction)
{
    const parser::Result result = parser::parse("a*acos(234)");
    const BasePtr expected = Product::create(a, Trigonometric::createAcos(Numeric::create(234)));

    checkSuccess(expected, result);
}

TEST(Parser, multipleParenthesesInSum)
{
    const parser::Result result = parser::parse("(((a + (b + c) + (15))))");
    const BasePtr expected = Sum::create(a, b, c, Numeric::create(15));

    checkSuccess(expected, result);
}

TEST(Parser, productWithSumsInParentheses)
{
    const parser::Result result = parser::parse("a*(b + c) + 4*(a + d)");
    const BasePtr expected = Sum::create(Product::create(four, a), Product::create(four, d),
            Product::create(a, Sum::create(b, c)));

    checkSuccess(expected, result);
}

TEST(Parser, simpleProductWithSumInParentheses)
{
    const parser::Result result = parser::parse("a*(2 + b)");
    const BasePtr expected = Product::create(a, Sum::create(two, b));

    checkSuccess(expected, result);
}

TEST(Parser, productWithSumInParentheses)
{
    const parser::Result result = parser::parse("a*(b + c)*5*d");
    const BasePtr expected = Product::create(a, Sum::create(b, c), five, d);

    checkSuccess(expected, result);
}

TEST(Parser, unrecognizedTokensWithSyntaxError)
{
    const parser::Result result = parser::parse("-{}*12*sin(b)");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, unrecognizedToken)
{
    const parser::Result result = parser::parse("{12*sin(b)");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, unrecognizedTokensAfterValidExpression)
{
    const BasePtr expected = Product::create(Numeric::create(123), Trigonometric::createAtan(Sum::create(a, b)));
    const parser::Result result = parser::parse("123*atan(a + b){\a}[[");

    check({ expected, true, false }, result);
}

TEST(Parser, unrecognizedTokensInsideValidExpression)
{
    const parser::Result result = parser::parse("[äüa*b*sqrt(12*c^2 - &c) - 40üä]\\");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, unrecognizedTokensInsideParentheses)
{
    const BasePtr expected = Product::create(Numeric::create(123), Trigonometric::createAtan(Sum::create(a, b)));
    const parser::Result result = parser::parse("123*atan(a + b))*({=}[[");

    check({ expected, true, false }, result);
}

TEST(Parser, symbolsAndComma)
{
    const parser::Result result = parser::parse("a,b,c,,d");

    check({ a, true, false }, result);
}

TEST(Parser, emptyString)
{
    const parser::Result result = parser::parse("");
    const BasePtr& expected = Undefined::create();

    check({ expected, false, true }, result);
}

TEST(Parser, onlyCommaSigns)
{
    const parser::Result result = parser::parse("(())");
    const BasePtr& expected = Undefined::create();

    check({ expected, false, false }, result);
}

TEST(Parser, emptyParenthesesAfterValidExpressionInProduct)
{
    const BasePtr expected = Product::create(Numeric::create(123), Trigonometric::createAtan(Sum::create(a, b)));
    const parser::Result result = parser::parse("123*atan(a + b)*()");

    check({ expected, true, false }, result);
}

TEST(Parser, syntaxErrorAfterValidExpressionInProduct)
{
    const BasePtr expected = Product::create(Numeric::create(-1, 5), Trigonometric::createAtan(Sum::create(a, b)));
    const parser::Result result = parser::parse("-1/5*atan(a + b)*({)}[[");

    check({ expected, true, false }, result);
}

TEST(Parser, emptyParentheses)
{
    const parser::Result result = parser::parse("(())");
    const BasePtr& expected = Undefined::create();

    check({ expected, false, false }, result);
}

TEST(Parser, mixedOperatorsWithUnaryMinusNumber)
{
    const parser::Result result = parser::parse("a/-3");
    const BasePtr expected = Product::create(a, Numeric::create(-1, 3));

    checkSuccess(expected, result);
}

TEST(Parser, mixedOperatorsWithUnaryMinusSymbol)
{
    const parser::Result result = parser::parse("a*-b + -c");
    const BasePtr expected = Product::minus(Sum::create(c, Product::create(a, b)));

    checkSuccess(expected, result);
}

TEST(Parser, emptyParenthesesBeforeValidPart)
{
    const parser::Result result = parser::parse("(())a*b + c");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, mixedTerm01)
{
    const parser::Result result = parser::parse("-a*sin(b)*(c + d)*12*b");
    const BasePtr expected = Product::create({ Numeric::create(-12), a, b, Trigonometric::createSin(b), Sum::create(c, d) });

    checkSuccess(expected, result);
}

TEST(Parser, mixedTerm02)
{
    const std::string inputWithFrac("a*atan(1/sqrt(17))*cos(c*d)*sin(a*b)^2*tan(a*b)");
    const std::string inputWithoutFrac("a*atan(17^(-1/2))*cos(c*d)*sin(a*b)^2*tan(a*b)");
    const BasePtr expected = Product::create({ a, Trigonometric::createAtan(Power::sqrt(Numeric::create(1, 17))),
            Trigonometric::createCos(Product::create(c, d)), Power::create(Trigonometric::createSin(Product::create(a, b)), two),
            Trigonometric::createTan(Product::create(a, b)) });

    checkSuccess(expected, parser::parse(inputWithFrac));
    checkSuccess(expected, parser::parse(inputWithoutFrac));
}

TEST(Parser, mixedTerm03)
{
    const parser::Result result = parser::parse("-a^(2/3)*b^(2/3*c - d)*c^((a + b)^2)");
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr expected = Product::create({ Numeric::mOne(), Power::create(a, twoThird),
                Power::create(b, Sum::create(Product::create(twoThird, c), Product::minus(d))),
                Power::create(c, Power::create(Sum::create(a, b), two)) });

    checkSuccess(expected, result);
}

TEST(Parser, powerOperator)
{
    const parser::Result result = parser::parse("a^2^3^2");
    const BasePtr expected = Power::create(a, Numeric::create(512));

    checkSuccess(expected, result);
}

TEST(Parser, nestedPowerOperator)
{
    const parser::Result result = parser::parse("a^(b^2 + c)^2^3");
    const BasePtr expected = Power::create(a, Power::create(Sum::create(Product::create(b, b), c), eight));

    checkSuccess(expected, result);
}

TEST(Parser, onlyPowerOperator)
{
    const parser::Result result = parser::parse("^^^");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, onlyPowerOperatorWithParentheses)
{
    const parser::Result result = parser::parse("^(a + b)");

    check({ Undefined::create(), false, false }, result);
}

TEST(Parser, misspelledFunction)
{
    const BasePtr expected = Symbol::create("alksdjflkasjf");
    const parser::Result result = parser::parse("alksdjflkasjf(a + b)");

    check({ expected, true, false }, result);
}

TEST(Parser, pi)
{
    checkSuccess(pi, parser::parse("pi"));
    checkSuccess(pi, parser::parse("Pi"));
    checkSuccess(pi, parser::parse("PI"));
    checkSuccess(pi, parser::parse("pI"));
}

TEST(Parser, piInMixedTerm)
{
    const parser::Result result = parser::parse("2*sin(pi) + pi*cos(pI)*sqrt(PI)");
    const BasePtr expected = Product::minus(pi, Power::sqrt(pi));

    checkSuccess(expected, result);
}

TEST(Parser, euler)
{
    checkSuccess(euler, parser::parse("euler"));
    checkSuccess(euler, parser::parse("Euler"));
    checkSuccess(euler, parser::parse("EULER"));
    checkSuccess(euler, parser::parse("euLEr"));
    checkSuccess(euler, parser::parse("EuleR"));
}

TEST(Parser, symbolTimesFunction)
{
    const parser::Result result = parser::parse("a*tan(b)");
    const BasePtr expected = Product::create(a, Trigonometric::createTan(b));

    checkSuccess(expected, result);
}

TEST(Parser, logOfEulerEvaluesToOne)
{
    const parser::Result result = parser::parse("a*log(euler)");

    checkSuccess(a, result);
}

TEST(Parser, eulerInMixedTerm)
{
    const parser::Result result = parser::parse("10*Euler + EULER^2 - a*b*log(euler)");
    const BasePtr expected = Sum::create(Product::create(ten, euler), Product::create(euler, euler), Product::minus(a, b));

    checkSuccess(expected, result);
}
