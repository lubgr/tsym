
#include "abc.h"
#include "order.h"
#include "symbol.h"
#include "numeric.h"
#include "constant.h"
#include "sum.h"
#include "undefined.h"
#include "power.h"
#include "product.h"
#include "trigonometric.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Order)
{
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr pi = Constant::createPi();
};

TEST(Order, twoUndefined)
{
    const BasePtr u1 = Undefined::create();
    const BasePtr u2 = Undefined::create();

    disableLog();
    /* This should normally not be requested. Just check, that no swap is indicated. */
    CHECK(order::isCorrect(u1, u2));
    enableLog();
}

TEST(Order, oneUndefined)
{
    const BasePtr u1 = Undefined::create();

    disableLog();
    /* See above. */
    CHECK(order::isCorrect(u1, a));
    CHECK(order::isCorrect(a, u1));
    enableLog();
}

TEST(Order, twoConstants)
{
    const BasePtr e = Constant::createE();

    CHECK(order::isCorrect(pi, pi));
    CHECK(order::doPermute(pi, e));
}

TEST(Order, twoSymbolsAlphanumericNames)
{
    const BasePtr x1 = Symbol::create("x1");
    const BasePtr x2 = Symbol::create("x2");

    CHECK(order::isCorrect(x1, x2));
    CHECK(order::doPermute(x2, x1));
}

TEST(Order, twoEqualSymbols)
{
    CHECK(order::isCorrect(a, a));
}

TEST(Order, twoSymbolsOnePositive)
{
    const BasePtr aPos = Symbol::createPositive("a");

    CHECK(order::isCorrect(aPos, a));
    CHECK(order::doPermute(a, aPos));
}

TEST(Order, twoSymbolsCapitalSmall)
{
    const BasePtr A = Symbol::create("A");

    CHECK(order::doPermute(a, A));
}

TEST(Order, twoSymbolsNumericNames)
{
    const BasePtr eleven = Symbol::create("11");

    CHECK(order::doPermute(eleven, ten));
}

TEST(Order, twoSymbolsNumericNameAndLetter)
{
    CHECK(order::doPermute(a, ten));
}

TEST(Order, twoNumericsFrac)
{
    const BasePtr twoThird = Numeric::create(2, 3);

    CHECK(order::doPermute(twoThird, Numeric::half()));
}

TEST(Order, twoNumericsDouble)
{
    const BasePtr n1 = Numeric::create(1.234556789);
    const BasePtr n2 = Numeric::create(12.3456789);

    CHECK(order::doPermute(n2, n1));
}

TEST(Order, twoNumericsInteger)
{
    CHECK(order::doPermute(four, three));
}

TEST(Order, twoNumericsMixedTypes)
{
    const BasePtr n1 = Numeric::create(3, 4);
    const BasePtr n2 = Numeric::create(1.23456789);

    CHECK(order::doPermute(n2, n1));
}

TEST(Order, twoPowersDifferentBase)
{
    const BasePtr pow1 = Power::sqrt(a);
    const BasePtr pow2 = Power::sqrt(b);

    CHECK(order::doPermute(pow2, pow1));
}

TEST(Order, twoPowersDifferentExp)
{
    const BasePtr pow1 = Power::sqrt(a);
    const BasePtr pow2 = Power::create(a, five);

    CHECK(order::doPermute(pow2, pow1));
}

TEST(Order, twoPowersDifferentBaseAndExp)
{
    const BasePtr pow1 = Power::create(a, five);
    const BasePtr pow2 = Power::sqrt(b);

    CHECK(order::doPermute(pow2, pow1));
}

TEST(Order, sumsWithTwoSummands)
{
    const BasePtr sum1 = Sum::create(a, b);
    const BasePtr sum2 = Sum::create(a, c);

    CHECK(order::doPermute(sum2, sum1));
}

TEST(Order, sumsWithThreeSummands)
{
    BasePtrList summands;
    const BasePtr sum1 = Sum::create(a, c, d);
    const BasePtr sum2 = Sum::create(b, c, d);

    CHECK(order::doPermute(sum2, sum1));
}

TEST(Order, sumsWithTwoAndThreeSummands)
{
    const BasePtr sum1 = Sum::create(c, d);
    const BasePtr sum2 = Sum::create(b, c, d);;

    CHECK(order::doPermute(sum2, sum1));
    CHECK(order::isCorrect(sum1, sum2));
}

TEST(Order, equalSumsWithFourSummands)
{
    BasePtr sum = Sum::create(a, b, c, d);

    CHECK(order::isCorrect(sum, sum));
}

TEST(Order, productsWithTwoFactors)
    /* This causes the same functions to be called as the sum tests do, so one test is enough. */
{
    const BasePtr product1 = Product::create(a, b);
    const BasePtr product2 = Product::create(a, c);

    CHECK(order::doPermute(product2, product1));
}

TEST(Order, productAndPower)
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr bToTheThree = Power::create(b, three);
    const BasePtr product = Product::create(a, bSquare);

    CHECK(order::isCorrect(product, bToTheThree));
    CHECK(order::doPermute(bToTheThree, product));
}

TEST(Order, productAndSymbol)
{
    const BasePtr product = Product::create(a, b);

    CHECK(order::doPermute(product, b));
    CHECK(order::isCorrect(b, product));
}

TEST(Order, powerAndNumber)
{
    const BasePtr sqrtA = Power::sqrt(a);

    CHECK(order::isCorrect(five, sqrtA));
    CHECK(order::doPermute(sqrtA, five));
}

TEST(Order, sumAndSymbol)
{
    const BasePtr sum = Sum::create(a, c);

    CHECK(order::isCorrect(b, sum));
    CHECK(order::isCorrect(c, sum));
    CHECK(order::doPermute(d, sum));
}

TEST(Order, powerAndSum)
    /* Switch (1/2 + b) and (1/2 + a)^2. */
{
    const BasePtr sum1 = Sum::create(Numeric::half(), a);
    const BasePtr sum2 = Sum::create(Numeric::half(), b);
    const BasePtr pow = Power::create(sum1, two);

    CHECK(order::doPermute(sum2, pow));
    CHECK(order::isCorrect(pow, sum2));
}

TEST(Order, powerAndSumSameBase)
{
    const BasePtr sum = Sum::create(a, d);
    const BasePtr pow = Power::sqrt(sum);

    CHECK(order::isCorrect(pow, sum));
    CHECK(order::doPermute(sum, pow));
}

TEST(Order, powerAndSymbol)
{
    const BasePtr sqrtA = Power::sqrt(a);

    CHECK(order::doPermute(b, sqrtA));
}

TEST(Order, numericAndConstPower)
{
    CHECK(order::doPermute(sqrtTwo, two));
}

TEST(Order, constSumAndNumeric)
{
    const BasePtr sum = Sum::create(three, sqrtTwo);

    CHECK(order::doPermute(sum, two));
}

TEST(Order, constSumAndConstPower)
{
    const BasePtr sum = Sum::create(one, sqrtTwo);

    CHECK(order::doPermute(sum, sqrtThree));
    CHECK(order::isCorrect(sqrtThree, sum));
}

TEST(Order, constantAndSmallerNumeric)
{
    CHECK(order::doPermute(pi, Numeric::half()));
}

TEST(Order, constantAndGreaterNumeric)
{
    CHECK(order::doPermute(pi, Numeric::create(100)));
}

TEST(Order, constantAndSymbol)
{
    CHECK(order::isCorrect(pi, a));
}

TEST(Order, constantAndPower)
{
    CHECK(order::isCorrect(pi, sqrtTwo));
}

TEST(Order, constantAndSum)
{
    const BasePtr sum = Sum::create(a, two);

    CHECK(order::isCorrect(Constant::createE(), sum));
}

TEST(Order, constantAndProduct)
{
    const BasePtr product = Product::create(sqrtTwo, a);

    CHECK(order::doPermute(product, pi));
}

TEST(Order, twoDifferentFunctionsSameArgument)
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr cos = Trigonometric::createCos(a);

    CHECK(order::doPermute(sin, cos));
}

TEST(Order, twoDifferentFunctionsDifferentArguments)
{
    const BasePtr tan = Trigonometric::createTan(a);
    const BasePtr sin = Trigonometric::createSin(b);

    CHECK(order::doPermute(tan, sin));
}

TEST(Order, sameFunctionDifferentSymbolArguments)
{
    const BasePtr asin1 = Trigonometric::createAsin(a);
    const BasePtr asin2 = Trigonometric::createAsin(b);

    CHECK(order::doPermute(asin2, asin1));
}

TEST(Order, sameFunctionDifferentNumericArguments)
{
    const BasePtr sin1 = Trigonometric::createSin(one);
    const BasePtr sin2 = Trigonometric::createSin(two);

    CHECK(order::doPermute(sin2, sin1));
}

TEST(Order, sameFunctionDifferentProductArgument)
{
    const BasePtr arg1 = Product::create(a, b);
    const BasePtr arg2 = Product::create(a, d);
    const BasePtr sin1 = Trigonometric::createSin(arg1);
    const BasePtr sin2 = Trigonometric::createSin(arg2);

    CHECK(order::doPermute(sin2, sin1));
}

TEST(Order, functionAndSymbolSameName)
{
    const BasePtr sinSymbol = Symbol::create("sin");
    const BasePtr sin = Trigonometric::createSin(a);

    CHECK(order::doPermute(sinSymbol, sin));
}

TEST(Order, functionAndSymbolDifferentNames)
{
    const BasePtr cos = Trigonometric::createCos(a);

    CHECK(order::doPermute(cos, a));
    CHECK_FALSE(order::doPermute(a, cos));
}

TEST(Order, functionAndProduct)
{
    const BasePtr fct = Trigonometric::createSin(a);
    const BasePtr product = Product::create(a, b);

    CHECK(order::doPermute(fct, product));
}

TEST(Order, functionAndProductEqualLastFactor)
{
    const BasePtr fct = Trigonometric::createSin(d);
    const BasePtr product = Product::create(a, b, c, fct);

    CHECK(order::doPermute(product, fct));
    CHECK(order::isCorrect(fct, product));
}

TEST(Order, functionAndPower)
{
    const BasePtr pow = Power::create(a, two);
    const BasePtr sin = Trigonometric::createSin(a);

    CHECK(order::doPermute(sin, pow));
    CHECK(order::isCorrect(pow, sin));
}

TEST(Order, functionAndSum)
{
    const BasePtr sin = Trigonometric::createSin(c);
    const BasePtr sum = Sum::create(b, Trigonometric::createSin(a));

    CHECK(order::doPermute(sin, sum));
    CHECK(order::isCorrect(sum, sin));
}

TEST(Order, numericFraction)
{
    const BasePtr n = Numeric::third();

    CHECK(order::isCorrect(n, n));
}

TEST(Order, numericPower)
{
    const BasePtr pow = Power::create(two, Numeric::third());

    CHECK(order::isCorrect(pow, pow));
}

TEST(Order, equalTrigonometricFunction)
{
    const BasePtr arg = Power::create(seven, Numeric::create(2, 3));
    const BasePtr cos = Trigonometric::createCos(arg);

    CHECK(order::isCorrect(cos, cos));
}
