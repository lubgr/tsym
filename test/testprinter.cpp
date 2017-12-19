
#include "abc.h"
#include "printer.h"
#include "symbol.h"
#include "numeric.h"
#include "power.h"
#include "constant.h"
#include "trigonometric.h"
#include "product.h"
#include "sum.h"
#include "vector.h"
#include "matrix.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Printer)
{
    std::stringstream stream;
    Printer printer;

    void setup()
    {
        stream.str("");

        printer.enableFractions();
        printer.enableUtf8();
    }

    void teardown()
    {
        printer.disableFractions();
    }
};

TEST(Printer, positiveIntNumber)
{
    const Printer printer(five);
    const std::string expected("5");

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, negativeIntNumber)
{
    const Number minusFive = Number(-5);
    const std::string expected("-5");

    printer.set(minusFive);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, doubleNumber)
{
    const std::string expected("0.123456");
    const Number n(0.123456);

    printer.set(n);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, undefinedNumber)
{
    const std::string expected("Undefined");
    Number u;

    disableLog();
    u = Number(1, 0);
    enableLog();

    printer.set(u);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, fractionNumber)
{
    const std::string expected("2/33");
    const Number n(2, 33);

    printer.set(n);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, operatorWithNumber)
{
    const std::string expected("-4/17");
    const Number frac(-4, 17);
    std::stringstream stream;

    stream << frac;

    CHECK_EQUAL(expected, stream.str());
}

TEST(Printer, pi)
{
    printer.set(Constant::createPi());

    CHECK_EQUAL("\u03c0", printer.getStr());

    Printer::disableUtf8();

    printer.set(Constant::createPi());

    CHECK_EQUAL("pi", printer.getStr());
}

TEST(Printer, e)
{
    const std::string expected("e");

    printer.set(Constant::createE());

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, function)
{
    const std::string expected("sin(a)");
    const BasePtr sin = Trigonometric::createSin(a);

    printer.set(sin);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, functionWithMoreThanOneArgument)
{
    const std::string expected("atan2(2*a, b)");
    const BasePtr atan2 = Trigonometric::createAtan2(Product::create(two, a), b);

    printer.set(atan2);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, symbol)
{
    const std::string expected("abcde");
    const Var sym("abcde");
    Printer printer(sym);

    CHECK_EQUAL(expected, printer.getStr());

    printer.set(sym);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, positiveSymbol)
{
    const std::string expected("a\u208A");
    const BasePtr aPos = Symbol::createPositive("a");

    printer.set(aPos);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, positiveSymbolWithSubAndSuperscript)
{
    const std::string expected("a_b_c\u208A");
    const Name name("a", "b", "c");
    const BasePtr aPos = Symbol::createPositive(name);

    printer.set(aPos);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, positiveSymbolUtf8Disabled)
{
    const BasePtr aPos = Symbol::createPositive("a");

    printer.disableUtf8();

    printer.set(aPos);

    CHECK_EQUAL("a", printer.getStr());
}

TEST(Printer, symbolGreekLetterWithAndWithoutUtf8)
{
    const BasePtr omega = Symbol::create("omega");

    printer.set(omega);

    CHECK_EQUAL("\u03C9", printer.getStr());

    printer.disableUtf8();

    printer.set(omega);

    CHECK_EQUAL("omega", printer.getStr());
}

TEST(Printer, capitalOmega)
{
    const BasePtr omega = Symbol::create("Omega");

    printer.set(omega);

    CHECK_EQUAL("\u03a9", printer.getStr());
}

TEST(Printer, alpha)
{
    const BasePtr alpha = Symbol::create("alpha");
    const BasePtr capitalAlpha = Symbol::create("Alpha");

    printer.set(alpha);

    CHECK_EQUAL("\u03B1", printer.getStr());

    printer.set(capitalAlpha);

    CHECK_EQUAL("\u0391", printer.getStr());
}

TEST(Printer, symbolAsVar)
{
    const std::string expected("a");
    const Var s("a");

    printer.set(s);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, sumWithConstantPi)
{
    const std::string expected("2 + \u03c0 + a + b");
    const BasePtr sum = Sum::create(two, a, b, Constant::createPi());

    printer.set(sum);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, sumWithFunction)
{
    const std::string expected("a + acos(b) + tan(c)");
    const BasePtr sum = Sum::create(a, Trigonometric::createTan(c), Trigonometric::createAcos(b));

    printer.set(sum);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, product)
{
    const std::string expected("a*b*c*d");
    const BasePtr product = Product::create(a, b, c, d);

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, negSymbolAsProduct)
{
    const std::string expected("-a");

    printer.set(Product::minus(a));

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfSymbolAndPositiveInteger)
{
    const std::string expected("a^2");
    const BasePtr pow = Power::create(a, two);
    Printer printer(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfSymbolAndNegInt)
{
    const std::string expected("1/a^3");
    const BasePtr pow = Power::create(a, Numeric::create(-3));

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfSymbolAndMinusOne)
{
    const std::string expectedFrac("1/a");
    const std::string expectedNoFrac("a^(-1)");
    const BasePtr pow = Power::create(a, Numeric::mOne());

    printer.set(pow);

    CHECK_EQUAL(expectedFrac, printer.getStr());

    printer.disableFractions();
    printer.set(pow);

    CHECK_EQUAL(expectedNoFrac, printer.getStr());
}

TEST(Printer, powerOfProductAndMinusOne)
{
    const std::string expected("1/(2*a*b)");
    const BasePtr pow = Power::create(Product::create(two, a, b), Numeric::mOne());

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfPowerOfPowerOfPower)
{
    const std::string expected("(((a^b)^c)^(-1/4*\u03c0))^d");
    const BasePtr pow1 = Power::create(a, b);
    const BasePtr pow2 = Power::create(pow1, c);
    const BasePtr pow3 = Power::create(pow2, Product::create(Numeric::create(-1, 4),
                Constant::createPi()));
    const BasePtr pow4 = Power::create(pow3, d);

    printer.set(pow4);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, omitFirstNumeratorFactorIfOne)
{
    const std::string expected("c/(2*a*b)");
    const BasePtr product = Product::create(c,
            Power::create(Product::create(two, a, b), Numeric::mOne()));

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, omitFirstNumeratorFactorIfMinusOne)
{
    const std::string expected("-c/(2*a*b)");
    const BasePtr product = Product::minus(c,
            Power::create(Product::create(two, a, b), Numeric::mOne()));

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfSymbolAndPosFrac)
{
    const std::string expected("a^(1/4)");
    const BasePtr pow = Power::create(a, Numeric::create(1, 4));

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, sqrtPower)
{
    const std::string expected("sqrt(a*b)");
    const BasePtr pow = Power::sqrt(Product::create(a, b));

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, oneOverSqrtPower)
{
    const std::string expectedNoFrac("a^(-1/2)*b^(-1/2)");
    const std::string expectedFrac("1/(sqrt(a)*sqrt(b))");
    const BasePtr exp = Numeric::create(-1, 2);
    const BasePtr product = Product::create(Power::create(a, exp), Power::create(b, exp));

    printer.set(product);

    CHECK_EQUAL(expectedFrac, printer.getStr());

    printer.disableFractions();
    printer.set(product);

    CHECK_EQUAL(expectedNoFrac, printer.getStr());
}

TEST(Printer, powerOfSymbolAndNegFrac)
{
    const std::string expectedNoFrac("a^(-2/3)");
    const std::string expectedFrac("1/a^(2/3)");
    const BasePtr pow = Power::create(a, Numeric::create(-2, 3));

    printer.set(pow);

    CHECK_EQUAL(expectedFrac, printer.getStr());

    printer.disableFractions();
    printer.set(pow);

    CHECK_EQUAL(expectedNoFrac, printer.getStr());
}

TEST(Printer, powerOfFraction)
{
    const std::string expected("(5/7)^a");
    const BasePtr n = Numeric::create(5, 7);
    const BasePtr pow = Power::create(n, a);

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerWithPiBase)
{
    const std::string expected("\u03c0^(a + b)");
    const BasePtr pi = Constant::createPi();
    const BasePtr pow = Power::create(pi, Sum::create(a, b));

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerWithPiExp)
{
    const std::string expected("(a + b)^\u03c0");
    const BasePtr pi = Constant::createPi();
    const BasePtr pow = Power::create(Sum::create(a, b), pi);

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfSymbolAndSymbol)
{
    const std::string expected("a^b");
    const BasePtr pow = Power::create(a, b);

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfSumAndNumber)
{
    const std::string expected("(a + b)^2");
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow = Power::create(sum, two);

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfNumberAndSum)
{
    const std::string expected("2^(a + b)");
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow = Power::create(two, sum);

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfFunctionAndNumber)
{
    const std::string expected("sin(a)^2");
    const BasePtr pow = Power::create(Trigonometric::createSin(a), two);

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfSumAndFunction)
{
    const std::string expected("(2 + b + sin(a))^asin(1/5)");
    const BasePtr sum = Sum::create(two, b, Trigonometric::createSin(a));
    const BasePtr pow = Power::create(sum, Trigonometric::createAsin(Numeric::create(1, 5)));

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfProductAndNumber)
{
    const std::string expected("a^2*b^2");
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, two);

    printer.set(pow);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, powerOfProductAndNegNumber)
{
    const std::string expectedFrac("1/(a^2*b^2)");
    const std::string expectedNoFrac("a^(-2)*b^(-2)");
    const BasePtr product = Product::create(a, b);
    const BasePtr pow = Power::create(product, Numeric::create(-2));

    printer.set(pow);

    CHECK_EQUAL(expectedFrac, printer.getStr());

    printer.disableFractions();
    printer.set(pow);

    CHECK_EQUAL(expectedNoFrac, printer.getStr());
}

TEST(Printer, largeProductOfPowersWithFrac)
{
    const std::string expectedFrac("a*b*(a + c)*f^a/(d*e^2)");
    const std::string expectedNoFrac("a*b*(a + c)*d^(-1)*e^(-2)*f^a");
    const BasePtr product = Product::create({ a, b, Sum::create(a, c), Power::create(f, a),
            Power::oneOver(d), Power::create(e, Numeric::create(-2)) });

    printer.set(product);

    CHECK_EQUAL(expectedFrac, printer.getStr());

    printer.disableFractions();
    printer.set(product);

    CHECK_EQUAL(expectedNoFrac, printer.getStr());
}

TEST(Printer, simpleDivisionOfSymbols)
{
    const std::string expectedFrac("a/b");
    const std::string expectedNoFrac("a*b^(-1)");
    const BasePtr product = Product::create(a, Power::oneOver(b));

    printer.set(product);

    CHECK_EQUAL(expectedFrac, printer.getStr());

    printer.disableFractions();
    printer.set(product);

    CHECK_EQUAL(expectedNoFrac, printer.getStr());
}

TEST(Printer, negProductFactorMinusOne)
{
    const std::string expected("-a*b");
    const BasePtr product = Product::minus(a, b);

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, negProductNonTrivialFactor)
{
    const std::string expected("-2*a*b");
    const BasePtr product = Product::create(a, b, Numeric::create(-2));

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, productWithConstantPi)
{
    const std::string expected("-2*\u03c0*a*b");
    const BasePtr product = Product::create({ Numeric::create(-2), a, b, Constant::createPi() });

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, productOfEqualExpPowers)
{
    const std::string expected("sqrt(a)*sqrt(b)");
    const BasePtr half = Numeric::create(1, 2);
    const BasePtr product = Product::create(Power::sqrt(a), Power::sqrt(b));

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, negProductOfEqualExpPowers)
{
    const std::string expected("-a^(2/3)*b^(2/3)");
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr product = Product::create({ Numeric::mOne(), Power::create(a, exp),
            Power::create(b, exp) });

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, productOfFunctions)
{
    const std::string expectedFrac("a*atan(1/sqrt(17))*cos(c*d)*sin(a*b)^3/cos(a*b)");
    const std::string expectedNoFrac("a*atan(17^(-1/2))*cos(a*b)^(-1)*cos(c*d)*sin(a*b)^3");
    BasePtrList fac;
    BasePtr product;

    fac.push_back(a);
    fac.push_back(Trigonometric::createAtan(Power::create(Numeric::create(17),
                    Numeric::create(-1, 2))));
    fac.push_back(Trigonometric::createCos(Product::create(c, d)));
    fac.push_back(Power::create(Trigonometric::createSin(Product::create(a, b)), two));
    fac.push_back(Trigonometric::createTan(Product::create(a, b)));

    product = Product::create(fac);
    printer.set(product);

    CHECK_EQUAL(expectedFrac, printer.getStr());

    printer.disableFractions();
    printer.set(product);

    CHECK_EQUAL(expectedNoFrac, printer.getStr());
}

TEST(Printer, fracOfSumAndProduct)
{
    const std::string expected("(a + b)/(c*d)");
    const BasePtr sum = Sum::create(a, b);
    const BasePtr product = Product::create(c, d);
    const BasePtr frac = Product::create(sum, Power::oneOver(product));

    printer.set(frac);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, fracOfTwoProducts)
{
    const std::string expected("a*b/(c*d)");
    const BasePtr prod1 = Product::create(a, b);
    const BasePtr prod2 = Product::create(c, d);
    const BasePtr frac = Product::create(prod1, Power::oneOver(prod2));

    printer.set(frac);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, fracOfPowerAndSum)
{
    const std::string expected("a^b/(c + d)");
    const BasePtr pow = Power::create(a, b);
    const BasePtr sum = Sum::create(c, d);
    const BasePtr frac = Product::create(pow, Power::oneOver(sum));

    printer.set(frac);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, negTermsInSum)
{
    const std::string expected("a - b");
    const BasePtr sum = Sum::create(a, Product::minus(b));

    printer.set(sum);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, posProductInSum)
{
    const std::string expected("a + b*c");
    const BasePtr sum = Sum::create(a, Product::create(b, c));

    printer.set(sum);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, negSumInProduct)
{
    const std::string expected("a*(b + c)");
    const BasePtr product = Product::create(a, Sum::create(b, c));

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, posSumInProduct)
{
    const std::string expected("a*(-b + c)");
    const BasePtr product = Product::create(a, Sum::create(Product::minus(b), c));

    printer.set(product);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, vectorOfSymbols)
{
    const std::string expected("[   a ]\n[ 2*b ]\n[   c ]");
    Vector v(3);

    v(0) = Var("a");
    v(1) = 2*Var("b");
    v(2) = Var("c");

    printer.set(v);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, vectorOfNumerics)
{
    const std::string expected("[   1 ]\n[ 2/3 ]\n[  -4 ]");
    Vector v(3);

    v(0) = 1;
    v(1) = Var(2, 3);
    v(2) = -4;

    printer.set(v);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, emptyVector)
{
    const std::string expected("[ ]");
    Vector v;
    Printer printer(v);

    printer.set(v);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, matrixOfMixedTypes)
{
    const std::string expected("[     a    0      b ]\n[ 2/3*b  5/8  c + d ]");
    Matrix m(2, 3);

    m(0, 0) = Var("a");
    m(0, 1) = Var();
    m(0, 2) = Var("b");
    m(1, 0) = Var(2, 3)*Var("b");
    m(1, 1) = Var(5, 8);
    m(1, 2) = Var("c") + Var("d");

    printer.set(m);

    CHECK_EQUAL(expected, printer.getStr());
}

TEST(Printer, emptyMatrix)
{
    const std::string expected("[ ]");
    Matrix empty;
    Printer printer(empty);

    CHECK_EQUAL(expected, printer.getStr());
}
