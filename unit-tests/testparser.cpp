
#include <string>
#include "basefct.h"
#include "constant.h"
#include "fixtures.h"
#include "numeric.h"
#include "parser.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct ParserFixture : public AbcFixture {
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr& euler = Constant::createE();
};

namespace {
    void check(const ParseResult& expected, const ParseResult& result)
    {
        if (isUndefined(*expected.value)) {
            BOOST_TEST(isUndefined(*result.value));
        } else {
            BOOST_CHECK_EQUAL(expected.value, result.value);
        }

        BOOST_CHECK_EQUAL(expected.success, result.success);
        BOOST_CHECK_EQUAL(expected.matchedWholeString, result.matchedWholeString);
    }

    void checkSuccess(const BasePtr& expected, const ParseResult& result)
    {
        check({expected, true, true}, result);
    }
}

BOOST_FIXTURE_TEST_SUITE(TestParser, ParserFixture)

BOOST_AUTO_TEST_CASE(posInteger)
{
    const int n = 123456;
    const ParseResult result = parseFrom(std::to_string(n));
    const BasePtr expected = Numeric::create(n);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(posIntegerWithWhitespace)
{
    const ParseResult result = parseFrom("123 456");
    const BasePtr expected = Numeric::create(123);

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(negInteger)
{
    const int n = -239224982;
    const ParseResult result = parseFrom(std::to_string(n));
    const BasePtr expected = Numeric::create(n);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosFloat)
{
    const std::string_view input("1.23456789");
    const BasePtr expected = Numeric::create(1.23456789);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosFloatNoDigitsAfterPeriod)
{
    const std::string_view input("987.");
    const BasePtr expected = Numeric::create(987);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosScientificLowerEPosExp)
{
    const std::string_view input("1.234e56");
    const BasePtr expected = Numeric::create(1.234e56);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosScientificUpperEPosExp)
{
    const std::string_view input(".34E+12");
    const BasePtr expected = Numeric::create(0.34e12);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosScientificZeroExp)
{
    const std::string_view input(".34E+0");
    const BasePtr expected = Numeric::create(0.34);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosScientificLowerENegExp)
{
    const std::string_view input("1.234E56");
    const BasePtr expected = Numeric::create(1.234e56);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosScientificUpperENegExp)
{
    const std::string_view input("1.234E-56");
    const BasePtr expected = Numeric::create(1.234e-56);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericPosScientificUpperEPosExpNoPeriod)
{
    const std::string_view input("123E2");
    const BasePtr expected = Numeric::create(12300.0);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericNegFloat)
{
    const std::string_view input("-123456.789");
    const BasePtr expected = Numeric::create(-123456.789);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericNegScientificLowerEPosExp)
{
    const std::string_view input("-123.E-2");
    const BasePtr expected = Numeric::create(-1.23);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericNegScientificLowerENegExp)
{
    const std::string_view input("-2.345e-2");
    const BasePtr expected = Numeric::create(-2.345e-2);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericNegScientificUpperEPosExp)
{
    const std::string_view input("-543.210e3");
    const BasePtr expected = Numeric::create(-543.210e3);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(numericNegScientificLowerEPosExpNoPeriod)
{
    const std::string_view input("-2e2");
    const BasePtr expected = Numeric::create(-200);
    const ParseResult result = parseFrom(input);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(symbol)
{
    const std::string_view input = "a";
    const ParseResult result = parseFrom(input);

    checkSuccess(a, result);
}

BOOST_AUTO_TEST_CASE(symbolInSpaces)
{
    const std::string_view input = "  a  ";
    const ParseResult result = parseFrom(input);

    checkSuccess(a, result);
}

BOOST_AUTO_TEST_CASE(symbolInSpacesAndWhitespace)
{
    const std::string_view input = " \n    a  \t   ";
    const ParseResult result = parseFrom(input);

    checkSuccess(a, result);
}

BOOST_AUTO_TEST_CASE(wrongSymbolWithNumberStart)
{
    const BasePtr& expected = Numeric::one();
    const ParseResult result = parseFrom("1a");

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(symbolWithShortSupscript)
{
    const ParseResult result = parseFrom("aBc123_a");
    const Name name{"aBc123", "a"};
    const BasePtr expected = Symbol::create(name);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(symbolWithLongSupscript)
{
    const ParseResult result = parseFrom("aBc123_{aA321}");
    const Name name{"aBc123", "aA321"};
    const BasePtr expected = Symbol::create(name);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(symbolWithShortSubscriptInBraces)
{
    const ParseResult result = parseFrom("abcdefghijk_{1}");
    const Name name{"abcdefghijk", "1"};
    const BasePtr expected = Symbol::create(name);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(symbolWithEmptySubscript)
{
    const ParseResult result = parseFrom("a_");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(symbolWithEmptySubscriptInBraces)
{
    const ParseResult result = parseFrom("a_{}");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(symbolWithLongSubscriptWithoutBraces)
{
    const ParseResult result = parseFrom("aBc123_abc");
    const BasePtr expected = Symbol::create(Name{"aBc123", "a"});

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(nonAsciiCharacterAtBeginning)
{
    const ParseResult result = parseFrom("Aüßöabc");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(nonAsciiCharacterAtEnd)
{
    const ParseResult result = parseFrom("a_{1a[ü}");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(symbolWithUnrecognizedCharactersInBetween)
{
    const ParseResult result = parseFrom("a{7z_2");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(symbolWithUnrecognizedCharacterInSubscript)
{
    const ParseResult result = parseFrom("a_[");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(symbolWithWhitespaceInside)
{
    const ParseResult result = parseFrom("a   bc_2");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(sumOfInts)
{
    const std::string_view input("2+5");
    const ParseResult result = parseFrom(input);

    checkSuccess(seven, result);
}

BOOST_AUTO_TEST_CASE(sumOfIntsWithSpaces)
{
    const std::string_view input("1 + 7");
    const ParseResult result = parseFrom(input);

    checkSuccess(eight, result);
}

BOOST_AUTO_TEST_CASE(productOfInts)
{
    const std::string_view input("3*7");
    const ParseResult result = parseFrom(input);

    checkSuccess(Numeric::create(21), result);
}

BOOST_AUTO_TEST_CASE(productOfIntsWithSpaces)
{
    const std::string_view input("\n 3*  7");
    const ParseResult result = parseFrom(input);

    checkSuccess(Numeric::create(21), result);
}

BOOST_AUTO_TEST_CASE(productOfIntsWithParentheses)
{
    const std::string_view input("3*(7)");
    const ParseResult result = parseFrom(input);

    checkSuccess(Numeric::create(21), result);
}

BOOST_AUTO_TEST_CASE(unaryMinusWithInteger)
{
    const ParseResult result = parseFrom("-3");
    const BasePtr expected = Numeric::create(-3);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryMinusWithSymbol)
{
    const ParseResult result = parseFrom("-a");
    const BasePtr expected = Product::minus(a);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryPlusWithInteger)
{
    const ParseResult result = parseFrom("+4");

    checkSuccess(four, result);
}

BOOST_AUTO_TEST_CASE(unaryPlusWithSymbol)
{
    const ParseResult result = parseFrom("+a");

    checkSuccess(a, result);
}

BOOST_AUTO_TEST_CASE(simpleSumTwoSummands)
{
    const BasePtr expected = Sum::create(a, b);
    const ParseResult result = parseFrom("a + b");

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleDifferenceTwoSummands)
{
    const BasePtr expected = Sum::create(a, Product::minus(b));
    const ParseResult result = parseFrom("a - b");

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleSumFourSummands)
{
    const ParseResult result = parseFrom("a + b + c + 10");
    const BasePtr expected = Sum::create({ten, a, b, c});

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleProductThreeSummands)
{
    const ParseResult result = parseFrom("a*b*c");
    const BasePtr expected = Product::create({a, b, c});

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleDivisionTwoSymbols)
{
    const ParseResult result = parseFrom("a/b");
    const BasePtr expected = Product::create(a, Power::oneOver(b));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleDivisionThreeSymbolsOneNumber)
{
    const ParseResult result = parseFrom("a/b/c/4");
    const BasePtr expected = Product::create(a, Power::oneOver(Product::create(four, b, c)));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(negativeFraction)
{
    const ParseResult result = parseFrom("-3/17");
    const BasePtr expected = Numeric::create(-3, 17);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sumOfNumberAndFraction)
{
    const ParseResult result = parseFrom("2 + 1/5");
    const BasePtr expected = Numeric::create(11, 5);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sinOfSymbol)
{
    const BasePtr expected = Trigonometric::createSin(a);

    checkSuccess(expected, parseFrom("sin(a)"));
    checkSuccess(expected, parseFrom("SIN(a)"));
    checkSuccess(expected, parseFrom("Sin(a)"));
    checkSuccess(expected, parseFrom("siN(a)"));
}

BOOST_AUTO_TEST_CASE(sinOfSum)
{
    const ParseResult result = parseFrom("sin(a + b + 10)");
    const BasePtr expected = Trigonometric::createSin(Sum::create(a, b, ten));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sinOfProduct)
{
    const ParseResult result = parseFrom("sin(a*b)");
    const BasePtr expected = Trigonometric::createSin(Product::create(a, b));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sinWrongSpelling)
{
    const BasePtr expected = Product::create(two, sqrtTwo, Symbol::create("sinn"));
    const ParseResult result = parseFrom("2*sqrt(2)*sinn(a)*(a + b)");

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(asinOfProduct)
{
    const BasePtr expected = Trigonometric::createAsin(Product::create(a, b, c));
    const ParseResult result = parseFrom("asin(a*b*c)");

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(acosResolvableArg)
{
    const BasePtr expected = Product::create(Numeric::create(1, 6), pi);
    const ParseResult result = parseFrom("acos(sqrt(3)/2)");

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2OfSymbols)
{
    const BasePtr expected = Trigonometric::createAtan2(b, a);
    const ParseResult result = parseFrom("atan2(b, a)");

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(atan2Resolvable)
{
    const BasePtr expected = Product::create(pi, Numeric::create(5, 4));
    const ParseResult result = parseFrom("atan2(-sqrt(10), -sqrt(10))");

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sqrtTwoVariations)
{
    checkSuccess(sqrtTwo, parseFrom("sqrt(2)"));
    checkSuccess(sqrtTwo, parseFrom("Sqrt(2)"));
    checkSuccess(sqrtTwo, parseFrom("SQRT(2)"));
}

BOOST_AUTO_TEST_CASE(powerOfInteger)
{
    const ParseResult result = parseFrom("2^3");

    checkSuccess(eight, result);
}

BOOST_AUTO_TEST_CASE(powerOfSymbols)
{
    const ParseResult result = parseFrom("a^b");
    const BasePtr expected = Power::create(a, b);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(powerOfSymbolBaseWithUnaryMinus)
{
    const ParseResult result = parseFrom("-a^2");
    const BasePtr expected = Product::minus(Power::create(a, two));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(powerOfSymbolsWithUselessParentheses)
{
    const ParseResult result = parseFrom("(((((a)))))^((b))");
    const BasePtr expected = Power::create(a, b);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryPlusInParentheses)
{
    const ParseResult result = parseFrom("a^(+b) - (+4) + 1 - 2*(+c)");
    const BasePtr expected = Sum::create(Power::create(a, b), Product::minus(three), Product::minus(two, c));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryMinusWithSumOperator)
{
    const ParseResult result = parseFrom("a +-b");
    const BasePtr expected = Sum::create(a, Product::minus(b));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryPlusWithNumberInSum)
{
    const ParseResult result = parseFrom("a ++2");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(unaryPlusWithNumberAndParenthesesInSum)
{
    const ParseResult result = parseFrom("a +(+2)");
    const BasePtr expected = Sum::create(a, two);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryPlusWithSymbolInSum)
{
    const ParseResult result = parseFrom("a +(+b) ++c");

    check({Sum::create(a, b), true, false}, result);
}

BOOST_AUTO_TEST_CASE(unaryMinusWithPower)
{
    const ParseResult result = parseFrom("a^-2");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(unaryPlusWithPower)
{
    const ParseResult result = parseFrom("a^+2");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(unaryMinusWithPowerInParentheses)
{
    const ParseResult result = parseFrom("a^(-2)");
    const BasePtr expected = Power::create(a, Numeric::create(-2));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sumWithUnaryMinusFirstNumericSummand)
{
    const ParseResult result = parseFrom("-1 + b");
    const BasePtr expected = Sum::create(Numeric::mOne(), b);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sumWithUnaryMinusFirstSymbolSummand)
{
    const ParseResult result = parseFrom("-a + b");
    const BasePtr expected = Sum::create(Product::minus(a), b);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(sumWithPowerSummand)
{
    const ParseResult result = parseFrom("-a + a^(-2)");
    const BasePtr expected = Sum::create(Product::minus(a), Power::create(a, Numeric::create(-2)));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryMinusBeforeProduct)
{
    const ParseResult result = parseFrom("-2*a*b");
    const BasePtr expected = Product::minus(two, a, b);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unaryMinusInParentheses)
{
    const ParseResult result = parseFrom("-23*a + b^(-2) + c + (-d) + 2 +(-4)");
    const BasePtr expected = Sum::create({Product::create(Numeric::create(-23), a),
      Power::create(b, Numeric::create(-2)), c, Product::minus(d), Numeric::create(-2)});

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(posLargeInteger)
{
    const char* intStr("12039182309810923809182093021938409283409820394");
    const BasePtr expected = Numeric::create(Int(intStr));
    const ParseResult result = parseFrom(intStr);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(negLargeInteger)
{
    const char* intStr = "-2039384092840928309482309480980928309482093480923840928309420938";
    const BasePtr expected = Numeric::create(Int(intStr));
    const ParseResult result = parseFrom(intStr);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(posDoubleOutsideOfRange)
{
    const ParseResult result = parseFrom("123.e928377489274892798347982");
    const BasePtr expected = Numeric::create(123);

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(negDoubleOutsideOfRange)
{
    const ParseResult result = parseFrom("-123.e928377489274892798347982");
    const BasePtr expected = Numeric::create(-123);

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(parenthesesAroundSymbol)
{
    const ParseResult result = parseFrom("(a)");

    checkSuccess(a, result);
}

BOOST_AUTO_TEST_CASE(parenthesesAroundNumber)
{
    const ParseResult result = parseFrom("(123)");

    checkSuccess(Numeric::create(123), result);
}

BOOST_AUTO_TEST_CASE(parenthesesAroundSum)
{
    const ParseResult result = parseFrom("(a + b)");

    checkSuccess(Sum::create(a, b), result);
}

BOOST_AUTO_TEST_CASE(parenthesesAroundFunction)
{
    const ParseResult result = parseFrom("a*acos(234)");
    const BasePtr expected = Product::create(a, Trigonometric::createAcos(Numeric::create(234)));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(multipleParenthesesInSum)
{
    const ParseResult result = parseFrom("(((a + (b + c) + (15))))");
    const BasePtr expected = Sum::create(a, b, c, Numeric::create(15));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(productWithSumsInParentheses)
{
    const ParseResult result = parseFrom("a*(b + c) + 4*(a + d)");
    const BasePtr expected =
      Sum::create(Product::create(four, a), Product::create(four, d), Product::create(a, Sum::create(b, c)));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(simpleProductWithSumInParentheses)
{
    const ParseResult result = parseFrom("a*(2 + b)");
    const BasePtr expected = Product::create(a, Sum::create(two, b));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(productWithSumInParentheses)
{
    const ParseResult result = parseFrom("a*(b + c)*5*d");
    const BasePtr expected = Product::create(a, Sum::create(b, c), five, d);

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(unrecognizedTokensWithSyntaxError)
{
    const ParseResult result = parseFrom("-{}*12*sin(b)");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(unrecognizedToken)
{
    const ParseResult result = parseFrom("{12*sin(b)");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(unrecognizedTokensAfterValidExpression)
{
    const BasePtr expected = Product::create(Numeric::create(123), Trigonometric::createAtan(Sum::create(a, b)));
    const ParseResult result = parseFrom("123*atan(a + b){\a}[[");

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(unrecognizedTokensInsideValidExpression)
{
    const ParseResult result = parseFrom("[äüa*b*sqrt(12*c^2 - &c) - 40üä]\\");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(unrecognizedTokensInsideParentheses)
{
    const BasePtr expected = Product::create(Numeric::create(123), Trigonometric::createAtan(Sum::create(a, b)));
    const ParseResult result = parseFrom("123*atan(a + b))*({=}[[");

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(symbolsAndComma)
{
    const ParseResult result = parseFrom("a,b,c,,d");

    check({a, true, false}, result);
}

BOOST_AUTO_TEST_CASE(emptyString)
{
    const ParseResult result = parseFrom("");

    check({undefined, false, true}, result);
}

BOOST_AUTO_TEST_CASE(onlyCommaSigns)
{
    const ParseResult result = parseFrom("(())");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(emptyParenthesesAfterValidExpressionInProduct)
{
    const BasePtr expected = Product::create(Numeric::create(123), Trigonometric::createAtan(Sum::create(a, b)));
    const ParseResult result = parseFrom("123*atan(a + b)*()");

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(syntaxErrorAfterValidExpressionInProduct)
{
    const BasePtr expected = Product::create(Numeric::create(-1, 5), Trigonometric::createAtan(Sum::create(a, b)));
    const ParseResult result = parseFrom("-1/5*atan(a + b)*({)}[[");

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(emptyParentheses)
{
    const ParseResult result = parseFrom("(())");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(mixedOperatorsWithUnaryMinusNumber)
{
    const ParseResult result = parseFrom("a/-3");
    const BasePtr expected = Product::create(a, Numeric::create(-1, 3));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(mixedOperatorsWithUnaryMinusSymbol)
{
    const ParseResult result = parseFrom("a*-b + -c");
    const BasePtr expected = Product::minus(Sum::create(c, Product::create(a, b)));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(emptyParenthesesBeforeValidPart)
{
    const ParseResult result = parseFrom("(())a*b + c");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(mixedTerm01)
{
    const ParseResult result = parseFrom("-a*sin(b)*(c + d)*12*b");
    const BasePtr expected =
      Product::create({Numeric::create(-12), a, b, Trigonometric::createSin(b), Sum::create(c, d)});

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(mixedTerm02)
{
    const std::string_view inputWithFrac("a*atan(1/sqrt(17))*cos(c*d)*sin(a*b)^2*tan(a*b)");
    const std::string_view inputWithoutFrac("a*atan(17^(-1/2))*cos(c*d)*sin(a*b)^2*tan(a*b)");
    const BasePtr expected = Product::create({a, Trigonometric::createAtan(Power::sqrt(Numeric::create(1, 17))),
      Trigonometric::createCos(Product::create(c, d)),
      Power::create(Trigonometric::createSin(Product::create(a, b)), two),
      Trigonometric::createTan(Product::create(a, b))});

    checkSuccess(expected, parseFrom(inputWithFrac));
    checkSuccess(expected, parseFrom(inputWithoutFrac));
}

BOOST_AUTO_TEST_CASE(mixedTerm03)
{
    const ParseResult result = parseFrom("-a^(2/3)*b^(2/3*c - d)*c^((a + b)^2)");
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr expected = Product::create({Numeric::mOne(), Power::create(a, twoThird),
      Power::create(b, Sum::create(Product::create(twoThird, c), Product::minus(d))),
      Power::create(c, Power::create(Sum::create(a, b), two))});

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(powerOperator)
{
    const ParseResult result = parseFrom("a^2^3^2");
    const BasePtr expected = Power::create(a, Numeric::create(512));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(nestedPowerOperator)
{
    const ParseResult result = parseFrom("a^(b^2 + c)^2^3");
    const BasePtr expected = Power::create(a, Power::create(Sum::create(Product::create(b, b), c), eight));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(onlyPowerOperator)
{
    const ParseResult result = parseFrom("^^^");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(onlyPowerOperatorWithParentheses)
{
    const ParseResult result = parseFrom("^(a + b)");

    check({undefined, false, false}, result);
}

BOOST_AUTO_TEST_CASE(misspelledFunction)
{
    const BasePtr expected = Symbol::create("alksdjflkasjf");
    const ParseResult result = parseFrom("alksdjflkasjf(a + b)");

    check({expected, true, false}, result);
}

BOOST_AUTO_TEST_CASE(piVariations)
{
    checkSuccess(pi, parseFrom("pi"));
    checkSuccess(pi, parseFrom("Pi"));
    checkSuccess(pi, parseFrom("PI"));
    checkSuccess(pi, parseFrom("pI"));
}

BOOST_AUTO_TEST_CASE(piInMixedTerm)
{
    const ParseResult result = parseFrom("2*sin(pi) + pi*cos(pI)*sqrt(PI)");
    const BasePtr expected = Product::minus(pi, Power::sqrt(pi));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(eulerVariations)
{
    checkSuccess(euler, parseFrom("euler"));
    checkSuccess(euler, parseFrom("Euler"));
    checkSuccess(euler, parseFrom("EULER"));
    checkSuccess(euler, parseFrom("euLEr"));
    checkSuccess(euler, parseFrom("EuleR"));
}

BOOST_AUTO_TEST_CASE(symbolTimesFunction)
{
    const ParseResult result = parseFrom("a*tan(b)");
    const BasePtr expected = Product::create(a, Trigonometric::createTan(b));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_CASE(logOfEulerEvaluesToOne)
{
    const ParseResult result = parseFrom("a*log(euler)");

    checkSuccess(a, result);
}

BOOST_AUTO_TEST_CASE(eulerInMixedTerm)
{
    const ParseResult result = parseFrom("10*Euler + EULER^2 - a*b*log(euler)");
    const BasePtr expected =
      Sum::create(Product::create(ten, euler), Product::create(euler, euler), Product::minus(a, b));

    checkSuccess(expected, result);
}

BOOST_AUTO_TEST_SUITE_END()
