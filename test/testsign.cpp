
#include <cmath>
#include "product.h"
#include "sum.h"
#include "constant.h"
#include "numeric.h"
#include "symbol.h"
#include "power.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "undefined.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

struct SignFixture : public AbcFixture {
    const BasePtr aPos = Symbol::createPositive("a");
    const BasePtr bPos = Symbol::createPositive("b");
    const BasePtr cPos = Symbol::createPositive("c");
    const BasePtr sqrtTwo = Power::sqrt(two);
};

namespace {
    void checkPos(const BasePtr& arg)
    {
        BOOST_TEST(arg->isPositive());
        BOOST_TEST(!arg->isNegative());
    }

    void checkNeg(const BasePtr& arg)
    {
        BOOST_TEST(arg->isNegative());
        BOOST_TEST(!arg->isPositive());
    }

    void checkUnclear(const BasePtr& arg)
    {
        BOOST_TEST(!arg->isNegative());
        BOOST_TEST(!arg->isPositive());
    }
}

BOOST_FIXTURE_TEST_SUITE(TestSign, SignFixture)

BOOST_AUTO_TEST_CASE(undefined)
{
    const BasePtr undefined = Undefined::create();

    checkUnclear(undefined);
}

BOOST_AUTO_TEST_CASE(constant)
{
    const BasePtr e = Constant::createE();

    checkPos(e);
}

BOOST_AUTO_TEST_CASE(unclearSymbol)
{
    checkUnclear(a);
}

BOOST_AUTO_TEST_CASE(positiveSimpleSymbol)
{
    checkPos(aPos);
}

BOOST_AUTO_TEST_CASE(positiveSymbolWithLongName)
{
    const Name name("Abcde", "1234", "f");
    const BasePtr positive = Symbol::createPositive(name);

    checkPos(positive);
}

BOOST_AUTO_TEST_CASE(zeroIsNeitherPosNorNeg)
{
    checkUnclear(zero);
}

BOOST_AUTO_TEST_CASE(posInteger)
{
    checkPos(five);
}

BOOST_AUTO_TEST_CASE(negInteger)
{
    const BasePtr neg = Numeric::create(-1234);

    checkNeg(neg);
}

BOOST_AUTO_TEST_CASE(posFraction)
{
    const BasePtr pos = Numeric::create(4, 17);

    checkPos(pos);
}

BOOST_AUTO_TEST_CASE(negFraction)
{
    const BasePtr neg = Numeric::create(-12, 13);

    checkNeg(neg);
}

BOOST_AUTO_TEST_CASE(posDouble)
{
    const BasePtr pos = Numeric::create(123.456789);

    checkPos(pos);
}

BOOST_AUTO_TEST_CASE(negDouble)
{
    const BasePtr neg = Numeric::create(-987.654321);

    checkNeg(neg);
}

BOOST_AUTO_TEST_CASE(simpleNegProduct)
{
    const BasePtr neg = Product::minus(aPos);

    checkNeg(neg);
}

BOOST_AUTO_TEST_CASE(simplePosProduct)
    /* (-a)*(-2*b - 3*c) is positive, if a, b and c are positive. */
{
    const BasePtr sum = Sum::create(Product::minus(two, bPos), Product::minus(three, cPos));
    const BasePtr pos = Product::minus(aPos, sum);

    checkPos(pos);
}

BOOST_AUTO_TEST_CASE(simpleUnclearProduct)
    /* Same as before, but with b not being specified as positive. */
{
    const BasePtr sum = Sum::create(Product::minus(two, b), Product::minus(three, cPos));
    const BasePtr result = Product::minus(aPos, sum);

    checkUnclear(result);
}

BOOST_AUTO_TEST_CASE(sumNumericallyEvaluableToZero)
{
    const BasePtr sum = Sum::create(Product::create(sqrtTwo, pi),
            Numeric::create(-4.442882938158366));

    checkUnclear(sum);
}

BOOST_AUTO_TEST_CASE(simplePosSum01)
{
    const BasePtr sum = Sum::create(aPos, bPos, sqrtTwo, cPos);

    checkPos(sum);
}

BOOST_AUTO_TEST_CASE(simplePosSum02)
{
    const BasePtr sum = Sum::create(two, pi, Product::minus(Constant::createE()));

    checkPos(sum);
}

BOOST_AUTO_TEST_CASE(simpleNegativeSum01)
{
    const BasePtr sum = Sum::create(Product::minus(two, bPos), Product::minus(three, cPos));

    checkNeg(sum);
}

BOOST_AUTO_TEST_CASE(simpleNegativeSum02)
{
    const BasePtr sum = Sum::create(Product::minus(aPos), Numeric::create(-2, 3),
            Product::minus(two, pi));

    checkNeg(sum);
}

BOOST_AUTO_TEST_CASE(simpleNegativeSum03)
{
    const BasePtr sum = Sum::create(two, Product::minus(pi));

    checkNeg(sum);
}

BOOST_AUTO_TEST_CASE(simpleUnclearSumByDifferentSigns)
{
    const BasePtr sum = Sum::create(Product::minus(aPos, bPos), pi, Numeric::create(-1, 3), cPos);

    checkUnclear(sum);
}

BOOST_AUTO_TEST_CASE(simpleUnclearSumByNonPositiveSymbol)
{
    const BasePtr sum = Sum::create(Product::minus(aPos), Numeric::create(-2, 3),
            Product::minus(two, pi), Product::minus(six, b));

    checkUnclear(sum);
}

BOOST_AUTO_TEST_CASE(logarithmArgGreaterThanOne)
{
    const BasePtr arg = Sum::create(Product::create(pi, nine, sqrtTwo),
            five, Constant::createE());
    const BasePtr log = Logarithm::create(arg);

    checkPos(log);
}

BOOST_AUTO_TEST_CASE(logarithmArgLessThanOne)
{
    const BasePtr arg = Sum::create(pi, Numeric::create(-3));
    const BasePtr log = Logarithm::create(arg);

    checkNeg(log);
}

BOOST_AUTO_TEST_CASE(posButNumericallyNonEvaluableLogArgGreaterOne)
{
    const BasePtr arg = Sum::create(Product::create(Constant::createE(), nine, sqrtTwo), aPos);
    const BasePtr log = Logarithm::create(arg);

    checkPos(log);
}

BOOST_AUTO_TEST_CASE(posButNumericallyNonEvaluableLogArgSmallerOne)
{
    const BasePtr arg = Sum::create(Numeric::create(0.987654321), Product::minus(aPos));
    const BasePtr log = Logarithm::create(arg);

    checkNeg(log);
}

BOOST_AUTO_TEST_CASE(unclearLogArg)
{
    const BasePtr arg = Sum::create(Numeric::create(0.987654321), aPos);
    const BasePtr log = Logarithm::create(arg);

    checkUnclear(log);
}

BOOST_AUTO_TEST_CASE(tan)
{
    const BasePtr tanPosArg = Trigonometric::createTan(aPos);
    const BasePtr tanNegArg = Trigonometric::createTan(Product::minus(aPos));
    const BasePtr tanUnclearArg = Trigonometric::createTan(a);

    checkUnclear(tanPosArg);
    checkUnclear(tanNegArg);
    checkUnclear(tanUnclearArg);
}

BOOST_AUTO_TEST_CASE(atan)
{
    const BasePtr atanPosArg = Trigonometric::createAtan(aPos);
    const BasePtr atanNegArg = Trigonometric::createAtan(Product::minus(aPos));
    const BasePtr atanUnclearArg = Trigonometric::createAtan(a);

    checkPos(atanPosArg);
    checkNeg(atanNegArg);
    checkUnclear(atanUnclearArg);
}

BOOST_AUTO_TEST_CASE(atan2)
{
    const BasePtr atan2Pos = Trigonometric::createAtan2(aPos, bPos);
    const BasePtr atan2Neg = Trigonometric::createAtan2(Product::minus(aPos), bPos);
    const BasePtr atan2Unclear = Trigonometric::createAtan2(a, bPos);

    checkUnclear(atan2Pos);
    checkUnclear(atan2Neg);
    checkUnclear(atan2Unclear);
}

BOOST_AUTO_TEST_CASE(sinSymbolic)
{
    const BasePtr sinPosArg = Trigonometric::createSin(aPos);
    const BasePtr sinNegArg = Trigonometric::createSin(Product::minus(aPos));
    const BasePtr sinUnclearArg = Trigonometric::createSin(a);

    checkUnclear(sinPosArg);
    checkUnclear(sinNegArg);
    checkUnclear(sinUnclearArg);
}

BOOST_AUTO_TEST_CASE(sinNumericallyEvaluable)
{
    const BasePtr posSin = Trigonometric::createSin(two);
    const BasePtr negSin = Trigonometric::createSin(four);

    checkPos(posSin);
    checkNeg(negSin);
}

BOOST_AUTO_TEST_CASE(posPowerWithPositiveBase)
{
    const BasePtr pow = Power::create(Sum::create(aPos, Product::create(two, bPos)),
            Sum::create(a, b, c, ten));

    checkPos(pow);
}

BOOST_AUTO_TEST_CASE(posPowerWithNegativeBase)
    /* (-pi)^2 = (-1)^2*pi^2 = pi^2 > 0. */
{
    const BasePtr pow = Power::create(Product::minus(pi), two);

    checkPos(pow);
}

BOOST_AUTO_TEST_CASE(unclearPower)
{
    const BasePtr pow = Power::create(a, bPos);

    checkUnclear(pow);
}

BOOST_AUTO_TEST_CASE(posPowerWithUnclearBase)
{
    const BasePtr pow = Power::create(a, two);

    checkPos(pow);
}

BOOST_AUTO_TEST_CASE(mixedPositive)
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

BOOST_AUTO_TEST_CASE(mixedUnclear)
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

BOOST_AUTO_TEST_CASE(mixedNegativeAndUnclear)
    /* -10*a^6 -2*a*b*c - pi/3 - a*b is negative. */
{
    const BasePtr s1 = Product::minus(ten, Power::create(a, six));
    const BasePtr s2 = Product::minus(two, aPos, bPos, cPos);
    const BasePtr s3 = Product::minus(Numeric::third(), pi);
    const BasePtr s4 = Product::minus(aPos, bPos);
    const BasePtr res = Sum::create(s1, s2, s3, s4);

    checkNeg(res);
    checkUnclear(res->subst(bPos, b));
}

BOOST_AUTO_TEST_CASE(mixedPosAndUnclear)
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

BOOST_AUTO_TEST_SUITE_END()
