
#include "abc.h"
#include "printer.h"
#include "symbol.h"
#include "numeric.h"
#include "power.h"
#include "constant.h"
#include "trigonometric.h"
#include "product.h"
#include "sum.h"
#include "plaintextprintengine.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Printer)
{
    const BasePtr pi = Constant::createPi();
    std::stringstream stream;

    void setup()
    {
        stream.str("");
    }

    template<class Engine, class T> std::string print(Engine& engine, const T& toBePrinted)
    {
        printer::print(engine, toBePrinted);

        return stream.str();
    }

    template<class T> std::string print(const T& toBePrinted)
    {
        PlaintextPrintEngine engine(stream);

        return print(engine, toBePrinted);
    }

    std::string printDebug(const BasePtr& toBePrinted)
    {
        PlaintextPrintEngine engine(stream);

        printer::printDebug(engine, *toBePrinted);

        return stream.str();
    }
};

TEST(Printer, positiveIntNumber)
{
    CHECK_EQUAL("5", print(five));
}

TEST(Printer, negativeIntNumber)
{
    CHECK_EQUAL("-5", print(-5));
}

TEST(Printer, doubleNumber)
{
    CHECK_EQUAL("0.123456", print(0.123456));
}

TEST(Printer, undefinedNumber)
{
    CHECK_EQUAL("Undefined", print(Number::createUndefined()));
}

TEST(Printer, fractionNumber)
{
    CHECK_EQUAL("2/33", print(Number(2, 33)));
}

TEST(Printer, operatorWithNumber)
{
    const Number frac(-4, 17);

    stream << frac;

    CHECK_EQUAL("-4/17", stream.str());
}

TEST(Printer, piUnicode)
{
    CHECK_EQUAL("\u03c0", print(pi));
}

TEST(Printer, piAscii)
{
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);

    CHECK_EQUAL("pi", print(engine, pi));
}

TEST(Printer, e)
{
    CHECK_EQUAL("e", print(Constant::createE()));
}

TEST(Printer, function)
{
    CHECK_EQUAL("sin(a)", print(Trigonometric::createSin(a)));
}

TEST(Printer, functionWithMoreThanOneArgument)
{
    const BasePtr atan2 = Trigonometric::createAtan2(Product::create(two, a), b);

    CHECK_EQUAL("atan2(2*a, b)", print(atan2));
}

TEST(Printer, symbol)
{
    const std::string name("abcde");

    CHECK_EQUAL(name, print(Symbol::create(name)));
}

TEST(Printer, positiveSymbol)
{
    const BasePtr aPos = Symbol::createPositive("a");

    CHECK_EQUAL("a\u208A", print(aPos));
}

TEST(Printer, positiveSymbolWithSubAndSuperscript)
{
    const Name name("a", "b", "c");
    const BasePtr aPos = Symbol::createPositive(name);

    CHECK_EQUAL("a_b_c\u208A", print(aPos));
}

TEST(Printer, positiveSymbolUtf8Disabled)
{
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);
    const BasePtr aPos = Symbol::createPositive("a");

    CHECK_EQUAL("a", print(engine, aPos));
}

TEST(Printer, symbolGreekLetterWithoutUnicode)
{
    const std::string name("omega");
    const BasePtr omega = Symbol::create(name);
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);

    CHECK_EQUAL(name, print(engine, omega));
}

TEST(Printer, symbolGreekLetterWithUnicode)
{
    const BasePtr omega = Symbol::create("omega");

    CHECK_EQUAL("\u03C9", print(omega));
}

TEST(Printer, capitalOmega)
{
    const BasePtr omega = Symbol::create("Omega");

    CHECK_EQUAL("\u03a9", print(omega));
}

TEST(Printer, lowerCaseAlpha)
{
    const BasePtr alpha = Symbol::create("alpha");

    CHECK_EQUAL("\u03B1", print(alpha));
}

TEST(Printer, upperCaseAlpha)
{
    const BasePtr capitalAlpha = Symbol::create("Alpha");
    
    CHECK_EQUAL("\u0391", print(capitalAlpha));
}

TEST(Printer, sumWithPi)
{
    const BasePtr sum = Sum::create(two, a, b, pi);

    CHECK_EQUAL("2 + \u03c0 + a + b", print(sum));
}

TEST(Printer, sumWithFunction)
{
    const BasePtr sum = Sum::create(a, Trigonometric::createTan(c), Trigonometric::createAcos(b));

    CHECK_EQUAL("a + acos(b) + tan(c)", print(sum));
}

TEST(Printer, product)
{
    const BasePtr product = Product::create(a, b, c, d);

    CHECK_EQUAL("a*b*c*d", print(product));
}

TEST(Printer, negSymbolAsProduct)
{
    CHECK_EQUAL("-a", print(Product::minus(a)));
}

TEST(Printer, powerOfSymbolAndPositiveInteger)
{
    const BasePtr pow = Power::create(a, two);

    CHECK_EQUAL("a^2", print(pow));
}

TEST(Printer, powerOfSymbolAndNegInt)
{
    const BasePtr pow = Power::create(a, Numeric::create(-3));

    CHECK_EQUAL("1/a^3", print(pow));
}

TEST(Printer, powerOfSymbolAndMinusOne)
{
    const BasePtr pow = Power::create(a, Numeric::mOne());

    CHECK_EQUAL("1/a", print(pow));
}

TEST(Printer, powerOfSymbolAndMinusOneDebugPrint)
{
    const BasePtr pow = Power::create(a, Numeric::mOne());

    CHECK_EQUAL("a^(-1)", printDebug(pow));
}

TEST(Printer, powerOfProductAndMinusOne)
{
    const BasePtr pow = Power::create(Product::create(two, a, b), Numeric::mOne());

    CHECK_EQUAL("1/(2*a*b)", print(pow));
}

TEST(Printer, powerOfPowerOfPowerOfPower)
{
    const BasePtr pow1 = Power::create(a, b);
    const BasePtr pow2 = Power::create(pow1, c);
    const BasePtr pow3 = Power::create(pow2, Product::create(Numeric::create(-1, 4),
                Constant::createPi()));
    const BasePtr pow4 = Power::create(pow3, d);

    CHECK_EQUAL("(((a^b)^c)^(-1/4*\u03c0))^d", print(pow4));
}

TEST(Printer, omitFirstNumeratorFactorIfOne)
{
    const BasePtr product = Product::create(c,
            Power::create(Product::create(two, a, b), Numeric::mOne()));

    CHECK_EQUAL("c/(2*a*b)", print(product));
}

TEST(Printer, omitFirstNumeratorFactorIfMinusOne)
{
    const BasePtr product = Product::minus(c,
            Power::create(Product::create(two, a, b), Numeric::mOne()));

    CHECK_EQUAL("-c/(2*a*b)", print(product));
}

TEST(Printer, powerOfSymbolAndPosFrac)
{
    const BasePtr pow = Power::create(a, Numeric::fourth());

    CHECK_EQUAL("a^(1/4)", print(pow));
}

TEST(Printer, sqrtPower)
{
    const BasePtr pow = Power::sqrt(Product::create(a, b));

    CHECK_EQUAL("sqrt(a*b)", print(pow));
}

TEST(Printer, oneOverSqrtPowerDebugPrint)
{
    const std::string expectedNoFrac();
    const BasePtr exp = Numeric::create(-1, 2);
    const BasePtr product = Product::create(Power::create(a, exp), Power::create(b, exp));
    
    CHECK_EQUAL("a^(-1/2)*b^(-1/2)", printDebug(product));
}

TEST(Printer, oneOverSqrtPower)
{
    const BasePtr exp = Numeric::create(-1, 2);
    const BasePtr product = Product::create(Power::create(a, exp), Power::create(b, exp));

    CHECK_EQUAL("1/(sqrt(a)*sqrt(b))", print(product));
}

TEST(Printer, powerOfSymbolAndNegFracDebugPrint)
{
    const BasePtr pow = Power::create(a, Numeric::create(-2, 3));

    CHECK_EQUAL("a^(-2/3)", printDebug(pow));
}

TEST(Printer, powerOfSymbolAndNegFrac)
{
    const BasePtr pow = Power::create(a, Numeric::create(-2, 3));

    CHECK_EQUAL("1/a^(2/3)", print(pow));
}

TEST(Printer, powerOfFraction)
{
    const BasePtr n = Numeric::create(5, 7);
    const BasePtr pow = Power::create(n, a);

    CHECK_EQUAL("(5/7)^a", print(pow));
}

TEST(Printer, powerWithPiBase)
{
    const BasePtr pow = Power::create(pi, Sum::create(a, b));

    CHECK_EQUAL("\u03c0^(a + b)", print(pow));
}

TEST(Printer, powerWithPiExp)
{
    const std::string expected();
    const BasePtr pow = Power::create(Sum::create(a, b), pi);

    CHECK_EQUAL("(a + b)^\u03c0", print(pow));
}

TEST(Printer, powerOfSymbolAndSymbol)
{
    const BasePtr pow = Power::create(a, b);

    CHECK_EQUAL("a^b", print(pow));
}

TEST(Printer, powerOfSumAndNumber)
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow = Power::create(sum, two);

    CHECK_EQUAL("(a + b)^2", print(pow));
}

TEST(Printer, powerOfNumberAndSum)
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow = Power::create(two, sum);

    CHECK_EQUAL("2^(a + b)", print(pow));
}

TEST(Printer, powerOfFunctionAndNumber)
{
    const BasePtr pow = Power::create(Trigonometric::createSin(a), two);

    CHECK_EQUAL("sin(a)^2", print(pow));
}

TEST(Printer, powerOfSumAndFunction)
{
    const BasePtr sum = Sum::create(two, b, Trigonometric::createSin(a));
    const BasePtr pow = Power::create(sum, Trigonometric::createAsin(Numeric::create(1, 5)));

    CHECK_EQUAL("(2 + b + sin(a))^asin(1/5)", print(pow));
}

TEST(Printer, powerOfProductAndNumber)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, two);

    CHECK_EQUAL("a^2*b^2", print(pow));
}

TEST(Printer, powerOfProductAndNegNumber)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, Numeric::create(-2));

    CHECK_EQUAL("1/(a^2*b^2)", print(pow));
}

TEST(Printer, powerOfProductAndNegNumberDebugPrint)
{
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, Numeric::create(-4));

    CHECK_EQUAL("a^(-4)*b^(-4)", printDebug(pow));
}

TEST(Printer, largeProductOfPowers)
{
    const BasePtr product = Product::create({ a, b, Sum::create(a, c), Power::create(f, a),
            Power::oneOver(d), Power::create(e, Numeric::create(-2)) });

    CHECK_EQUAL("a*b*(a + c)*f^a/(d*e^2)", print(product));
}

TEST(Printer, largeProductOfPowersDebugPrint)
{
    const BasePtr product = Product::create({ a, b, Sum::create(a, c), Power::create(f, a),
            Power::oneOver(d), Power::create(e, Numeric::create(-2)) });
    
    CHECK_EQUAL("a*b*(a + c)*d^(-1)*e^(-2)*f^a", printDebug(product));
}

TEST(Printer, simpleDivisionOfSymbols)
{
    const BasePtr product = Product::create(a, Power::oneOver(b));

    CHECK_EQUAL("a/b", print(product));
}

TEST(Printer, simpleDivisionOfSymbolsDebugPrint)
{
    const BasePtr product = Product::create(a, Power::oneOver(b));

    CHECK_EQUAL("a*b^(-1)", printDebug(product));
}

TEST(Printer, negProductFactorMinusOne)
{
    const BasePtr product = Product::minus(a, b);

    CHECK_EQUAL("-a*b", print(product));
}

TEST(Printer, negProductNonTrivialFactor)
{
    const BasePtr product = Product::create(a, b, Numeric::create(-2));

    CHECK_EQUAL("-2*a*b", print(product));
}

TEST(Printer, productWithConstantPi)
{
    const BasePtr product = Product::create({ Numeric::create(-2), a, b, pi });

    CHECK_EQUAL("-2*\u03c0*a*b", print(product));
}

TEST(Printer, productOfEqualExpPowers)
{
    const BasePtr product = Product::create(Power::sqrt(a), Power::sqrt(b));

    CHECK_EQUAL("sqrt(a)*sqrt(b)", print(product));
}

TEST(Printer, negProductOfEqualExpPowers)
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr product = Product::create({ Numeric::mOne(), Power::create(a, exp),
            Power::create(b, exp) });

    CHECK_EQUAL("-a^(2/3)*b^(2/3)", print(product));
}

TEST(Printer, productOfFunctions)
{
    BasePtrList fac;

    fac.push_back(a);
    fac.push_back(Trigonometric::createAtan(Power::create(Numeric::create(17),
                    Numeric::create(-1, 2))));
    fac.push_back(Trigonometric::createCos(Product::create(c, d)));
    fac.push_back(Power::create(Trigonometric::createSin(Product::create(a, b)), two));
    fac.push_back(Trigonometric::createTan(Product::create(a, b)));

    const BasePtr product = Product::create(fac);
    
    CHECK_EQUAL("a*atan(1/sqrt(17))*cos(c*d)*sin(a*b)^3/cos(a*b)", print(product));
}

TEST(Printer, productOfFunctionsDebugPrint)
{
    BasePtrList fac;

    fac.push_back(a);
    fac.push_back(Trigonometric::createAtan(Power::create(Numeric::create(17),
                    Numeric::create(-1, 2))));
    fac.push_back(Trigonometric::createCos(Product::create(c, d)));
    fac.push_back(Power::create(Trigonometric::createSin(Product::create(a, b)), two));
    fac.push_back(Trigonometric::createTan(Product::create(a, b)));

    const BasePtr product = Product::create(fac);

    CHECK_EQUAL("a*atan(17^(-1/2))*cos(a*b)^(-1)*cos(c*d)*sin(a*b)^3", printDebug(product));
}

TEST(Printer, fracOfSumAndProduct)
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr product = Product::create(c, d);
    const BasePtr frac = Product::create(sum, Power::oneOver(product));

    CHECK_EQUAL("(a + b)/(c*d)", print(frac));
}

TEST(Printer, fracOfTwoProducts)
{
    const BasePtr prod1 = Product::create(a, b);
    const BasePtr prod2 = Product::create(c, d);
    const BasePtr frac = Product::create(prod1, Power::oneOver(prod2));

    CHECK_EQUAL("a*b/(c*d)", print(frac));
}

TEST(Printer, fracOfPowerAndSum)
{
    const BasePtr pow = Power::create(a, b);
    const BasePtr sum = Sum::create(c, d);
    const BasePtr frac = Product::create(pow, Power::oneOver(sum));

    CHECK_EQUAL("a^b/(c + d)", print(frac));
}

TEST(Printer, negTermsInSum)
{
    const BasePtr sum = Sum::create(a, Product::minus(b));

    CHECK_EQUAL("a - b", print(sum));
}

TEST(Printer, posProductInSum)
{
    const BasePtr sum = Sum::create(a, Product::create(b, c));

    CHECK_EQUAL("a + b*c", print(sum));
}

TEST(Printer, negSumInProduct)
{
    const BasePtr product = Product::create(a, Sum::create(b, c));

    CHECK_EQUAL("a*(b + c)", print(product));
}

TEST(Printer, posSumInProduct)
{
    const BasePtr product = Product::create(a, Sum::create(Product::minus(b), c));

    CHECK_EQUAL("a*(-b + c)", print(product));
}
