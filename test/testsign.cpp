
#include <cmath>
#include "cpputest.h"
#include "abc.h"
#include "product.h"
#include "sum.h"
#include "constant.h"
#include "power.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "undefined.h"

using namespace tsym;

TEST_GROUP(Sign)
{
    BasePtr aPos;
    BasePtr bPos;
    BasePtr cPos;
    BasePtr sqrtTwo;
    BasePtr pi;

    void setup()
    {
        aPos = Symbol::createPositive("a");
        bPos = Symbol::createPositive("b");
        cPos = Symbol::createPositive("c");
        sqrtTwo = Power::sqrt(two);
        pi = Constant::createPi();
    }

    void checkPos(const BasePtr& arg)
    {
        CHECK(arg->isPositive());
        CHECK(!arg->isNegative());
    }

    void checkNeg(const BasePtr& arg)
    {
        CHECK(arg->isNegative());
        CHECK(!arg->isPositive());
    }

    void checkUnclear(const BasePtr& arg)
    {
        CHECK(!arg->isNegative());
        CHECK(!arg->isPositive());
    }
};

TEST(Sign, undefined)
{
    const BasePtr undefined = Undefined::create();

    checkUnclear(undefined);
}

TEST(Sign, constant)
{
    const BasePtr e = Constant::createE();

    checkPos(e);
}

TEST(Sign, unclearSymbol)
{
    checkUnclear(a);
}

TEST(Sign, positiveSimpleSymbol)
{
    checkPos(aPos);
}

TEST(Sign, positiveSymbolWithLongName)
{
    const Name name("Abcde", "1234", "f");
    const BasePtr positive = Symbol::createPositive(name);

    checkPos(positive);
}

TEST(Sign, zero)
{
    checkUnclear(zero);
}

TEST(Sign, posInteger)
{
    checkPos(five);
}

TEST(Sign, negInteger)
{
    const BasePtr neg = Numeric::create(-1234);

    checkNeg(neg);
}

TEST(Sign, posFraction)
{
    const BasePtr pos = Numeric::create(4, 17);

    checkPos(pos);
}

TEST(Sign, negFraction)
{
    const BasePtr neg = Numeric::create(-12, 13);

    checkNeg(neg);
}

TEST(Sign, posDouble)
{
    const BasePtr pos = Numeric::create(123.456789);

    checkPos(pos);
}

TEST(Sign, negDouble)
{
    const BasePtr neg = Numeric::create(-987.654321);

    checkNeg(neg);
}

TEST(Sign, simpleNegProduct)
{
    const BasePtr neg = Product::minus(aPos);

    checkNeg(neg);
}

TEST(Sign, simplePosProduct)
    /* (-a)*(-2*b - 3*c) is positive, if a, b and c are positive. */
{
    const BasePtr sum = Sum::create(Product::minus(two, bPos), Product::minus(three, cPos));
    const BasePtr pos = Product::minus(aPos, sum);

    checkPos(pos);
}

TEST(Sign, simpleUnclearProduct)
    /* Same as before, but with b not being specified as positive. */
{
    const BasePtr sum = Sum::create(Product::minus(two, b), Product::minus(three, cPos));
    const BasePtr result = Product::minus(aPos, sum);

    checkUnclear(result);
}

TEST(Sign, sumNumericallyEvaluableToZero)
{
    const BasePtr sum = Sum::create(Product::create(sqrtTwo, pi),
            Numeric::create(-4.442882938158366));

    checkUnclear(sum);
}

TEST(Sign, simplePositiveSum)
{
    const BasePtr sum = Sum::create(aPos, bPos, sqrtTwo, cPos);

    checkPos(sum);
}

TEST(Sign, simpleNegativeSum01)
{
    const BasePtr sum = Sum::create(Product::minus(two, bPos), Product::minus(three, cPos));

    checkNeg(sum);
}

TEST(Sign, simpleNegativeSum02)
{
    const BasePtr sum = Sum::create(Product::minus(aPos), Numeric::create(-2, 3),
            Product::minus(two, pi));

    checkNeg(sum);
}

TEST(Sign, simpleUnclearSumByDifferentSigns)
{
    const BasePtr sum = Sum::create(Product::minus(aPos, bPos), pi, Numeric::create(-1, 3), cPos);

    checkUnclear(sum);
}

TEST(Sign, simpleUnclearSumByNonPositiveSymbol)
{
    const BasePtr sum = Sum::create(Product::minus(aPos), Numeric::create(-2, 3),
            Product::minus(two, pi), Product::minus(six, b));

    checkUnclear(sum);
}

TEST(Sign, logarithmArgGreaterThanOne)
{
    const BasePtr arg = Sum::create(Product::create(pi, nine, sqrtTwo),
            five, Constant::createE());
    const BasePtr log = Logarithm::create(arg);

    checkPos(log);
}

TEST(Sign, logarithmArgLessThanOne)
{
    const BasePtr arg = Sum::create(pi, Numeric::create(-3));
    const BasePtr log = Logarithm::create(arg);

    checkNeg(log);
}

TEST(Sign, positiveButNumericallyNonEvaluableLogArg)
{
    const BasePtr arg = Sum::create(Product::create(Constant::createE(), nine, sqrtTwo), aPos);
    const BasePtr log = Logarithm::create(arg);

    checkUnclear(log);
}

TEST(Sign, tan)
{
    const BasePtr tanPosArg = Trigonometric::createTan(aPos);
    const BasePtr tanNegArg = Trigonometric::createTan(Product::minus(aPos));
    const BasePtr tanUnclearArg = Trigonometric::createTan(a);

    checkUnclear(tanPosArg);
    checkUnclear(tanNegArg);
    checkUnclear(tanUnclearArg);
}

TEST(Sign, atan)
{
    const BasePtr atanPosArg = Trigonometric::createAtan(aPos);
    const BasePtr atanNegArg = Trigonometric::createAtan(Product::minus(aPos));
    const BasePtr atanUnclearArg = Trigonometric::createAtan(a);

    checkPos(atanPosArg);
    checkNeg(atanNegArg);
    checkUnclear(atanUnclearArg);
}

TEST(Sign, atan2)
{
    const BasePtr atan2Pos = Trigonometric::createAtan2(aPos, bPos);
    const BasePtr atan2Neg = Trigonometric::createAtan2(Product::minus(aPos), bPos);
    const BasePtr atan2Unclear = Trigonometric::createAtan2(a, bPos);

    checkUnclear(atan2Pos);
    checkUnclear(atan2Neg);
    checkUnclear(atan2Unclear);
}

TEST(Sign, sinSymbolic)
{
    const BasePtr sinPosArg = Trigonometric::createSin(aPos);
    const BasePtr sinNegArg = Trigonometric::createSin(Product::minus(aPos));
    const BasePtr sinUnclearArg = Trigonometric::createSin(a);

    checkUnclear(sinPosArg);
    checkUnclear(sinNegArg);
    checkUnclear(sinUnclearArg);
}

TEST(Sign, sinNumericallyEvaluable)
{
    const BasePtr posSin = Trigonometric::createSin(two);
    const BasePtr negSin = Trigonometric::createSin(four);

    checkPos(posSin);
    checkNeg(negSin);
}

TEST(Sign, posPowerWithPositiveBase)
{
    const BasePtr pow = Power::create(Sum::create(aPos, Product::create(two, bPos)),
            Sum::create(a, b, c, ten));

    checkPos(pow);
}

TEST(Sign, negativePowByOddExp)
    /* (-a)^(3/5) = (-1)*a^(3/5)< 0. */
{
    const BasePtr pow = Power::create(Product::minus(aPos), Numeric::create(3, 5));

    checkNeg(pow);
}

TEST(Sign, posPowerWithNegativeBase)
    /* (-pi)^2 = (-1)^2*pi^2 = pi^2 > 0. */
{
    const BasePtr pow = Power::create(Product::minus(pi), two);

    checkPos(pow);
}

TEST(Sign, positivePowByEvenExpNumerator)
    /* (-a)^(4/5) = (-1)^(4/5)*a^(4/5) = a^(4/5) > 0. */
{
    const BasePtr pow = Power::create(Product::minus(aPos), Numeric::create(4, 5));

    checkPos(pow);
}

TEST(Sign, unclearPower)
{
    const BasePtr pow = Power::create(a, bPos);

    checkUnclear(pow);
}

TEST(Sign, posPowerWithUnclearBase)
{
    const BasePtr pow = Power::create(a, two);

    checkPos(pow);
}

TEST(Sign, mixedPositive)
    /* 2*a + b*c + b^(2*c + pi) + 0.12345*c^2 is positive. */
{
    BasePtrList summands;
    BasePtr res;

    summands.push_back(Product::create(two, aPos));
    summands.push_back(Product::create(bPos, cPos));
    summands.push_back(Power::create(bPos, Sum::create(Product::create(two, cPos), pi)));
    summands.push_back(Product::create(Numeric::create(0.12345), cPos, cPos));

    res = Sum::create(summands);

    checkPos(res);
}

TEST(Sign, mixedUnclear)
    /* Same as above but with b being not specified as positive. */
{
    BasePtrList summands;
    BasePtr res;

    summands.push_back(Product::create(two, aPos));
    summands.push_back(Product::create(b, cPos));
    summands.push_back(Power::create(bPos, Sum::create(Product::create(two, cPos), pi)));
    summands.push_back(Product::create(Numeric::create(0.12345), cPos, cPos));

    res = Sum::create(summands);

    checkUnclear(res);
}

TEST(Sign, mixedNegativeAndUnclear)
    /* -10*a^6 -2*a*b*c - pi/3 - a*b is negative. */
{
    const BasePtr s1 = Product::minus(ten, Power::create(a, six));
    const BasePtr s2 = Product::minus(two, aPos, bPos, cPos);
    const BasePtr s3 = Product::minus(Numeric::create(1, 3), pi);
    const BasePtr s4 = Product::minus(aPos, bPos);
    const BasePtr res = Sum::create(s1, s2, s3, s4);

    checkNeg(res);
    checkUnclear(res->subst(bPos, b));
}

TEST(Sign, mixedPosAndUnclear)
    /* (5/13*c^(18/19))*(a^2 - 2*pi + 10*b + sqrt(101)) is positive. */
{
    const BasePtr fac1 = Product::create(Numeric::create(5, 13),
            Power::create(c, Numeric::create(18, 19)));
    const BasePtr fac2 = Sum::create(Power::create(a, two), Product::minus(two, pi),
            Product::create(ten, bPos), Power::sqrt(Numeric::create(101)));
    const BasePtr res = Product::create(fac1, fac2);

    checkPos(res);

    checkUnclear(res->subst(bPos, b));
}
