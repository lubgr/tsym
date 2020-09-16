
#include "constant.h"
#include "fixtures.h"
#include "numeric.h"
#include "plaintextprintengine.h"
#include "power.h"
#include "printer.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

struct PrinterFixture : public AbcFixture {
    std::stringstream stream;

    template <class Engine, class T> std::string print(Engine& engine, const T& toBePrinted)
    {
        tsym::print(engine, toBePrinted);

        return stream.str();
    }

    template <class T> std::string print(const T& toBePrinted)
    {
        PlaintextPrintEngine engine(stream);

        return print(engine, toBePrinted);
    }

    std::string printDebug(const BasePtr& toBePrinted)
    {
        PlaintextPrintEngine engine(stream);

        tsym::printDebug(engine, *toBePrinted);

        return stream.str();
    }
};

BOOST_FIXTURE_TEST_SUITE(TestPrinter, PrinterFixture)

BOOST_AUTO_TEST_CASE(positiveIntNumber)
{
    BOOST_CHECK_EQUAL("5", print(*five));
}

BOOST_AUTO_TEST_CASE(negativeIntNumber)
{
    BOOST_CHECK_EQUAL("-5", print(-5));
}

BOOST_AUTO_TEST_CASE(doubleNumber)
{
    BOOST_CHECK_EQUAL("0.123456", print(0.123456));
}

BOOST_AUTO_TEST_CASE(fractionNumber)
{
    BOOST_CHECK_EQUAL("2/33", print(Number(2, 33)));
}

BOOST_AUTO_TEST_CASE(operatorWithNumber)
{
    const Number frac(-4, 17);

    stream << frac;

    BOOST_CHECK_EQUAL("-4/17", stream.str());
}

BOOST_AUTO_TEST_CASE(piUnicode)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "\u03c0";
#else
      "pi";
#endif
    BOOST_CHECK_EQUAL(expected, print(*pi));
}

BOOST_AUTO_TEST_CASE(piAscii)
{
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);

    BOOST_CHECK_EQUAL("pi", print(engine, *pi));
}

BOOST_AUTO_TEST_CASE(euler)
{
    BOOST_CHECK_EQUAL("e", print(*Constant::createE()));
}

BOOST_AUTO_TEST_CASE(function)
{
    BOOST_CHECK_EQUAL("sin(a)", print(*Trigonometric::createSin(a)));
}

BOOST_AUTO_TEST_CASE(functionWithMoreThanOneArgument)
{
    const BasePtr atan2 = Trigonometric::createAtan2(Product::create(two, a), b);

    BOOST_CHECK_EQUAL("atan2(2*a, b)", print(*atan2));
}

BOOST_AUTO_TEST_CASE(symbol)
{
    const std::string name("abcde");

    BOOST_CHECK_EQUAL(name, print(*Symbol::create(name)));
}

BOOST_AUTO_TEST_CASE(positiveSymbol)
{
    const BasePtr aPos = Symbol::createPositive("a");

    BOOST_CHECK_EQUAL("a", print(*aPos));
}

BOOST_AUTO_TEST_CASE(positiveSymbolWithSubAndSuperscript)
{
    const std::string expected = "a_b^c";
    const Name name{"a", "b", "c"};
    const BasePtr aPos = Symbol::createPositive(name);

    BOOST_CHECK_EQUAL(expected, print(*aPos));
}

BOOST_AUTO_TEST_CASE(positiveSymbolAsciiCharset)
{
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);
    const BasePtr aPos = Symbol::createPositive("a");

    BOOST_CHECK_EQUAL("a", print(engine, *aPos));
}

BOOST_AUTO_TEST_CASE(symbolGreekLetterWithoutUnicode)
{
    const std::string name("omega");
    const BasePtr omega = Symbol::create(name);
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);

    BOOST_CHECK_EQUAL(name, print(engine, *omega));
}

BOOST_AUTO_TEST_CASE(symbolGreekLetterWithUnicode)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "\u03C9";
#else
      "omega";
#endif
    const BasePtr omega = Symbol::create("omega");

    BOOST_CHECK_EQUAL(expected, print(*omega));
}

BOOST_AUTO_TEST_CASE(capitalOmega)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "\u03a9";
#else
      "Omega";
#endif
    const BasePtr omega = Symbol::create("Omega");

    BOOST_CHECK_EQUAL(expected, print(*omega));
}

BOOST_AUTO_TEST_CASE(lowerCaseAlpha)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "\u03B1";
#else
      "alpha";
#endif
    const BasePtr alpha = Symbol::create("alpha");

    BOOST_CHECK_EQUAL(expected, print(*alpha));
}

BOOST_AUTO_TEST_CASE(upperCaseAlpha)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "\u0391";
#else
      "Alpha";
#endif
    const BasePtr capitalAlpha = Symbol::create("Alpha");

    BOOST_CHECK_EQUAL(expected, print(*capitalAlpha));
}

BOOST_AUTO_TEST_CASE(sumWithFunction)
{
    const BasePtr sum = Sum::create(a, Trigonometric::createTan(c), Trigonometric::createAcos(b));

    BOOST_CHECK_EQUAL("a + acos(b) + tan(c)", print(*sum));
}

BOOST_AUTO_TEST_CASE(product)
{
    const BasePtr product = Product::create(a, b, c, d);

    BOOST_CHECK_EQUAL("a*b*c*d", print(*product));
}

BOOST_AUTO_TEST_CASE(negSymbolAsProduct)
{
    BOOST_CHECK_EQUAL("-a", print(*Product::minus(a)));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndPositiveInteger)
{
    const BasePtr pow = Power::create(a, two);

    BOOST_CHECK_EQUAL("a^2", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndNegInt)
{
    const BasePtr pow = Power::create(a, Numeric::create(-3));

    BOOST_CHECK_EQUAL("1/a^3", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndMinusOne)
{
    const BasePtr pow = Power::create(a, Numeric::mOne());

    BOOST_CHECK_EQUAL("1/a", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndMinusOneDebugPrint)
{
    const BasePtr pow = Power::create(a, Numeric::mOne());

    BOOST_CHECK_EQUAL("a^(-1)", printDebug(pow));
}

BOOST_AUTO_TEST_CASE(powerOfProductAndMinusOne)
{
    const BasePtr pow = Power::create(Product::create(two, a, b), Numeric::mOne());

    BOOST_CHECK_EQUAL("1/(2*a*b)", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfPowerOfPowerOfPower)
{
    const BasePtr pow1 = Power::create(a, b);
    const BasePtr pow2 = Power::create(pow1, c);
    const BasePtr pow3 = Power::create(pow2, Product::create(Numeric::create(-1, 4), a));
    const BasePtr pow4 = Power::create(pow3, d);

    BOOST_CHECK_EQUAL("(((a^b)^c)^(-1/4*a))^d", print(*pow4));
}

BOOST_AUTO_TEST_CASE(omitFirstNumeratorFactorIfOne)
{
    const BasePtr product = Product::create(c, Power::create(Product::create(two, a, b), Numeric::mOne()));

    BOOST_CHECK_EQUAL("c/(2*a*b)", print(*product));
}

BOOST_AUTO_TEST_CASE(omitFirstNumeratorFactorIfMinusOne)
{
    const BasePtr product = Product::minus(c, Power::create(Product::create(two, a, b), Numeric::mOne()));

    BOOST_CHECK_EQUAL("-c/(2*a*b)", print(*product));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndPosFrac)
{
    const BasePtr pow = Power::create(a, Numeric::fourth());

    BOOST_CHECK_EQUAL("a^(1/4)", print(*pow));
}

BOOST_AUTO_TEST_CASE(sqrtPower)
{
    const BasePtr pow = Power::sqrt(Product::create(a, b));

    BOOST_CHECK_EQUAL("sqrt(a*b)", print(*pow));
}

BOOST_AUTO_TEST_CASE(oneOverSqrtPowerDebugPrint)
{
    const BasePtr exp = Numeric::create(-1, 2);
    const BasePtr product = Product::create(Power::create(a, exp), Power::create(b, exp));

    BOOST_CHECK_EQUAL("a^(-1/2)*b^(-1/2)", printDebug(product));
}

BOOST_AUTO_TEST_CASE(oneOverSqrtPower)
{
    const BasePtr exp = Numeric::create(-1, 2);
    const BasePtr product = Product::create(Power::create(a, exp), Power::create(b, exp));

    BOOST_CHECK_EQUAL("1/(sqrt(a)*sqrt(b))", print(*product));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndNegFracDebugPrint)
{
    const BasePtr pow = Power::create(a, Numeric::create(-2, 3));

    BOOST_CHECK_EQUAL("a^(-2/3)", printDebug(pow));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndNegFrac)
{
    const BasePtr pow = Power::create(a, Numeric::create(-2, 3));

    BOOST_CHECK_EQUAL("1/a^(2/3)", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfFraction)
{
    const BasePtr n = Numeric::create(5, 7);
    const BasePtr pow = Power::create(n, a);

    BOOST_CHECK_EQUAL("(5/7)^a", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerWithPiBase)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "\u03c0^(a + b)";
#else
      "pi^(a + b)";
#endif
    const BasePtr pow = Power::create(pi, Sum::create(a, b));

    BOOST_CHECK_EQUAL(expected, print(*pow));
}

BOOST_AUTO_TEST_CASE(powerWithPiExp)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "(a + b)^\u03c0";
#else
      "(a + b)^pi";
#endif
    const BasePtr pow = Power::create(Sum::create(a, b), pi);

    BOOST_CHECK_EQUAL(expected, print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfSymbolAndSymbol)
{
    const BasePtr pow = Power::create(a, b);

    BOOST_CHECK_EQUAL("a^b", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfSumAndNumber)
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow = Power::create(sum, two);

    BOOST_CHECK_EQUAL("(a + b)^2", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfNumberAndSum)
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow = Power::create(two, sum);

    BOOST_CHECK_EQUAL("2^(a + b)", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfFunctionAndNumber)
{
    const BasePtr pow = Power::create(Trigonometric::createSin(a), two);

    BOOST_CHECK_EQUAL("sin(a)^2", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfSumAndFunction)
{
    const BasePtr sum = Sum::create(two, b, Trigonometric::createSin(a));
    const BasePtr pow = Power::create(sum, Trigonometric::createAsin(Numeric::create(1, 5)));

    BOOST_CHECK_EQUAL("(2 + b + sin(a))^asin(1/5)", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfProductAndNumber)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, two);

    BOOST_CHECK_EQUAL("a^2*b^2", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfProductAndNegNumber)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, Numeric::create(-2));

    BOOST_CHECK_EQUAL("1/(a^2*b^2)", print(*pow));
}

BOOST_AUTO_TEST_CASE(powerOfProductAndNegNumberDebugPrint)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, Numeric::create(-4));

    BOOST_CHECK_EQUAL("a^(-4)*b^(-4)", printDebug(pow));
}

BOOST_AUTO_TEST_CASE(largeProductOfPowers)
{
    const BasePtr product = Product::create(
      {a, b, Sum::create(a, c), Power::create(f, a), Power::oneOver(d), Power::create(e, Numeric::create(-2))});

    BOOST_CHECK_EQUAL("a*b*(a + c)*f^a/(d*e^2)", print(*product));
}

BOOST_AUTO_TEST_CASE(largeProductOfPowersDebugPrint)
{
    const BasePtr product = Product::create(
      {a, b, Sum::create(a, c), Power::create(f, a), Power::oneOver(d), Power::create(e, Numeric::create(-2))});

    BOOST_CHECK_EQUAL("a*b*(a + c)*d^(-1)*e^(-2)*f^a", printDebug(product));
}

BOOST_AUTO_TEST_CASE(simpleDivisionOfSymbols)
{
    const BasePtr product = Product::create(a, Power::oneOver(b));

    BOOST_CHECK_EQUAL("a/b", print(*product));
}

BOOST_AUTO_TEST_CASE(simpleDivisionOfSymbolsDebugPrint)
{
    const BasePtr product = Product::create(a, Power::oneOver(b));

    BOOST_CHECK_EQUAL("a*b^(-1)", printDebug(product));
}

BOOST_AUTO_TEST_CASE(negProductFactorMinusOne)
{
    const BasePtr product = Product::minus(a, b);

    BOOST_CHECK_EQUAL("-a*b", print(*product));
}

BOOST_AUTO_TEST_CASE(negProductNonTrivialFactor)
{
    const BasePtr product = Product::create(a, b, Numeric::create(-2));

    BOOST_CHECK_EQUAL("-2*a*b", print(*product));
}

BOOST_AUTO_TEST_CASE(productWithConstantPi)
{
    const std::string expected =
#ifndef TSYM_ASCII_ONLY
      "-2*\u03c0*a*b";
#else
      "-2*pi*a*b";
#endif
    const BasePtr product = Product::create({Numeric::create(-2), a, b, pi});

    BOOST_CHECK_EQUAL(expected, print(*product));
}

BOOST_AUTO_TEST_CASE(productOfEqualExpPowers)
{
    const BasePtr product = Product::create(Power::sqrt(a), Power::sqrt(b));

    BOOST_CHECK_EQUAL("sqrt(a)*sqrt(b)", print(*product));
}

BOOST_AUTO_TEST_CASE(negProductOfEqualExpPowers)
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr product = Product::create({Numeric::mOne(), Power::create(a, exp), Power::create(b, exp)});

    BOOST_CHECK_EQUAL("-a^(2/3)*b^(2/3)", print(*product));
}

BOOST_AUTO_TEST_CASE(productOfFunctions)
{
    BasePtrList fac;

    fac.push_back(a);
    fac.push_back(Trigonometric::createAtan(Power::create(Numeric::create(17), Numeric::create(-1, 2))));
    fac.push_back(Trigonometric::createCos(Product::create(c, d)));
    fac.push_back(Power::create(Trigonometric::createSin(Product::create(a, b)), two));
    fac.push_back(Trigonometric::createTan(Product::create(a, b)));

    const BasePtr product = Product::create(fac);

    BOOST_CHECK_EQUAL("a*atan(1/sqrt(17))*cos(c*d)*sin(a*b)^3/cos(a*b)", print(*product));
}

BOOST_AUTO_TEST_CASE(productOfFunctionsDebugPrint)
{
    BasePtrList fac;

    fac.push_back(a);
    fac.push_back(Trigonometric::createAtan(Power::create(Numeric::create(17), Numeric::create(-1, 2))));
    fac.push_back(Trigonometric::createCos(Product::create(c, d)));
    fac.push_back(Power::create(Trigonometric::createSin(Product::create(a, b)), two));
    fac.push_back(Trigonometric::createTan(Product::create(a, b)));

    const BasePtr product = Product::create(fac);

    BOOST_CHECK_EQUAL("a*atan(17^(-1/2))*cos(a*b)^(-1)*cos(c*d)*sin(a*b)^3", printDebug(product));
}

BOOST_AUTO_TEST_CASE(fracOfSumAndProduct)
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr product = Product::create(c, d);
    const BasePtr frac = Product::create(sum, Power::oneOver(product));

    BOOST_CHECK_EQUAL("(a + b)/(c*d)", print(*frac));
}

BOOST_AUTO_TEST_CASE(fracOfTwoProducts)
{
    const BasePtr prod1 = Product::create(a, b);
    const BasePtr prod2 = Product::create(c, d);
    const BasePtr frac = Product::create(prod1, Power::oneOver(prod2));

    BOOST_CHECK_EQUAL("a*b/(c*d)", print(*frac));
}

BOOST_AUTO_TEST_CASE(fracOfPowerAndSum)
{
    const BasePtr pow = Power::create(a, b);
    const BasePtr sum = Sum::create(c, d);
    const BasePtr frac = Product::create(pow, Power::oneOver(sum));

    BOOST_CHECK_EQUAL("a^b/(c + d)", print(*frac));
}

BOOST_AUTO_TEST_CASE(negTermsInSum)
{
    const BasePtr sum = Sum::create(a, Product::minus(b));

    BOOST_CHECK_EQUAL("a - b", print(*sum));
}

BOOST_AUTO_TEST_CASE(posProductInSum)
{
    const BasePtr sum = Sum::create(a, Product::create(b, c));

    BOOST_CHECK_EQUAL("a + b*c", print(*sum));
}

BOOST_AUTO_TEST_CASE(negSumInProduct)
{
    const BasePtr product = Product::create(a, Sum::create(b, c));

    BOOST_CHECK_EQUAL("a*(b + c)", print(*product));
}

BOOST_AUTO_TEST_CASE(posSumInProduct)
{
    const BasePtr product = Product::create(a, Sum::create(Product::minus(b), c));

    BOOST_CHECK_EQUAL("a*(-b + c)", print(*product));
}

BOOST_AUTO_TEST_CASE(negativePowerWithConstantBase)
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr product = Product::minus(Power::create(Constant::createE(), exp));
    const std::string result = print(*product);

    BOOST_CHECK_EQUAL("-1/e^123", result);
}

BOOST_AUTO_TEST_CASE(negativePowerWithConstantBaseDebug)
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr product = Product::minus(Power::create(Constant::createE(), exp));

    BOOST_CHECK_EQUAL("-e^(-123)", printDebug(product));
}

BOOST_AUTO_TEST_CASE(parenthesesInPosProductWithNegSumFactor)
{
    const BasePtr product = Product::create(c, Sum::create(Product::minus(a), Numeric::create(-3)));
    const std::string result = print(*product);

    BOOST_CHECK_EQUAL("(-3 - a)*c", result);
}

BOOST_AUTO_TEST_CASE(parenthesesInNegProductWithSumFactor)
{
    const BasePtr product = Product::minus(c, Sum::create(Product::minus(a), Numeric::create(-3)));
    const std::string result = print(*product);

    BOOST_CHECK_EQUAL("-(-3 - a)*c", result);
}

BOOST_AUTO_TEST_CASE(parenthesesInNegProductWithSumFactorInsideSum)
{
    const BasePtr sum = Sum::create(a, Product::minus(c, Sum::create(Product::minus(a), Numeric::create(-3))));
    const std::string result = print(*sum);

    BOOST_CHECK_EQUAL("a - (-3 - a)*c", result);
}

BOOST_AUTO_TEST_CASE(parenthesesInNegProductWithSumFactorInsideLargerSum)
{
    const BasePtr mThree = Numeric::create(-3);
    const BasePtr sum = Sum::create(b, Product::create(mThree, c), Product::minus(a, c),
      Product::create(Numeric::mOne(), Sum::create(mThree, Product::minus(a)), c));
    const std::string result = print(*sum);

    BOOST_CHECK_EQUAL("b - 3*c - a*c - (-3 - a)*c", result);
}

BOOST_AUTO_TEST_SUITE_END()
