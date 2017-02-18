
#include "abc.h"
#include "baseptr.h"
#include "sum.h"
#include "constant.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "product.h"
#include "power.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Subst)
{
    BasePtr undefined;
    BasePtr pi;

    void setup()
    {
        undefined = Undefined::create();
        pi = Constant::createPi();
    }
};

TEST(Subst, undefinedToSymbol)
{
    const BasePtr res = undefined->subst(undefined, a);

    CHECK_EQUAL(a, res);
}

TEST(Subst, symbolToNumeric)
{
    const BasePtr res = a->subst(a, two);

    CHECK_EQUAL(two, res);
}

TEST(Subst, symbolNotToNumeric)
{
    const BasePtr res = a->subst(b, two);

    CHECK_EQUAL(a, res);
}

TEST(Subst, constantToSum)
{
    const BasePtr sum = Sum::create(a, b);
    BasePtr res;

    res = pi->subst(pi, sum);

    CHECK_EQUAL(sum, res);
}

TEST(Subst, constantNotToNumeric)
{
    const BasePtr res = pi->subst(a, two);

    CHECK_EQUAL(pi, res);
}

TEST(Subst, numericToSymbol)
{
    const BasePtr orig = Numeric::create(5, 17);
    BasePtr res;

    res = orig->subst(orig, a);

    CHECK_EQUAL(a, res);
}

TEST(Subst, powerToSum)
{
    const BasePtr sum = Sum::create(a, b, c);
    const BasePtr orig = Power::create(a, two);
    BasePtr res;

    res = orig->subst(orig, sum);

    CHECK_EQUAL(sum, res);
}

TEST(Subst, productToPower)
{
    const BasePtr pow = Power::create(a, b);
    const BasePtr orig = Product::create(a, b);
    BasePtr res;

    res = orig->subst(orig, pow);

    CHECK_EQUAL(pow, res);
}

TEST(Subst, sumToSymbol)
{
    const BasePtr orig = Sum::create(a, b);
    BasePtr res;

    res = orig->subst(orig, a);

    CHECK_EQUAL(a, res);
}

TEST(Subst, undefinedNoChange)
{
    const BasePtr result = undefined->subst(a, b);

    CHECK(result->isUndefined());
}

TEST(Subst, cosineToConstant)
{
    const BasePtr orig = Trigonometric::createCos(a);
    BasePtr res;

    res = orig->subst(orig, pi);

    CHECK_EQUAL(pi, res);
}

TEST(Subst, atanArgToConstant)
{
    const BasePtr orig = Trigonometric::createAtan(a);
    const BasePtr expected = Trigonometric::createAtan(pi);
    BasePtr res;

    res = orig->subst(a, pi);

    CHECK_EQUAL(expected, res);
}

TEST(Subst, sinToZero)
    /* Sin(a) = 0 for a = Pi. */
{
    const BasePtr orig = Trigonometric::createSin(a);
    BasePtr res;

    res = orig->subst(a, pi);

    CHECK(res->isZero());
}

TEST(Subst, asinToUndefined)
    /* Asin(a) = Undefined for |a| > 1. */
{
    const BasePtr orig = Trigonometric::createAsin(a);
    const BasePtr tmp = Numeric::create(1.23456789);
    BasePtr res;

    res = orig->subst(a, tmp);

    CHECK(res->isUndefined());
}

TEST(Subst, atan2ToPiFourth)
    /* Atan(b, a) = Pi/4 for b = 2 and a = 2. */
{
    const BasePtr orig = Trigonometric::createAtan2(b, a);
    const BasePtr firstSubst = orig->subst(b, two);
    const BasePtr expected = Product::create(Numeric::create(1, 4), pi);
    BasePtr secondSubst;

    CHECK(firstSubst->isFunction());

    secondSubst = firstSubst->subst(a, two);

    CHECK_EQUAL(expected, secondSubst);
}

TEST(Subst, logToLog)
    /* Log(a) = log(b) for a = b. */
{
    const BasePtr expected = Logarithm::create(b);
    const BasePtr orig = Logarithm::create(a);
    BasePtr res;

    res = orig->subst(a, b);

    CHECK_EQUAL(expected, res);
}

TEST(Subst, logToZero)
    /* Log(a) = 0 for a = 1. */
{
    const BasePtr orig = Logarithm::create(a);
    BasePtr res;

    res = orig->subst(a, one);

    CHECK(res->isZero());
}

TEST(Subst, logEqualArg)
{
    const BasePtr orig = Logarithm::create(a);
    BasePtr res;

    res = orig->subst(orig, a);

    CHECK_EQUAL(a, res);
}

TEST(Subst, powerToUndefined)
    /* a^(-2) = Undefined for a = 0. */
{
    const BasePtr orig = Power::create(a, Numeric::create(-2));
    BasePtr res;

    disableLog();
    res = orig->subst(a, zero);
    enableLog();

    CHECK(res->isUndefined());
}

TEST(Subst, sumToUndefined)
    /* a + b + c + d = Undefined for c = Undefined. */
{
    const BasePtr orig = Sum::create(a, b, c, d);
    const BasePtr res = orig->subst(c, undefined);

    CHECK(res->isUndefined());
}

TEST(Subst, sumToOne)
    /* a + b^a + a*(b + c) = 1 for a = 0. */
{
    const BasePtr orig = Sum::create(a, Power::create(b, a), Product::create(a, Sum::create(b, c)));
    BasePtr res;

    res = orig->subst(a, zero);

    CHECK_EQUAL(one, res);
}

TEST(Subst, noSubExpressionInSum)
    /* a + b + c stays constant for a + b = 2. */
{
    const BasePtr abSum = Sum::create(a, b);
    const BasePtr orig = Sum::create(abSum, c);
    BasePtr res;

    res = orig->subst(abSum, two);

    CHECK_EQUAL(orig, res);
}

TEST(Subst, noSubExpressionInProduct)
    /* 2*a*b stays constant for a*b = 3. */
{
    const BasePtr abProduct = Product::create(a, b);
    const BasePtr orig = Product::create(two, abProduct);
    BasePtr res;

    res = orig->subst(abProduct, three);

    CHECK_EQUAL(orig, res);
}

TEST(Subst, powerExpanedBySubst)
    /* (a*b + c + d)^2= a^2*b^2 for c = d = 0. */
{
    const BasePtr expected = Product::create(Power::create(a, two), Power::create(b, two));
    const BasePtr abProduct = Product::create(a, b);
    const BasePtr base = Sum::create(abProduct, c, d);
    const BasePtr orig = Power::create(base, two);
    BasePtr res;

    res = orig->subst(c, zero)->subst(d, zero);

    CHECK_EQUAL(expected, res);
}

TEST(Subst, numPowerSimplInSubstitution)
{
    const BasePtr orig = Product::create(two, a, Trigonometric::createCos(b));
    const BasePtr result = orig->subst(b, Product::create(Numeric::create(5, 4), pi));
    const BasePtr expected = Product::minus(Power::sqrt(two), a);

    CHECK_EQUAL(expected, result);
}

TEST(Subst, mixedTerm)
    /* 2*sqrt(3)*(a + b + sqrt(3))*(c + d)/a = 4*17^(1/3)*a + 2*17^(1/3)*b
     * for sqrt(3) = a and c + d = 17^(1/3). */
{
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr numPow = Power::create(Numeric::create(17), Numeric::create(1, 3));
    const BasePtr expected = Sum::create(Product::create(four, numPow, a),
            Product::create(two, numPow, b));
    BasePtrList fac;
    BasePtr orig;
    BasePtr res;

    fac.push_back(two);
    fac.push_back(sqrtThree);
    fac.push_back(Sum::create(a, b, sqrtThree));
    fac.push_back(Sum::create(b, c));
    fac.push_back(Power::oneOver(a));

    orig = Product::create(fac);

    res = orig->subst(sqrtThree, a)->subst(Sum::create(b, c), numPow);

    CHECK_EQUAL(expected, res);
}
