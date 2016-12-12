
#include <cmath>
#include <limits>
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
#include "cpputest.h"

using namespace tsym;

const double TOL = 1.e-10;

TEST_GROUP(Power)
{
    BasePtr aPos;
    BasePtr bPos;
    BasePtr half;
    BasePtr oneThird;
    BasePtr minusHalf;
    BasePtr sqrtTwo;
    BasePtr undefPtr;
    BasePtr pi;
    Int maxInt;
    Int defaultPrimeFacLimit;

    void setup()
    {
        aPos = Symbol::createPositive("a");
        bPos = Symbol::createPositive("b");
        half = Numeric::create(1, 2);
        oneThird = Numeric::create(1, 3);
        minusHalf = Numeric::create(-1, 2);
        sqrtTwo = Power::sqrt(two);
        undefPtr = Undefined::create();
        pi = Constant::createPi();
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

TEST(Power, negFractionBaseWithNegFractionExp)
{
    const BasePtr res = Power::create(minusHalf, half);

    CHECK(res->isUndefined());
}

TEST(Power, negFractionBaseWithPosFractionExp)
{
    const BasePtr res = Power::create(minusHalf, minusHalf);

    CHECK(res->isUndefined());
}

TEST(Power, negNumericBasePosNumericExp)
{
    const BasePtr res = Power::create(minusHalf, Numeric::create(9.87654321));
    CHECK(res->isUndefined());
}

TEST(Power, negNumericBaseNegNumericExp)
{
    const BasePtr res = Power::create(minusHalf, Numeric::create(-1.23456789));

    CHECK(res->isUndefined());
}

TEST(Power, unclearBaseExponentContractionBothFractionsOddDenom)
    /* (a^(2/3))^(4/5) = a^(8/15) for a neither positive nor negative. */
{
    const BasePtr expected = Power::create(a, Numeric::create(8, 15));
    const BasePtr res = Power::create(Power::create(a, Numeric::create(2, 3)),
            Numeric::create(4, 5));

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseExponentContractionBothFractionsEvenDenom)
    /* (a^(1/8))^(3/10) = a^(3/80). */
{
    const BasePtr expected = Power::create(a, Numeric::create(3, 80));
    const BasePtr res = Power::create(Power::create(a, Numeric::create(1, 8)),
            Numeric::create(3, 10));

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseExponentContractionBothFractions)
    /* (a^(1/2))^(1/3) = a^(1/6) for a neither positive nor negative. */
{
    const BasePtr expected = Power::create(a, Numeric::create(1, 6));
    const BasePtr res = Power::create(Power::create(a, half), oneThird);

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseExponentContractionWithInteger)
    /* (a^(1/6))^3 = sqrt(a) for a neither positive nor negative. */
{
    const BasePtr res = Power::create(Power::create(a, Numeric::create(1, 6)), three);
    const BasePtr expected = Power::sqrt(a);

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseExpContraction)
    /* (a^7)^(1/4) = a^(7/4). This works because a is assumed to be real, not possibly complex (this
     * is different in most CAS). */
{
    const BasePtr res = Power::create(Power::create(a, seven), Numeric::create(1, 4));
    const BasePtr expected = Power::create(a, Numeric::create(7, 4));

    CHECK_EQUAL(expected, res);
}

TEST(Power, negBaseSymbolicExp)
    /* ((-a - b)^c)^2 isn't simplified. */
{
    const BasePtr base = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr res = Power::create(Power::create(base, c), two);

    CHECK(res->isPower());
    CHECK_EQUAL(two, res->exp());

    CHECK(res->base()->isPower());
    CHECK_EQUAL(c, res->base()->exp());
    CHECK_EQUAL(base, res->base()->base());
}

TEST(Power, unclearProductBaseFractionExp)
    /* (a*b)^(2/3) can't be simplified, because a*b could be > 0 with a, b < 0. */
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr res = Power::create(Product::create(a, b), exp);

    CHECK(res->isPower());
    CHECK_EQUAL(Product::create(a, b), res->base());
    CHECK_EQUAL(exp, res->exp());
}

TEST(Power, extractAllFactorsOfProductBase)
    /* (a*b*c*d*e*pi)^8 = a^8*b^8*c^8*d^8*e^8*pi^8. */
{
    BasePtrList::const_iterator it;
    BasePtrList factors;
    BasePtr res;

    factors.push_back(a);
    factors.push_back(b);
    factors.push_back(c);
    factors.push_back(d);
    factors.push_back(e);
    factors.push_back(pi);

    res = Power::create(Product::create(factors), eight);

    CHECK(res->isProduct());

    for (it = res->operands().begin(); it != res->operands().end(); ++it) {
        CHECK((*it)->isPower());
        CHECK_EQUAL(eight, (*it)->exp());
    }
}

TEST(Power, extractPositiveFactorsOfProductBase)
    /* (a*b*c*d*e*pi)^(2/3) = a^(2/3)*b^(2/3)*pi^(2/3)*(c*d*e)^(2/3) for a, b, > 0. */
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr expected[] = { Power::create(pi, exp), Power::create(aPos, exp),
        Power::create(bPos, exp), Power::create(Product::create(c, d, e), exp) };
    BasePtrList::const_iterator it;
    BasePtrList factors;
    BasePtr res;

    factors.push_back(aPos);
    factors.push_back(bPos);
    factors.push_back(c);
    factors.push_back(d);
    factors.push_back(e);
    factors.push_back(pi);

    res = Power::create(Product::create(factors), exp);

    CHECK(res->isProduct());

    factors = res->operands();

    for (int i = 0; i < 4; ++i) {
        res = factors.pop_front();

        CHECK_EQUAL(expected[i], res);
    }
}

TEST(Power, negProductBaseFractionExp)
    /* (-a*b)^(2/3) is undefined for a, b > 0. */
{
    const BasePtr base = Product::create(Numeric::mOne(), aPos, bPos);
    const BasePtr res = Power::create(base, Numeric::create(2, 3));

    CHECK(res->isUndefined());
}

TEST(Power, zeroExponent)
{
    const BasePtr res = Power::create(a, zero);

    CHECK(res->isNumeric());
    CHECK_EQUAL(1, res->numericEval());
}

TEST(Power, symbolicPowerBaseSymbolExp)
    /* (a^b)^c isn't simplified. */
{
    const BasePtr base = Power::create(a, b);
    const BasePtr res = Power::create(base, c);

    CHECK(res->isPower());
    CHECK_EQUAL(base, res->base());
    CHECK_EQUAL(c, res->exp());
}

TEST(Power, zeroBaseToPosExponent)
    /* 0^a = 0 with a > 0. */
{
    const BasePtr res = Power::create(zero, aPos);

    CHECK(res->isZero());
}

TEST(Power, zeroBaseToUnclearExponent)
    /* 0^a = 0. */
{
    const BasePtr res = Power::create(zero, a);

    CHECK(res->isZero());
}

TEST(Power, zeroBaseToNegExponent)
    /* 0^(-a) with a > 0 is Undefined. */
{
    const BasePtr exp = Product::minus(aPos);
    BasePtr res;

    disableLog();
    res = Power::create(zero, exp);
    enableLog();

    CHECK(res->isUndefined());
}

TEST(Power, identity)
{
    const BasePtr res = Power::create(a, one);
    const Name expected("a");

    CHECK(res->isSymbol());
    CHECK_EQUAL(expected, res->name());
}

TEST(Power, powerWithPowerExp)
{
    const BasePtr exp = Power::create(Sum::create(a, b), two);
    const BasePtr res = Power::create(a, exp);

    CHECK(res->isPower());
    CHECK(res->exp()->isPower());

    CHECK_EQUAL(a, res->base());
    CHECK_EQUAL(exp, res->exp());
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
    /* ((a^2)^3)^1 = a^6. */
{
    const BasePtr pow1 = Power::create(a, two);
    const BasePtr pow2 = Power::create(pow1, three);
    const BasePtr res = Power::create(pow2, one);

    CHECK(res->isPower());
    CHECK_EQUAL(a, res->base());
    CHECK_EQUAL(six, res->exp());
}

TEST(Power, squareOfSymbolSquareRoot)
    /* (sqrt(a))^2 isn't simplified, if the sign of a is unknown. */
{
    const BasePtr res = Power::create(Power::sqrt(a), two);

    CHECK(res->isPower());
    CHECK_EQUAL(two, res->exp());
    CHECK_EQUAL(Power::sqrt(a), res->base());
}

TEST(Power, squareOfPosSymbolSquareRoot)
    /* (sqrt(a))^2 = a for positive a. */
{
    const BasePtr res = Power::create(Power::sqrt(aPos), two);

    CHECK_EQUAL(aPos, res);
}

TEST(Power, squareRootOfPosSymbolSquare)
    /* sqrt(a^2) = a for positive a. */
{
    const BasePtr res = Power::sqrt(Power::create(aPos, two));

    CHECK_EQUAL(aPos, res);
}

TEST(Power, powerOfNegSymbolSquare)
    /* ((-a - b)^2)^(-1/2) = (a + b)^(-1) for positive a, b. */
{
    const BasePtr base = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr res = Power::create(Power::create(base, two), minusHalf);

    CHECK_EQUAL(Power::oneOver(Sum::create(aPos, bPos)), res);
}

TEST(Power, powerOfNegSymbolSumChangesSign)
    /* ((-a - b)^2)^(1/6) = (a + b)^(1/3) for a, b, > 0. */
{
    const BasePtr base = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr res = Power::create(Power::create(base, two), Numeric::create(1, 6));
    const BasePtr expected = Power::create(Sum::create(aPos, bPos), oneThird);

    CHECK_EQUAL(expected, res);
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
    const BasePtr res = Power::create(pi, exp);

    CHECK_EQUAL(exp, res->exp());
    CHECK_EQUAL(pi, res->base());
}

TEST(Power, numericBaseConstantExp)
    /* No simplification of 2^Pi. */
{
    const BasePtr res = Power::create(two, pi);

    CHECK_EQUAL(pi, res->exp());
    CHECK_EQUAL(two, res->base());
}

TEST(Power, productBaseConstantExp)
    /* (a*b)^Pi = a^Pi*b^Pi for a > 0, b unclear. */
{
    const BasePtr res = Power::create(Product::create(aPos, b), pi);

    CHECK(res->isProduct());
    CHECK_EQUAL(Power::create(aPos, pi), res->operands().front());
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

TEST(Power, negativePowByOddExp)
    /* (-aPos)^(3/5) is undefined. */
{
    const BasePtr pow = Power::create(Product::minus(aPos), Numeric::create(3, 5));

    CHECK(pow->isUndefined());
}

TEST(Power, unclearSymbolBaseLeftUnchanged)
{
    const BasePtr base = Power::create(a, oneThird);
    const BasePtr res = Power::create(base, three);

    CHECK(res->isPower());
    CHECK_EQUAL(three, res->exp());
    CHECK_EQUAL(base, res->base());
}

TEST(Power, squareRootOfNegBaseSquared)
    /* ((-a - b)^2)^(-1/2) = (a + b)^(-1). */
{
    const BasePtr arg = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr base = Power::create(arg, two);
    const BasePtr res = Power::create(base, minusHalf);

    CHECK_EQUAL(Power::oneOver(Sum::create(aPos, bPos)), res);
}

TEST(Power, negativeBaseOfPowerToTheOneThird)
    /* ((-a - b)^2)^(1/3) = (a + b)^(2/3). */
{
    const BasePtr arg = Product::minus(Sum::create(aPos, bPos));
    const BasePtr res = Power::create(Power::create(arg, two), oneThird);
    const BasePtr expected = Power::create(Sum::create(aPos, bPos), Numeric::create(2, 3));

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseDoubleExpContraction)
    /* The exponents of (a^(0.123456789))^1.23456789 are contracted. */
{
    const BasePtr e1 = Numeric::create(0.123456789);
    const BasePtr e2 = Numeric::create(1.23456789);
    const BasePtr res = Power::create(Power::create(a, e1), e2);
    const BasePtr expected = Power::create(a, Product::create(e1, e2));

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseExpContractionWithConstantFirstExp)
    /* (a^pi)^3 = a^(3*pi). */
{
    const BasePtr res = Power::create(Power::create(a, pi), three);
    const BasePtr expected = Power::create(a, Product::create(three, pi));

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseExpContractionWithConstantSecondExp)
    /* (a^3)^(-pi) = a^(-3*pi). */
{
    const BasePtr res = Power::create(Power::create(a, three), pi);
    const BasePtr expected = Power::create(a, Product::create(three, pi));

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseExpContractionWithConstantFirstExpSecondEvenInt)
    /* ((-a)^pi)^(-8) = (-a)^(-8*pi). */
{
    const BasePtr base = Product::minus(a);
    const BasePtr res = Power::create(Power::create(base, pi), Numeric::create(-8));
    const BasePtr expected = Power::create(base, Product::minus(eight, pi));

    CHECK_EQUAL(expected, res);
}

TEST(Power, unclearBaseNoExpContractionWithConstantSecondExp)
    /* (a^8)^pi isn't simplified. */
{
    const BasePtr res = Power::create(Power::create(a, eight), pi);

    CHECK(res->isPower());
    CHECK_EQUAL(pi, res->exp());

    CHECK(res->base()->isPower());
    CHECK_EQUAL(eight, res->base()->exp());
    CHECK_EQUAL(a, res->base()->base());
}

TEST(Power, unclearBaseNoExpContractionWithSymbolSecondExp)
    /* (a^(1/4))^b isn't simplified because a^(1/4) can be Undefined, while b = 4 is possible. */
{
    const BasePtr res = Power::create(Power::create(a, Numeric::create(1, 4)), b);

    CHECK(res->isPower());
    CHECK_EQUAL(b, res->exp());

    CHECK(res->base()->isPower());
    CHECK_EQUAL(Numeric::create(1, 4), res->base()->exp());
    CHECK_EQUAL(a, res->base()->base());
}

TEST(Power, numericallyEvaluableExpToUndefined)
    /* (-2)^pi is Undefined. */
{
    const BasePtr res = Power::create(Numeric::create(-2), pi);

    CHECK(res->isUndefined());
}
