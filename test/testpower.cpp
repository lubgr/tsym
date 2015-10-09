
#include <cmath>
#include <limits>
#include "cpputest.h"
#include "abc.h"
#include "power.h"
#include "symbol.h"
#include "numeric.h"
#include "undefined.h"
#include "product.h"
#include "constant.h"
#include "trigonometric.h"
#include "numpowersimpl.h"
#include "logarithm.h"
#include "sum.h"

using namespace tsym;

const double TOL = 1.e-10;

TEST_GROUP(Power)
{
    BasePtr half;
    BasePtr oneThird;
    BasePtr minusHalf;
    BasePtr sqrtTwo;
    BasePtr undefPtr;
    Int maxInt;
    Int defaultPrimeFacLimit;

    void setup()
    {
        half = Numeric::create(1, 2);
        oneThird = Numeric::create(1, 3);
        minusHalf = Numeric::create(-1, 2);
        sqrtTwo = Power::sqrt(two);
        undefPtr = Undefined::create();
        maxInt = Int::max();
        defaultPrimeFacLimit = NumPowerSimpl::getMaxPrimeResolution();
    }

    void teardown()
    {
        NumPowerSimpl::setMaxPrimeResolution(defaultPrimeFacLimit);
    }
};

TEST(Power, typeStr)
{
    const std::string expected("Power");
    const BasePtr ptr = Power::create(a, two);

    CHECK_EQUAL(expected, ptr->typeStr());
}

TEST(Power, undefinedExponent)
{
    const BasePtr res = Power::create(a, undefPtr);

    CHECK(res->isUndefined());
}

TEST(Power, undefinedNumberExponent)
{
    const Number n(Number::createUndefined());
    const BasePtr res = Power::create(a, Numeric::create(n));

    CHECK(res->isUndefined());
}

TEST(Power, undefinedBase)
{
    const BasePtr res = Power::create(undefPtr, two);

    CHECK(res->isUndefined());
}

TEST(Power, integerOnly)
{
    const BasePtr res = Power::create(three, four);

    CHECK(res->isNumeric());
    CHECK_EQUAL(81, res->numericEval());
}

TEST(Power, exponentFraction)
    /* 2^(2/3) is not simplified. */
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr pow = Power::create(two, exp);

    CHECK(pow->isPower());
    CHECK_EQUAL(two, pow->base());
    CHECK_EQUAL(exp, pow->exp());
}

TEST(Power, largeBaseIntegerExp)
{
    const long maxLong = std::numeric_limits<long>::max();
    const long lBase = (long)std::sqrt((double)(maxLong - 2));
    const BasePtr base = Numeric::create(Int(lBase));
    const BasePtr pow = Power::create(base, two);

    CHECK_EQUAL(Numeric::create(Int(lBase*lBase)), pow);
}

TEST(Power, largeBaseIntegerExpIntOverflow)
{
    const BasePtr expected = Numeric::create(std::pow(Int::max().toDouble(), 2));
    const BasePtr base = Numeric::create(Int::max());
    BasePtr pow;

    disableLog();
    pow = Power::create(base, two);
    enableLog();

    CHECK_EQUAL(expected, pow);
}

TEST(Power, expFracGreaterThanOne)
    /* 2^(10/3) = 8*2^(1/3). */
{
    const BasePtr expectedPow = Power::create(two, Numeric::create(1, 3));
    const BasePtr res = Power::create(two, Numeric::create(10, 3));

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(eight, res->operands().front());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, baseFracWithExpFracGreaterThanOne)
    /* (2/3)^(3/2) = (2/3)*sqrt(2/3). */
{
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr expectedPow = Power::sqrt(twoThird);
    const BasePtr res = Power::create(twoThird, Numeric::create(3, 2));

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(twoThird, res->operands().front());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, expFracLessThanMinusOne)
    /* (5/13)^(-19/7) = (169/25)*(13/5)^(5/7).  */
{
    const BasePtr expectedPow = Power::create(Numeric::create(13, 5), Numeric::create(5, 7));
    const BasePtr origBase = Numeric::create(5, 13);
    const BasePtr origExp = Numeric::create(-19, 7);
    const BasePtr res = Power::create(origBase, origExp);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(Numeric::create(169, 25), res->operands().front());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, simpleFracBaseToIntWithNegExp)
    /* sqrt(1/2) = 2^(-1/2). */
{
    const BasePtr pow = Power::sqrt(half);

    CHECK(pow->isPower());
    CHECK_EQUAL(two, pow->base());
    CHECK_EQUAL(minusHalf, pow->exp());
}

TEST(Power, illegalPower)
    /* sqrt(-2) = Undefined. */
{
    const BasePtr result = Power::sqrt(Numeric::create(-2));

    CHECK(result->isUndefined());
}

TEST(Power, fracBaseToIntWithNegExp)
    /* (1/12345)^(1/5) = 12345^(-1/5). */
{
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr minusOneFifth = Numeric::create(-1, 5);
    const BasePtr baseFrac = Numeric::create(1, 12345);
    const BasePtr pow = Power::create(baseFrac, oneFifth);

    CHECK(pow->isPower());
    CHECK_EQUAL(12345, pow->base()->numericEval());
    CHECK_EQUAL(minusOneFifth, pow->exp());
}

TEST(Power, evalLargeNumericExponent)
    /* 5^(125/3) = 5^41*5^(2/3) which is converted to a double because of an overflow. */
{
    const BasePtr expected = Numeric::create(std::pow(5.0, 125.0/3.0));
    BasePtr result;

    disableLog();
    result = Power::create(five, Numeric::create(125, 3));
    enableLog();

    CHECK_EQUAL(expected, result);
}

TEST(Power, fracBaseNegFracExp)
    /* (2/3)^(-1/2) = sqrt(3/2). If the fraction base can't be converted to an integer by changing
     * the sign of the exponent, a positive sign of the exponent is preferred over a negative one.
     * */
{
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr pow = Power::create(twoThird, minusHalf);

    CHECK(pow->isPower());
    CHECK_EQUAL(Numeric::create(3, 2), pow->base());
    CHECK_EQUAL(half, pow->exp());
}

TEST(Power, fracBaseNoChange)
    /* No change of sqrt(2/3). */
{
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr pow = Power::sqrt(twoThird);

    CHECK(pow->isPower());
    CHECK_EQUAL(twoThird, pow->base());
    CHECK_EQUAL(half, pow->exp());
}

TEST(Power, resolvableExpFracBaseInt)
    /* 8^(1/3) = 2. */
{
    const BasePtr exp = Numeric::create(1, 3);
    const BasePtr res = Power::create(eight, exp);

    CHECK(res->isNumeric());
    CHECK_EQUAL(2, res->numericEval());
}

TEST(Power, simpleSplittableExpFracBaseInt)
    /* sqrt(8) = 2*sqrt(2). */
{
    const BasePtr res = Power::sqrt(eight);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(two, res->operands().front());
    CHECK_EQUAL(sqrtTwo, res->operands().back());
}

TEST(Power, splittableExpFracBaseInt)
    /* sqrt(32) = 4*sqrt(2). */
{
    const BasePtr res = Power::sqrt(Numeric::create(32));
    const BasePtr expectedPow = Power::sqrt(two);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(4, res->operands().front()->numericEval());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, splittableExpSqrtBaseInt)
    /* sqrt(833) = 7*sqrt(17). */
{
    const BasePtr res = Power::sqrt(Numeric::create(833));
    const BasePtr expectedPow = Power::sqrt(Numeric::create(17));

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(7, res->operands().front()->numericEval());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, splittableExpFracBaseOneEigth)
    /* sqrt(1/8) = 1/2*2^(-1/2). */
{
    const BasePtr oneEigth = Numeric::create(1, 8);
    const BasePtr res = Power::sqrt(oneEigth);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(half, res->operands().front());
    CHECK_EQUAL(Power::create(two, minusHalf), res->operands().back());
}

TEST(Power, splittableNegExpFracBaseOneThirtyHalf)
    /* sqrt(1/32) = 1/4*2^(-1/2). */
{
    const BasePtr res = Power::sqrt(Numeric::create(1, 32));
    const BasePtr expectedPow = Power::create(two, minusHalf);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(Numeric::create(1, 4), res->operands().front());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, splittableExpFracBaseThreeEigth)
    /* sqrt(3/8) = 1/2*2^(-1/2). */
{
    const BasePtr threeEigth = Numeric::create(3, 8);
    const BasePtr res = Power::sqrt(threeEigth);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(half, res->operands().front());
    CHECK_EQUAL(Power::sqrt(Numeric::create(3, 2)), res->operands().back());
}

TEST(Power, splittableExpFracBaseFrac)
    /* (54/4375)^(1/3) = 3/5*(2/35)^(1/3), if the limit value for prime factorization is high
     * enough. */
{
    const BasePtr base = Numeric::create(54, 4375);
    BasePtr res;

    res = Power::create(base, oneThird);

    CHECK(res->isPower());
    CHECK_EQUAL(oneThird, res->exp());
    CHECK_EQUAL(base, res->base());

    NumPowerSimpl::setMaxPrimeResolution(10000);

    res = Power::create(base, oneThird);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(Numeric::create(3, 5), res->operands().front());
    CHECK_EQUAL(oneThird, res->operands().back()->exp());
    CHECK_EQUAL(Numeric::create(2, 35), res->operands().back()->base());
}

TEST(Power, splittableBaseNumeratorExpFrac)
    /* (168080/17)^(2/5) = 49*(10/17)^(2/5), if the limit value for prime factorization is high
     * enough. */
{
    const BasePtr exp = Numeric::create(2, 5);
    const BasePtr origBase = Numeric::create(168070, 17);
    const BasePtr expectedPow = Power::create(Numeric::create(10, 17), exp);
    BasePtr res;

    res = Power::create(origBase, exp);

    CHECK(res->isPower());
    CHECK_EQUAL(origBase, res->base());
    CHECK_EQUAL(exp, res->exp());

    NumPowerSimpl::setMaxPrimeResolution(200000);

    res = Power::create(origBase, exp);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(Numeric::create(49), res->operands().front());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, splittalbeBaseDenomExpFrac)
    /* (21/10648)^(5/3) = 21/5153632*21^(2/3), if the limit value for prime factorization is high
     * enough. */
{
    const BasePtr origExp = Numeric::create(5, 3);
    const BasePtr origBase = Numeric::create(21, 10648);
    const BasePtr resultingExp = Numeric::create(2, 3);
    const BasePtr expectedPow = Power::create(Numeric::create(21), resultingExp);
    const BasePtr expectedNumeric = Numeric::create(21, 5153632);
    BasePtr res;

    res = Power::create(origBase, origExp);

    CHECK(res->isPower());
    CHECK_EQUAL(origExp, res->exp());
    CHECK_EQUAL(origBase, res->base());

    NumPowerSimpl::setMaxPrimeResolution(10648);

    res = Power::create(origBase, origExp);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(expectedNumeric, res->operands().front());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, splittableNegExpFracBaseFrac)
    /* (8/81)^(-4/3) = 243/16*3^(1/3). */
{
    const BasePtr expectedNumeric = Numeric::create(243, 16);
    const BasePtr expectedPow = Power::create(three, oneThird);
    BasePtr res;

    res = Power::create(Numeric::create(8, 81), Numeric::create(-4, 3));

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(expectedNumeric, res->operands().front());
    CHECK_EQUAL(expectedPow, res->operands().back());
}

TEST(Power, simpleUnresolvableExpFrac)
    /* No change of sqrt(2). */
{
    const BasePtr pow = Power::sqrt(two);

    CHECK(pow->isPower());
    CHECK(pow->isNumericPower());
    DOUBLES_EQUAL(std::sqrt(2.0), pow->numericEval().toDouble(), TOL);
}

TEST(Power, intBaseDoubleExp)
    /* 2^0.123456789 = 1.0893418703486832. */
{
    const BasePtr pow = Power::create(two, Numeric::create(0.123456789));
    const BasePtr expected = Numeric::create(1.0893418703486832);

    /* Uses the internal tolerance. */
    CHECK_EQUAL(expected, pow);
    DOUBLES_EQUAL(1.0893418703486832, pow->numericEval().toDouble(), TOL);
}

TEST(Power, doubleBaseDoubleExpToInt)
    /* Double power that evalautes to exactly 2. */
{
    const BasePtr expected = two;
    const BasePtr pow = Power::create(Numeric::create(1.0893418703486832),
            Numeric::create(1.0/0.123456789));

    CHECK_EQUAL(expected, pow);
}

TEST(Power, unresolvableExpFrac)
    /* No change of 7^(1/3). */
{
    const BasePtr pow = Power::create(seven, oneThird);

    CHECK(pow->isPower());
    CHECK(pow->isNumericPower());

    DOUBLES_EQUAL(std::pow(7.0, 1.0/3.0), pow->numericEval().toDouble(), TOL);
}

TEST(Power, zeroExponent)
{
    const BasePtr res = Power::create(a, zero);

    CHECK(res->isNumeric());
    CHECK_EQUAL(1, res->numericEval());
}

TEST(Power, identity)
{
    const BasePtr res = Power::create(a, one);
    const Name expected("a");

    CHECK(res->isSymbol());
    CHECK_EQUAL(expected, res->name());
}

TEST(Power, multiplySymbolExp)
    /* (2^a)^a = 2^(a^2). */
{
    const BasePtr pow1 = Power::create(two, a);
    const BasePtr res = Power::create(pow1, a);

    CHECK(res->isPower());

    CHECK(res->base()->isNumeric());
    CHECK(res->exp()->isPower());

    CHECK_EQUAL(2, res->base()->numericEval());
    CHECK_EQUAL(2, res->exp()->exp()->numericEval());
}

TEST(Power, multiplyExpWithZeroExp)
    /* (a^2)^0 = 1. */
{
    const BasePtr pow1 = Power::create(a, two);
    BasePtr res;

    res = Power::create(pow1, zero);

    CHECK_EQUAL(one, res);
}

TEST(Power, multiplyNumericExp)
    /* (2^(2/3))^(6/7) = 2^(4/7). */
{
    const BasePtr expected = Power::create(two, Numeric::create(4, 7));
    const BasePtr pow1 = Power::create(two, Numeric::create(2, 3));
    BasePtr res;

    res = Power::create(pow1, Numeric::create(6, 7));

    CHECK_EQUAL(expected, res);
}

TEST(Power, multiplyNumericExpToDouble)
    /* (sqrt(2))^1.23456789 shall be evaluated to a double numeric. */
{
    const double exp = 1.23456789;
    const BasePtr expected = Numeric::create(std::pow(std::pow(2, 0.5), exp));
    BasePtr res;

    res = Power::create(sqrtTwo, Numeric::create(exp));

    CHECK_EQUAL(expected, res);
}

TEST(Power, wrongDoubleEvaluationRequest)
{
    const BasePtr pow = Power::create(a, two);
    Number result;

    result = pow->numericEval();

    CHECK(result.isUndefined());
}

TEST(Power, multiplyExponentsByCreation)
{
    const BasePtr pow1 = Power::create(a, two);
    const BasePtr pow2 = Power::create(pow1, three);
    const BasePtr res = Power::create(pow2, one);

    CHECK(res->isPower());
    CHECK_EQUAL(a, res->operands().front());
    CHECK_EQUAL(six, res->operands().back());
}

TEST(Power, oneExpByMultiplicationByCreation)
    /* (a^2)^(1/2) = a. */
{
    const BasePtr pow1 = Power::create(a, two);
    const BasePtr res = Power::sqrt(pow1);
    const Name expected("a");

    /* (a^2)^(1/2) should be 'a' upon creation, thus, a symbol. */
    CHECK(res->isSymbol());
    CHECK_EQUAL(expected, res->name());
}

TEST(Power, applyExponentToProduct)
    /* (a*b^2)^3 = (a^2)*(b^6) */
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr product = Product::create(a, bSquare);
    const BasePtr res = Power::create(product, three);
    std::list<BasePtr>::const_iterator it;
    BasePtr fac;

    CHECK(res->isProduct());

    it = res->operands().begin();
    fac = *it;
    CHECK(fac->isPower());
    CHECK(fac->base()->name() == Name("a"));
    CHECK_EQUAL(three, fac->exp());

    ++it;
    fac = *it;
    CHECK(fac->isPower());
    CHECK(fac->base()->name() == Name("b"));
    CHECK_EQUAL(six, fac->exp());
}

TEST(Power, intOverflowLargeBase)
    /* A large base with exponent > 1 causes an integer overflow. The power is converted into a
     * numeric of type double.  */
{
    const BasePtr expected = Numeric::create(std::pow(maxInt.toDouble(), 3.0));
    BasePtr pow;

    disableLog();
    pow = Power::create(Numeric::create(maxInt), three);
    enableLog();

    CHECK_EQUAL(expected, pow);
}

TEST(Power, intOverflowNumericExpMultiplication)
    /* (a^b)^c shall be evaluated to a double, if b and c are rational numbers that lead to an
     * integer overflow. This test case depends on the zero tolerance of the Number class! */
{
    const Number exp1(1, maxInt - 1000);
    const Number exp2(1, 3);
    const BasePtr expected = Power::create(a, Numeric::create(exp1.toDouble()*exp2.toDouble()));
    const BasePtr pow1 = Power::create(a, Numeric::create(exp1));
    BasePtr res;

    disableLog();
    res = Power::create(pow1, Numeric::create(exp2));
    enableLog();

    CHECK_EQUAL(expected, res);
}

TEST(Power, sumBase)
    /* No simplification of (a + b)^(a + b). */
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr res = Power::create(sum, sum);

    CHECK_EQUAL(sum, res->exp());
    CHECK_EQUAL(sum, res->base());
}

TEST(Power, constantBase)
    /* No simplification of Pi^(a + b). */
{
    const BasePtr exp = Sum::create(a, b);
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Power::create(pi, exp);

    CHECK_EQUAL(exp, res->exp());
    CHECK_EQUAL(pi, res->base());
}

TEST(Power, numericBaseConstantExp)
    /* No simplification of 2^Pi. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Power::create(two, pi);

    CHECK_EQUAL(pi, res->exp());
    CHECK_EQUAL(two, res->base());
}

TEST(Power, productBaseConstantExp)
    /* (a*b)^Pi = a^Pi*b^Pi. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Power::create(Product::create(a, b), pi);

    CHECK(res->isProduct());
    CHECK_EQUAL(Power::create(a, pi), res->operands().front());
    CHECK_EQUAL(Power::create(b, pi), res->operands().back());
}

TEST(Power, functionBase)
    /* No simplification of sin(a)^a. */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr res = Power::create(sin, a);

    CHECK(res->isPower());
    CHECK_EQUAL(sin, res->base());
    CHECK_EQUAL(a, res->exp());
}

TEST(Power, eulerBaseLogExp)
    /* e^log(a + b + sqrt(2)) = a + b + sqrt(2). */
{
    const BasePtr arg = Sum::create(a, b, Power::sqrt(two));
    const BasePtr result = Power::create(Constant::createE(), Logarithm::create(arg));

    CHECK_EQUAL(arg, result);
}
