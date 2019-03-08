
#include "constant.h"
#include "fixtures.h"
#include "numeric.h"
#include "order.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct OrderFixture : public AbcFixture {
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
};

BOOST_FIXTURE_TEST_SUITE(TestOrder, OrderFixture)

BOOST_AUTO_TEST_CASE(twoUndefined, noLogs())
{
    const BasePtr u1 = undefined;
    const BasePtr u2 = undefined;

    /* This should normally not be requested. Just check, that no swap is indicated. */
    BOOST_TEST(isCorrect(*u1, *u2));
}

BOOST_AUTO_TEST_CASE(oneUndefined, noLogs())
{
    /* See above. */
    BOOST_TEST(isCorrect(*undefined, *a));
    BOOST_TEST(isCorrect(*a, *undefined));
}

BOOST_AUTO_TEST_CASE(twoConstants)
{
    const BasePtr e = Constant::createE();

    BOOST_TEST(isCorrect(*pi, *pi));
    BOOST_TEST(doPermute(*pi, *e));
}

BOOST_AUTO_TEST_CASE(twoSymbolsAlphanumericNames)
{
    const BasePtr x1 = Symbol::create("x1");
    const BasePtr x2 = Symbol::create("x2");

    BOOST_TEST(isCorrect(*x1, *x2));
    BOOST_TEST(doPermute(*x2, *x1));
}

BOOST_AUTO_TEST_CASE(twoEqualSymbols)
{
    BOOST_TEST(isCorrect(*a, *a));
}

BOOST_AUTO_TEST_CASE(twoSymbolsOnePositive)
{
    const BasePtr aPos = Symbol::createPositive("a");

    BOOST_TEST(isCorrect(*aPos, *a));
    BOOST_TEST(doPermute(*a, *aPos));
}

BOOST_AUTO_TEST_CASE(twoSymbolsCapitalSmall)
{
    const BasePtr A = Symbol::create("A");

    BOOST_TEST(doPermute(*a, *A));
}

BOOST_AUTO_TEST_CASE(twoSymbolsNumericNames)
{
    const BasePtr eleven = Symbol::create("11");

    BOOST_TEST(doPermute(*eleven, *ten));
}

BOOST_AUTO_TEST_CASE(twoSymbolsNumericNameAndLetter)
{
    BOOST_TEST(doPermute(*a, *ten));
}

BOOST_AUTO_TEST_CASE(twoNumericsFrac)
{
    const BasePtr twoThird = Numeric::create(2, 3);

    BOOST_TEST(doPermute(*twoThird, *Numeric::half()));
}

BOOST_AUTO_TEST_CASE(twoNumericsDouble)
{
    const BasePtr n1 = Numeric::create(1.234556789);
    const BasePtr n2 = Numeric::create(12.3456789);

    BOOST_TEST(doPermute(*n2, *n1));
}

BOOST_AUTO_TEST_CASE(twoNumericsInteger)
{
    BOOST_TEST(doPermute(*four, *three));
}

BOOST_AUTO_TEST_CASE(twoNumericsMixedTypes)
{
    const BasePtr n1 = Numeric::create(3, 4);
    const BasePtr n2 = Numeric::create(1.23456789);

    BOOST_TEST(doPermute(*n2, *n1));
}

BOOST_AUTO_TEST_CASE(twoPowersDifferentBase)
{
    const BasePtr pow1 = Power::sqrt(a);
    const BasePtr pow2 = Power::sqrt(b);

    BOOST_TEST(doPermute(*pow2, *pow1));
}

BOOST_AUTO_TEST_CASE(twoPowersDifferentExp)
{
    const BasePtr pow1 = Power::sqrt(a);
    const BasePtr pow2 = Power::create(a, five);

    BOOST_TEST(doPermute(*pow2, *pow1));
}

BOOST_AUTO_TEST_CASE(twoPowersDifferentBaseAndExp)
{
    const BasePtr pow1 = Power::create(a, five);
    const BasePtr pow2 = Power::sqrt(b);

    BOOST_TEST(doPermute(*pow2, *pow1));
}

BOOST_AUTO_TEST_CASE(sumsWithTwoSummands)
{
    const BasePtr sum1 = Sum::create(a, b);
    const BasePtr sum2 = Sum::create(a, c);

    BOOST_TEST(doPermute(*sum2, *sum1));
}

BOOST_AUTO_TEST_CASE(sumsWithThreeSummands)
{
    const BasePtr sum1 = Sum::create(a, c, d);
    const BasePtr sum2 = Sum::create(b, c, d);

    BOOST_TEST(doPermute(*sum2, *sum1));
}

BOOST_AUTO_TEST_CASE(sumsWithTwoAndThreeSummands)
{
    const BasePtr sum1 = Sum::create(c, d);
    const BasePtr sum2 = Sum::create(b, c, d);

    BOOST_TEST(doPermute(*sum2, *sum1));
    BOOST_TEST(isCorrect(*sum1, *sum2));
}

BOOST_AUTO_TEST_CASE(equalSumsWithFourSummands)
{
    BasePtr sum = Sum::create(a, b, c, d);

    BOOST_TEST(isCorrect(*sum, *sum));
}

BOOST_AUTO_TEST_CASE(productsWithTwoFactors)
/* This causes the same functions to be called as the sum tests do, so one test is enough. */
{
    const BasePtr product1 = Product::create(a, b);
    const BasePtr product2 = Product::create(a, c);

    BOOST_TEST(doPermute(*product2, *product1));
}

BOOST_AUTO_TEST_CASE(productAndPower)
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr bToTheThree = Power::create(b, three);
    const BasePtr product = Product::create(a, bSquare);

    BOOST_TEST(isCorrect(*product, *bToTheThree));
    BOOST_TEST(doPermute(*bToTheThree, *product));
}

BOOST_AUTO_TEST_CASE(productAndSymbol)
{
    const BasePtr product = Product::create(a, b);

    BOOST_TEST(doPermute(*product, *b));
    BOOST_TEST(isCorrect(*b, *product));
}

BOOST_AUTO_TEST_CASE(powerAndNumber)
{
    const BasePtr sqrtA = Power::sqrt(a);

    BOOST_TEST(isCorrect(*five, *sqrtA));
    BOOST_TEST(doPermute(*sqrtA, *five));
}

BOOST_AUTO_TEST_CASE(sumAndSymbol)
{
    const BasePtr sum = Sum::create(a, c);

    BOOST_TEST(isCorrect(*b, *sum));
    BOOST_TEST(isCorrect(*c, *sum));
    BOOST_TEST(doPermute(*d, *sum));
}

BOOST_AUTO_TEST_CASE(powerAndSum)
/* Switch (1/2 + b) and (1/2 + a)^2. */
{
    const BasePtr sum1 = Sum::create(Numeric::half(), a);
    const BasePtr sum2 = Sum::create(Numeric::half(), b);
    const BasePtr pow = Power::create(sum1, two);

    BOOST_TEST(doPermute(*sum2, *pow));
    BOOST_TEST(isCorrect(*pow, *sum2));
}

BOOST_AUTO_TEST_CASE(powerAndSumSameBase)
{
    const BasePtr sum = Sum::create(a, d);
    const BasePtr pow = Power::sqrt(sum);

    BOOST_TEST(isCorrect(*pow, *sum));
    BOOST_TEST(doPermute(*sum, *pow));
}

BOOST_AUTO_TEST_CASE(powerAndSymbol)
{
    const BasePtr sqrtA = Power::sqrt(a);

    BOOST_TEST(doPermute(*b, *sqrtA));
}

BOOST_AUTO_TEST_CASE(numericAndConstPower)
{
    BOOST_TEST(doPermute(*sqrtTwo, *two));
}

BOOST_AUTO_TEST_CASE(constSumAndNumeric)
{
    const BasePtr sum = Sum::create(three, sqrtTwo);

    BOOST_TEST(doPermute(*sum, *two));
}

BOOST_AUTO_TEST_CASE(constSumAndConstPower)
{
    const BasePtr sum = Sum::create(one, sqrtTwo);

    BOOST_TEST(doPermute(*sum, *sqrtThree));
    BOOST_TEST(isCorrect(*sqrtThree, *sum));
}

BOOST_AUTO_TEST_CASE(constantAndSmallerNumeric)
{
    BOOST_TEST(doPermute(*pi, *Numeric::half()));
}

BOOST_AUTO_TEST_CASE(constantAndGreaterNumeric)
{
    BOOST_TEST(doPermute(*pi, *Numeric::create(100)));
}

BOOST_AUTO_TEST_CASE(constantAndSymbol)
{
    BOOST_TEST(isCorrect(*pi, *a));
}

BOOST_AUTO_TEST_CASE(constantAndPower)
{
    BOOST_TEST(isCorrect(*pi, *sqrtTwo));
}

BOOST_AUTO_TEST_CASE(constantAndSum)
{
    const BasePtr sum = Sum::create(a, two);

    BOOST_TEST(isCorrect(*Constant::createE(), *sum));
}

BOOST_AUTO_TEST_CASE(constantAndProduct)
{
    const BasePtr product = Product::create(sqrtTwo, a);

    BOOST_TEST(doPermute(*product, *pi));
}

BOOST_AUTO_TEST_CASE(twoDifferentFunctionsSameArgument)
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr cos = Trigonometric::createCos(a);

    BOOST_TEST(doPermute(*sin, *cos));
}

BOOST_AUTO_TEST_CASE(twoDifferentFunctionsDifferentArguments)
{
    const BasePtr tan = Trigonometric::createTan(a);
    const BasePtr sin = Trigonometric::createSin(b);

    BOOST_TEST(doPermute(*tan, *sin));
}

BOOST_AUTO_TEST_CASE(sameFunctionDifferentSymbolArguments)
{
    const BasePtr asin1 = Trigonometric::createAsin(a);
    const BasePtr asin2 = Trigonometric::createAsin(b);

    BOOST_TEST(doPermute(*asin2, *asin1));
}

BOOST_AUTO_TEST_CASE(sameFunctionDifferentNumericArguments)
{
    const BasePtr sin1 = Trigonometric::createSin(one);
    const BasePtr sin2 = Trigonometric::createSin(two);

    BOOST_TEST(doPermute(*sin2, *sin1));
}

BOOST_AUTO_TEST_CASE(sameFunctionDifferentProductArgument)
{
    const BasePtr arg1 = Product::create(a, b);
    const BasePtr arg2 = Product::create(a, d);
    const BasePtr sin1 = Trigonometric::createSin(arg1);
    const BasePtr sin2 = Trigonometric::createSin(arg2);

    BOOST_TEST(doPermute(*sin2, *sin1));
}

BOOST_AUTO_TEST_CASE(functionAndSymbolSameName)
{
    const BasePtr sinSymbol = Symbol::create("sin");
    const BasePtr sin = Trigonometric::createSin(a);

    BOOST_TEST(doPermute(*sinSymbol, *sin));
}

BOOST_AUTO_TEST_CASE(functionAndSymbolDifferentNames)
{
    const BasePtr cos = Trigonometric::createCos(a);

    BOOST_TEST(doPermute(*cos, *a));
    BOOST_TEST(!doPermute(*a, *cos));
}

BOOST_AUTO_TEST_CASE(functionAndProduct)
{
    const BasePtr fct = Trigonometric::createSin(a);
    const BasePtr product = Product::create(a, b);

    BOOST_TEST(doPermute(*fct, *product));
}

BOOST_AUTO_TEST_CASE(functionAndProductEqualLastFactor)
{
    const BasePtr fct = Trigonometric::createSin(d);
    const BasePtr product = Product::create(a, b, c, fct);

    BOOST_TEST(doPermute(*product, *fct));
    BOOST_TEST(isCorrect(*fct, *product));
}

BOOST_AUTO_TEST_CASE(functionAndPower)
{
    const BasePtr pow = Power::create(a, two);
    const BasePtr sin = Trigonometric::createSin(a);

    BOOST_TEST(doPermute(*sin, *pow));
    BOOST_TEST(isCorrect(*pow, *sin));
}

BOOST_AUTO_TEST_CASE(functionAndSum)
{
    const BasePtr sin = Trigonometric::createSin(c);
    const BasePtr sum = Sum::create(b, Trigonometric::createSin(a));

    BOOST_TEST(doPermute(*sin, *sum));
    BOOST_TEST(isCorrect(*sum, *sin));
}

BOOST_AUTO_TEST_CASE(numericFraction)
{
    const BasePtr n = Numeric::third();

    BOOST_TEST(isCorrect(*n, *n));
}

BOOST_AUTO_TEST_CASE(numericPower)
{
    const BasePtr pow = Power::create(two, Numeric::third());

    BOOST_TEST(isCorrect(*pow, *pow));
}

BOOST_AUTO_TEST_CASE(equalTrigonometricFunction)
{
    const BasePtr arg = Power::create(seven, Numeric::create(2, 3));
    const BasePtr cos = Trigonometric::createCos(arg);

    BOOST_TEST(isCorrect(*cos, *cos));
}

BOOST_AUTO_TEST_SUITE_END()
