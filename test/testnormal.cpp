
#include "cpputest.h"
#include "abc.h"
#include "constant.h"
#include "sum.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "product.h"
#include "power.h"
#include "symbolmap.h"

using namespace tsym;

TEST_GROUP(Normal)
{
    const BasePtr undefined;
    BasePtr argToZero;
    SymbolMap map;
    BasePtr pi;

    void setup()
    {
        const BasePtr denom = Power::oneOver(Sum::create(b, c));
        argToZero = Sum::create(a, Product::create(Numeric::mOne(), a, b, denom),
                Product::create(Numeric::mOne(), a, c, denom));

        pi = Constant::createPi();
        map = SymbolMap();
    }
};

TEST(Normal, numericInteger)
{
    CHECK_EQUAL(two, two->normal());
}

TEST(Normal, numericDouble)
{
    const BasePtr d = Numeric::create(1.234567);

    CHECK_EQUAL(d, d->normal());
}

TEST(Normal, numericFraction)
{
    const BasePtr frac = Numeric::create(1, 3);

    CHECK_EQUAL(frac, frac->normal());
}

TEST(Normal, symbol)
{
    CHECK_EQUAL(a, a->normal());
}

TEST(Normal, constantPi)
{
    CHECK_EQUAL(pi, pi->normal());
}

TEST(Normal, constantE)
{
    const BasePtr e = Constant::createE();

    CHECK_EQUAL(e, e->normal());
}

TEST(Normal, undefined)
{
    CHECK_EQUAL(undefined, undefined->normal());
}

TEST(Normal, powerWithPosIntExp)
{
    const BasePtr orig = Power::create(a, two);

    CHECK_EQUAL(orig, orig->normal());
}

TEST(Normal, powerWithPosNegExp)
{
    const BasePtr orig = Power::create(a, Numeric::create(-2));

    CHECK_EQUAL(orig, orig->normal());
}

TEST(Normal, powerWithSymbolExp)
{
    const BasePtr orig = Power::create(Sum::create(a, b), c);

    CHECK_EQUAL(orig, orig->normal());
}

TEST(Normal, powerWithMinusSymbolExp)
{
    const BasePtr orig = Power::create(Sum::create(a, b), Product::minus(c));

    CHECK_EQUAL(orig, orig->normal());
}

TEST(Normal, powerWithPiExp)
{
    const BasePtr orig = Power::create(a, pi);

    CHECK_EQUAL(orig, orig->normal());
}

TEST(Normal, powerWithNegNumEvalExp)
{
    const BasePtr pos = Product::create(three, pi, Power::sqrt(two));
    const BasePtr orig = Power::create(a, Product::minus(pos));

    CHECK_EQUAL(orig, orig->normal());
}

TEST(Normal, simpleProduct)
{
    const BasePtr orig = Product::create(a, Power::oneOver(b));
    const Fraction frac = orig->normal(map);

    CHECK_EQUAL(a, frac.num());
    CHECK_EQUAL(b, frac.denom());
}

TEST(Normal, product)
    /* (1/4)*(2*c + 2*b^2 + a*b^2 + a*c)(2*a + a^2)^(-1) becomes 1/4*a^(-1)*(b^2 + c). */
{
    const BasePtr oneFourth = Numeric::create(1, 4);
    const BasePtr num = Sum::create(Product::create(two, c), Product::create(two, b, b),
            Product::create(a, b, b), Product::create(a, c));
    const BasePtr denom = Sum::create(Product::create(two, a), Power::create(a, two));
    const BasePtr orig = Product::create(oneFourth, num, Power::oneOver(denom));
    const BasePtr expected = Product::create(oneFourth, Sum::create(Power::create(b, two), c),
            Power::oneOver(a));
    const BasePtr expectedNum = Sum::create(Power::create(b, two), c);
    const BasePtr expectedDenom = Product::create(four, a);
    Fraction frac;

    frac = orig->normal(map);

    CHECK_EQUAL(expectedNum, frac.num());
    CHECK_EQUAL(expectedDenom, frac.denom());
    CHECK_EQUAL(expected, orig->normal());
}

TEST(Normal, simpleSum)
    /* a/b + 2/b becomes (2 + a)/b. */
{
    const BasePtr oneOverB = Power::oneOver(b);
    const BasePtr expected = Product::create(Sum::create(two, a), oneOverB);
    const BasePtr orig = Sum::create(Product::create(a, oneOverB), Product::create(two, oneOverB));
    const BasePtr result = orig->normal();

    CHECK_EQUAL(expected, result);
}

TEST(Normal, simpleSumToZero)
{
    const BasePtr result = argToZero->normal();

    CHECK(result->isZero());
}

TEST(Normal, simpleSumWithFractionCoeff)
    /* a/b + 1/(5*b) becomes 1/5*(1 + 5*a)/b. */
{
    const BasePtr oneOverB = Power::oneOver(b);
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr expected = Product::create(oneFifth, oneOverB,
            Sum::create(one, Product::create(five, a)));
    const BasePtr orig = Sum::create(Product::create(a, oneOverB),
            Product::create(oneFifth, oneOverB));
    BasePtr result;

    result = orig->normal();

    CHECK_EQUAL(expected, result);
}

TEST(Normal, sum)
    /* sqrt(2)*a/b + c/(sqrt(2)*d) - sin(a)/b becomes
     * (2*a*d + b*c - sqrt(2)*d*sin(a))/(sqrt(2)*b*d). */
{
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr oneOverB = Power::oneOver(b);
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr orig = Sum::create(Product::create(sqrtTwo, a, oneOverB),
            Product::create(c, Power::oneOver(sqrtTwo), Power::oneOver(d)),
            Product::minus(sinA, oneOverB));
    const BasePtr expected = Product::create(Power::oneOver(Product::create(sqrtTwo, b, d)),
            Sum::create(Product::create(two, a, d), Product::create(b, c),
                Product::minus(sqrtTwo, d, sinA)));
    BasePtr result;

    result = orig->normal();

    CHECK_EQUAL(expected, result);
}

TEST(Normal, longSimpleSum)
    /* (1/a)* (a + b - b*(c - (a + b)*c/a + d)/(-b*c/a + d)) becomes 1. */
{
    const BasePtr oneOverA = Power::oneOver(a);
    const BasePtr tmp1 = Sum::create(d, Product::minus(b, c, oneOverA));
    const BasePtr tmp2 = Sum::create(c, Product::minus(c, oneOverA, Sum::create(a, b)), d);
    const BasePtr num = Sum::create(a, b, Product::minus(b, tmp2, Power::oneOver(tmp1)));
    const BasePtr orig = Product::create(oneOverA, num);
    BasePtr result;

    result = orig->normal();

    CHECK_EQUAL(one, result);
}

TEST(Normal, replacementOfFunctionWithNumPowerArg)
    /* sin(sqrt(3))^(-1) is rationalized to 1/tmp with tmp = sin(sqrt(3)). */
{
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sin = Trigonometric::createSin(sqrtThree);
    const BasePtr orig = Power::oneOver(sin);
    const Fraction frac = orig->normal(map);
    BasePtr denom;

    CHECK_EQUAL(one, frac.num());
    CHECK(frac.denom()->isSymbol());

    denom = map.replaceTmpSymbolsBackFrom(frac.denom());

    CHECK_EQUAL(sin, denom);
}

TEST(Normal, fractionsOfNumPowersAndFunctions01)
    /* sqrt(2)/sin(sqrt(3)) + asin(a)/cos(b) becomes
     * (sqrt(2)*cos(b) + sin(sqrt(3))*asin(a))/(sin(sqrt3)*cos(b)). */
{
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sin = Trigonometric::createSin(Power::sqrt(three));
    const BasePtr cos = Trigonometric::createCos(b);
    const BasePtr asin = Trigonometric::createAsin(a);
    const BasePtr orig = Sum::create(Product::create(sqrtTwo, Power::oneOver(sin)),
            Product::create(asin, Power::oneOver(cos)));
    const BasePtr expectedNum = Sum::create(Product::create(sqrtTwo, cos),
            Product::create(sin, asin));
    const BasePtr expectedDenom = Product::create(sin, cos);
    const BasePtr expected = Product::create(expectedNum, Power::oneOver(expectedDenom));
    BasePtr result;

    result = orig->normal();

    CHECK_EQUAL(expected, result);
}

TEST(Normal, fractionsOfNumPowersAndFunctions02)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sqrtSix = Power::sqrt(six);
    const BasePtr aSquare = Power::create(a, two);
    const BasePtr f1 = Power::oneOver(Product::create(three, sqrtTwo));
    const BasePtr f2 = Product::create(sinA, Power::oneOver(sqrtThree));
    const BasePtr f3 = Product::create(sqrtSix, Power::oneOver(aSquare));
    const BasePtr orig = Sum::create(f1, f2, f3);
    const BasePtr expectedNum = Sum::create(Product::create(sqrtThree, aSquare),
            Product::create(three, sqrtTwo, sinA, aSquare), Numeric::create(18));
    const BasePtr expectedDenom = Product::create(three, sqrtSix, aSquare);
    const BasePtr expected = Product::create(expectedNum, Power::oneOver(expectedDenom));
    BasePtr result;

    result = orig->normal();

    CHECK_EQUAL(expected, result);
}

TEST(Normal, fractionSumWithPiExp)
    /* (a/b)^(-Pi) + 1/(a^Pi) becomes (1 + b^Pi)/a^Pi.*/
{
    const BasePtr pi = Constant::createPi();
    const BasePtr f1 = Power::create(Product::create(a, Power::oneOver(b)), Product::minus(pi));
    const BasePtr f2 = Power::oneOver(Power::create(a, pi));
    const BasePtr orig = Sum::create(f1, f2);
    const BasePtr expected = Product::create(Sum::create(one, Power::create(b, pi)),
                Power::oneOver(Power::create(a, pi)));
    BasePtr result;

    result = orig->normal();

    CHECK_EQUAL(expected, result);
}

TEST(Normal, sumWithNumPowerOfFractions)
    /* (3/4)^(-1/5) + 3^(1/5) becomes 2^(2/5)/3^(1/5) + 3^(1/5).*/
{
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr mOneFifth = Numeric::create(-1, 5);
    const BasePtr f1 = Power::create(Numeric::create(3, 4), mOneFifth);
    const BasePtr f2 = Power::create(three, oneFifth);
    const BasePtr orig = Sum::create(f1, f2);
    const BasePtr expected = Sum::create(Product::create(Power::create(two, Numeric::create(2, 5)),
            Power::create(three, mOneFifth)), Power::create(three, oneFifth));
    BasePtr result;

    result = orig->normal();

    CHECK_EQUAL(expected, result);
}

TEST(Normal, trigonometricFunctionArgToZero)
{
    const BasePtr orig = Trigonometric::createSin(argToZero);
    const BasePtr result = orig->normal();

    CHECK(result->isZero());
}

TEST(Normal, logarithmicFunctionArgToZero)
{
    const BasePtr orig = Logarithm::create(argToZero);
    BasePtr result;

    disableLog();
    result = orig->normal();
    enableLog();

    CHECK(result->isUndefined());
}

TEST(Normal, logarithmicFunction)
{
    const BasePtr orig = Logarithm::create(Product::create(two, a));
    const BasePtr result = orig->normal();

    CHECK_EQUAL(orig, result);
}
