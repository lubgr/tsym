
#include "cpputest.h"
#include "abc.h"
#include "sum.h"
#include "trigonometric.h"
#include "product.h"
#include "constant.h"
#include "power.h"

using namespace tsym;

TEST_GROUP(Degree)
{
    BasePtr abSum;
    BasePtr abProduct;

    void setup()
    {
        abSum = Sum::create(a, b);
        abProduct = Product::create(a, b);
    }
};

/* All types other than Numeric should return 1 for a degree request with the variable being equal
 * to the BasePtr object. This is similiar to Mathematica, but not GiNaC. */
TEST(Degree, equalSymbol)
{
    CHECK_EQUAL(1, a->degree(a));
}

TEST(Degree, equalFunction)
{
    const BasePtr sinA = Trigonometric::createSin(a);

    CHECK_EQUAL(1, sinA->degree(sinA));
}

TEST(Degree, equalConstant)
{
    const BasePtr pi = Constant::createPi();

    CHECK_EQUAL(1, pi->degree(pi));
}

TEST(Degree, equalSum)
{
    const BasePtr sum = Sum::create(ten, a);

    CHECK_EQUAL(1, sum->degree(sum));
}

TEST(Degree, equalProduct)
{
    CHECK_EQUAL(1, abProduct->degree(abProduct));
}

TEST(Degree, equalPower)
{
    const BasePtr pow = Power::create(a, b);

    CHECK_EQUAL(1, pow->degree(pow));
}

TEST(Degree, equalNumeric)
{
    CHECK_EQUAL(0, four->degree(four));
}

TEST(Degree, equalUndefined)
    /* Two Undefined are never equal, so querying the coefficient will return 0. */
{
    const BasePtr undefined = Undefined::create();

    CHECK_EQUAL(0, undefined->degree(undefined));
}

/* Equal behavior for all types: If the argument is completeley different (i.e., no subexpression of
 * the BasePtr object, degree shall always return 0. */
TEST(Degree, differentSymbols)
{
    CHECK_EQUAL(0, a->degree(b));
}

TEST(Degree, symbolDifferentType)
{
    CHECK_EQUAL(0, a->degree(abSum));
}

TEST(Degree, differentNumerics)
{
    CHECK_EQUAL(0, four->degree(five));
}

TEST(Degree, numericSymbol)
{
    CHECK_EQUAL(0, four->degree(a));
}

TEST(Degree, constantSymbol)
{
    const BasePtr pi = Constant::createPi();

    CHECK_EQUAL(0, pi->degree(a));
}

TEST(Degree, functionSum)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr sum = Sum::create(one, b);

    CHECK_EQUAL(0, sinA->degree(sum));
}

TEST(Degree, undefinedSymbol)
{
    const BasePtr undefined = Undefined::create();

    CHECK_EQUAL(0, undefined->degree(a));
}

TEST(Degree, differentPowers)
{
    const BasePtr pow1 = Power::create(a, b);
    const BasePtr pow2 = Power::create(c, three);

    CHECK_EQUAL(0, pow1->degree(pow2));
}

TEST(Degree, sumProductNoSubExpression)
{
    const BasePtr sum = Sum::create(two, a);
    const BasePtr product = Product::create(b, c);

    CHECK_EQUAL(0, sum->degree(product));
}

TEST(Degree, productPowerNoSubExpression)
{
    const BasePtr pow = Power::create(c, two);

    CHECK_EQUAL(0, abProduct->degree(pow));
}

/* The following tests concern Power, Product or Sum types and degrees of subexpressions. */
TEST(Degree, simpleSum)
{
    CHECK_EQUAL(1, abSum->degree(a));
    CHECK_EQUAL(1, abSum->degree(b));
}

TEST(Degree, sumWithProduct)
{
    const BasePtr sum = Sum::create(two, a, Product::create(c, d));

    CHECK_EQUAL(1, sum->degree(d));
}

TEST(Degree, sumWithMultipleExponents)
    /* Degree(10 + a + a*b + a^3 + a^5*b*c, a) = 5. */
{
    BasePtrList summands;
    BasePtr sum;

    summands.push_back(ten);
    summands.push_back(a);
    summands.push_back(abProduct);
    summands.push_back(Power::create(a, three));
    summands.push_back(Product::create(b, c, Power::create(a, five)));

    sum = Sum::create(summands);

    CHECK_EQUAL(5, sum->degree(a));
}

TEST(Degree, sumWithNegativeAndZeroDegree)
    /* Degree(a^(-4) + b^(-3), a) = 0, because degree(b^(-3), a) = 0 > -4. */
{
    const BasePtr sum = Sum::create(Power::create(a, Numeric::create(-4)),
            Power::create(b, Numeric::create(-3)));

    CHECK_EQUAL(0, sum->degree(a));
}

TEST(Degree, sumWithOnlyNegativeDegrees)
    /* Degree(a^(-7) + a^(-3)*b*sin(a), a) = -3. */
{
    const BasePtr summand1 = Power::create(a, Numeric::create(-7));
    const BasePtr pow = Power::create(a, Numeric::create(-3));
    const BasePtr summand2 = Product::create(pow, b, Trigonometric::createSin(a));
    const BasePtr sum = Sum::create(summand1, summand2);

    CHECK_EQUAL(-3, sum->degree(a));
}

TEST(Degree, simpleProduct)
{
    CHECK_EQUAL(1, abProduct->degree(a));
    CHECK_EQUAL(1, abProduct->degree(b));
}

TEST(Degree, unexpandedProduct)
    /* Degree(a*(a + a*b*(a + c)), a) = 3. */
{
    const BasePtr aTimesBTimesAPlusC = Product::create(abProduct, Sum::create(a, c));
    const BasePtr sum = Sum::create(a, aTimesBTimesAPlusC);
    const BasePtr product = Product::create(a, sum);

    CHECK_EQUAL(3, product->degree(a));
}

TEST(Degree, productWithNegativeExp)
    /* Degree(a*(a^(-4) + b), a) = 1. */
{
    const BasePtr product = Product::create(a,
            Sum::create(Power::create(a, Numeric::create(-4)), b));

    CHECK_EQUAL(1, product->degree(a));
}

TEST(Degree, simplePower)
{
    const BasePtr pow = Power::create(a, ten);

    CHECK_EQUAL(10, pow->degree(a));
}

TEST(Degree, simplePowerSumBase)
{
    const BasePtr pow = Power::create(abSum, three);

    CHECK_EQUAL(3, pow->degree(a));
}

TEST(Degree, powerSumBase)
{
    const BasePtr pow = Power::create(Sum::create(a, Power::create(a, Numeric::create(-5))),
            Numeric::create(-2));

    CHECK_EQUAL(-2, pow->degree(a));
}

TEST(Degree, powerTooLargeIntExp)
{
    const BasePtr pow = Power::create(a, Numeric::create(Int::max()));
    int degree;

    disableLog();
    degree = pow->degree(a);
    enableLog();

    CHECK_EQUAL(0, degree);
}

TEST(Degree, powerWithNegIntExp)
{
    const BasePtr pow = Power::create(a, Numeric::create(-2));
    const int degree = pow->degree(a);

    CHECK_EQUAL(-2, degree);
}

TEST(Degree, powerTooSmallIntExp)
{
    const Int min(Int::min());
    const BasePtr pow = Power::create(a, Numeric::create(min));
    int degree;

    disableLog();
    degree = pow->degree(a);
    enableLog();

    /* Depending on the architecture (32/64bit), Int::min() is just the primitive int-limit. */
    if (min.fitsIntoInt()) {
        CHECK_EQUAL(min.toInt(), degree);
    } else {
        CHECK_EQUAL(0, degree);
    }
}
