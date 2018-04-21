
#include <cmath>
#include <limits>
#include "power.h"
#include "symbol.h"
#include "numeric.h"
#include "undefined.h"
#include "product.h"
#include "constant.h"
#include "bplist.h"
#include "trigonometric.h"
#include "options.h"
#include "logarithm.h"
#include "sum.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

struct PowerFixture : public AbcFixture {
    const Int defaultPrimeFacLimit = options::getMaxPrimeResolution();
    const BasePtr aPos = Symbol::createPositive("a");
    const BasePtr bPos = Symbol::createPositive("b");
    const BasePtr& half = Numeric::half();
    const BasePtr& oneThird = Numeric::third();
    const BasePtr minusHalf = Numeric::create(-1, 2);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const double TOL = 1.e-10;

    ~PowerFixture()
    {
        options::setMaxPrimeResolution(defaultPrimeFacLimit);
    }
};

BOOST_FIXTURE_TEST_SUITE(TestPower, PowerFixture)

BOOST_AUTO_TEST_CASE(typeStr)
{
    const std::string expected("Power");
    const BasePtr ptr = Power::create(a, two);

    BOOST_CHECK_EQUAL(expected, ptr->typeStr());
}

BOOST_AUTO_TEST_CASE(undefinedExponent)
{
    const BasePtr res = Power::create(a, undefined);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(illegalNumberExponent, noLogs())
{
    const BasePtr res = Power::create(a, Numeric::create(10, 0));

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(undefinedBase)
{
    const BasePtr res = Power::create(undefined, two);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(integerOnly)
{
    const BasePtr res = Power::create(three, four);

    BOOST_TEST(res->isNumeric());
    BOOST_CHECK_EQUAL(81, res->numericEval());
}

BOOST_AUTO_TEST_CASE(exponentFraction)
    /* 2^(2/3) is not simplified. */
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr pow = Power::create(two, exp);

    BOOST_TEST(pow->isPower());
    BOOST_CHECK_EQUAL(two, pow->base());
    BOOST_CHECK_EQUAL(exp, pow->exp());
}

BOOST_AUTO_TEST_CASE(largeBaseIntegerExp)
{
    const long maxLong = std::numeric_limits<long>::max();
    const long lBase = static_cast<long>(std::sqrt(static_cast<double>(maxLong - 2)));
    const BasePtr base = Numeric::create(Int(lBase));
    const BasePtr pow = Power::create(base, two);

    BOOST_CHECK_EQUAL(Numeric::create(Int(lBase*lBase)), pow);
}

BOOST_AUTO_TEST_CASE(expFracGreaterThanOne)
    /* 2^(10/3) = 8*2^(1/3). */
{
    const BasePtr expectedPow = Power::create(two, Numeric::third());
    const BasePtr res = Power::create(two, Numeric::create(10, 3));

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(eight, res->operands().front());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(baseFracWithExpFracGreaterThanOne)
    /* (2/3)^(3/2) = (2/3)*sqrt(2/3). */
{
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr expectedPow = Power::sqrt(twoThird);
    const BasePtr res = Power::create(twoThird, Numeric::create(3, 2));

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(twoThird, res->operands().front());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(expFracLessThanMinusOne)
    /* (5/13)^(-19/7) = (169/25)*(13/5)^(5/7).  */
{
    const BasePtr expectedPow = Power::create(Numeric::create(13, 5), Numeric::create(5, 7));
    const BasePtr origBase = Numeric::create(5, 13);
    const BasePtr origExp = Numeric::create(-19, 7);
    const BasePtr res = Power::create(origBase, origExp);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(Numeric::create(169, 25), res->operands().front());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(simpleFracBaseToIntWithNegExp)
    /* sqrt(1/2) = 2^(-1/2). */
{
    const BasePtr pow = Power::sqrt(half);

    BOOST_TEST(pow->isPower());
    BOOST_CHECK_EQUAL(two, pow->base());
    BOOST_CHECK_EQUAL(minusHalf, pow->exp());
}

BOOST_AUTO_TEST_CASE(illegalPower)
    /* sqrt(-2) = Undefined. */
{
    const BasePtr result = Power::sqrt(Numeric::create(-2));

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(fracBaseToIntWithNegExp)
    /* (1/12345)^(1/5) = 12345^(-1/5). */
{
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr minusOneFifth = Numeric::create(-1, 5);
    const BasePtr baseFrac = Numeric::create(1, 12345);
    const BasePtr pow = Power::create(baseFrac, oneFifth);

    BOOST_TEST(pow->isPower());
    BOOST_CHECK_EQUAL(12345, pow->base()->numericEval());
    BOOST_CHECK_EQUAL(minusOneFifth, pow->exp());
}

BOOST_AUTO_TEST_CASE(evalLargeNumericExponent)
    /* 5^(125/3) = 5^41*5^(2/3) = [large int]*5^(2/3). */
{
    const BasePtr result = Power::create(five, Numeric::create(125, 3));
    const Int prefac = Int("45474735088646411895751953125");
    const BasePtr expected = Product::create(Numeric::create(prefac),
            Power::create(five, Numeric::create(2, 3)));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(fracBaseNegFracExp)
    /* (2/3)^(-1/2) = sqrt(3/2). If the fraction base can't be converted to an integer by changing
     * the sign of the exponent, a positive sign of the exponent is preferred over a negative one.
     * */
{
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr pow = Power::create(twoThird, minusHalf);

    BOOST_TEST(pow->isPower());
    BOOST_CHECK_EQUAL(Numeric::create(3, 2), pow->base());
    BOOST_CHECK_EQUAL(half, pow->exp());
}

BOOST_AUTO_TEST_CASE(fracBaseNoChange)
    /* No change of sqrt(2/3). */
{
    const BasePtr twoThird = Numeric::create(2, 3);
    const BasePtr pow = Power::sqrt(twoThird);

    BOOST_TEST(pow->isPower());
    BOOST_CHECK_EQUAL(twoThird, pow->base());
    BOOST_CHECK_EQUAL(half, pow->exp());
}

BOOST_AUTO_TEST_CASE(resolvableExpFracBaseInt)
    /* 8^(1/3) = 2. */
{
    const BasePtr exp = Numeric::third();
    const BasePtr res = Power::create(eight, exp);

    BOOST_TEST(res->isNumeric());
    BOOST_CHECK_EQUAL(2, res->numericEval());
}

BOOST_AUTO_TEST_CASE(simpleSplittableExpFracBaseInt)
    /* sqrt(8) = 2*sqrt(2). */
{
    const BasePtr res = Power::sqrt(eight);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(two, res->operands().front());
    BOOST_CHECK_EQUAL(sqrtTwo, res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittableExpFracBaseInt)
    /* sqrt(32) = 4*sqrt(2). */
{
    const BasePtr res = Power::sqrt(Numeric::create(32));
    const BasePtr expectedPow = Power::sqrt(two);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(4, res->operands().front()->numericEval());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittableExpSqrtBaseInt)
    /* sqrt(833) = 7*sqrt(17). */
{
    const BasePtr res = Power::sqrt(Numeric::create(833));
    const BasePtr expectedPow = Power::sqrt(Numeric::create(17));

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(7, res->operands().front()->numericEval());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittableExpFracBaseOneEigth)
    /* sqrt(1/8) = 1/2*2^(-1/2). */
{
    const BasePtr oneEigth = Numeric::create(1, 8);
    const BasePtr res = Power::sqrt(oneEigth);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(half, res->operands().front());
    BOOST_CHECK_EQUAL(Power::create(two, minusHalf), res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittableNegExpFracBaseOneThirtyHalf)
    /* sqrt(1/32) = 1/4*2^(-1/2). */
{
    const BasePtr res = Power::sqrt(Numeric::create(1, 32));
    const BasePtr expectedPow = Power::create(two, minusHalf);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(Numeric::fourth(), res->operands().front());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittableExpFracBaseThreeEigth)
    /* sqrt(3/8) = 1/2*2^(-1/2). */
{
    const BasePtr threeEigth = Numeric::create(3, 8);
    const BasePtr res = Power::sqrt(threeEigth);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(half, res->operands().front());
    BOOST_CHECK_EQUAL(Power::sqrt(Numeric::create(3, 2)), res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittableExpFracBaseFrac)
    /* (54/4375)^(1/3) = 3/5*(2/35)^(1/3), if the limit value for prime factorization is high
     * enough. */
{
    const BasePtr base = Numeric::create(54, 4375);
    BasePtr res;

    options::setMaxPrimeResolution(1000);
    res = Power::create(base, oneThird);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(oneThird, res->exp());
    BOOST_CHECK_EQUAL(base, res->base());

    options::setMaxPrimeResolution(10000);

    res = Power::create(base, oneThird);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(Numeric::create(3, 5), res->operands().front());
    BOOST_CHECK_EQUAL(oneThird, res->operands().back()->exp());
    BOOST_CHECK_EQUAL(Numeric::create(2, 35), res->operands().back()->base());
}

BOOST_AUTO_TEST_CASE(splittableBaseNumeratorExpFrac)
    /* (168080/17)^(2/5) = 49*(10/17)^(2/5), if the limit value for prime factorization is high
     * enough. */
{
    const BasePtr exp = Numeric::create(2, 5);
    const BasePtr origBase = Numeric::create(168070, 17);
    const BasePtr expectedPow = Power::create(Numeric::create(10, 17), exp);
    BasePtr res;

    options::setMaxPrimeResolution(100000);

    res = Power::create(origBase, exp);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(origBase, res->base());
    BOOST_CHECK_EQUAL(exp, res->exp());

    options::setMaxPrimeResolution(200000);

    res = Power::create(origBase, exp);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(Numeric::create(49), res->operands().front());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittalbeBaseDenomExpFrac)
    /* (21/10648)^(5/3) = 21/5153632*21^(2/3), if the limit value for prime factorization is high
     * enough. */
{
    const Int baseDenom(10648);
    const BasePtr origExp = Numeric::create(5, 3);
    const BasePtr origBase = Numeric::create(21, baseDenom);
    const BasePtr resultingExp = Numeric::create(2, 3);
    const BasePtr expectedPow = Power::create(Numeric::create(21), resultingExp);
    const BasePtr expectedNumeric = Numeric::create(21, 5153632);
    BasePtr res;

    options::setMaxPrimeResolution(baseDenom - 1);

    res = Power::create(origBase, origExp);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(origExp, res->exp());
    BOOST_CHECK_EQUAL(origBase, res->base());

    options::setMaxPrimeResolution(baseDenom);

    res = Power::create(origBase, origExp);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(expectedNumeric, res->operands().front());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(splittableNegExpFracBaseFrac)
    /* (8/81)^(-4/3) = 243/16*3^(1/3). */
{
    const BasePtr expectedNumeric = Numeric::create(243, 16);
    const BasePtr expectedPow = Power::create(three, oneThird);
    BasePtr res;

    res = Power::create(Numeric::create(8, 81), Numeric::create(-4, 3));

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(expectedNumeric, res->operands().front());
    BOOST_CHECK_EQUAL(expectedPow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(simpleUnresolvableExpFrac)
    /* No change of sqrt(2). */
{
    const BasePtr pow = Power::sqrt(two);

    BOOST_TEST(pow->isPower());
    BOOST_TEST(pow->isNumericPower());
    BOOST_CHECK_CLOSE(std::sqrt(2.0), pow->numericEval().toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(intBaseDoubleExp)
    /* 2^0.123456789 = 1.0893418703486832. */
{
    const BasePtr pow = Power::create(two, Numeric::create(0.123456789));
    const BasePtr expected = Numeric::create(1.0893418703486832);

    /* Uses the internal tolerance. */
    BOOST_CHECK_EQUAL(expected, pow);
    BOOST_CHECK_CLOSE(1.0893418703486832, pow->numericEval().toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(doubleBaseDoubleExpToInt)
    /* Double power that evalautes to exactly 2. */
{
    const BasePtr expected = two;
    const BasePtr pow = Power::create(Numeric::create(1.0893418703486832),
            Numeric::create(1.0/0.123456789));

    BOOST_CHECK_EQUAL(expected, pow);
}

BOOST_AUTO_TEST_CASE(unresolvableExpFrac)
    /* No change of 7^(1/3). */
{
    const BasePtr pow = Power::create(seven, oneThird);

    BOOST_TEST(pow->isPower());
    BOOST_TEST(pow->isNumericPower());

    BOOST_CHECK_CLOSE(std::pow(7.0, 1.0/3.0), pow->numericEval().toDouble(), TOL);
}

BOOST_AUTO_TEST_CASE(negFractionBaseWithNegFractionExp)
{
    const BasePtr res = Power::create(minusHalf, half);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(negFractionBaseWithPosFractionExp)
{
    const BasePtr res = Power::create(minusHalf, minusHalf);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(negNumericBasePosNumericExp)
{
    const BasePtr res = Power::create(minusHalf, Numeric::create(9.87654321));
    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(negNumericBaseNegNumericExp)
{
    const BasePtr res = Power::create(minusHalf, Numeric::create(-1.23456789));

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(unclearBaseExponentContractionBothFractionsOddDenom)
    /* (a^(2/3))^(4/5) = a^(8/15) for a neither positive nor negative. */
{
    const BasePtr expected = Power::create(a, Numeric::create(8, 15));
    const BasePtr res = Power::create(Power::create(a, Numeric::create(2, 3)),
            Numeric::create(4, 5));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseExponentContractionBothFractionsEvenDenom)
    /* (a^(1/8))^(3/10) = a^(3/80). */
{
    const BasePtr expected = Power::create(a, Numeric::create(3, 80));
    const BasePtr res = Power::create(Power::create(a, Numeric::create(1, 8)),
            Numeric::create(3, 10));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseExponentContractionBothFractions)
    /* (a^(1/2))^(1/3) = a^(1/6) for a neither positive nor negative. */
{
    const BasePtr expected = Power::create(a, Numeric::create(1, 6));
    const BasePtr res = Power::create(Power::create(a, half), oneThird);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseExponentContractionWithInteger)
    /* (a^(1/6))^3 = sqrt(a) for a neither positive nor negative. */
{
    const BasePtr res = Power::create(Power::create(a, Numeric::create(1, 6)), three);
    const BasePtr expected = Power::sqrt(a);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseExpContraction)
    /* (a^7)^(1/4) = a^(7/4). This works because a is assumed to be real, not possibly complex (this
     * is different in most CAS). */
{
    const BasePtr res = Power::create(Power::create(a, seven), Numeric::fourth());
    const BasePtr expected = Power::create(a, Numeric::create(7, 4));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(negBaseSymbolicExp)
    /* ((-a - b)^c)^2 isn't simplified. */
{
    const BasePtr base = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr res = Power::create(Power::create(base, c), two);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(two, res->exp());

    BOOST_TEST(res->base()->isPower());
    BOOST_CHECK_EQUAL(c, res->base()->exp());
    BOOST_CHECK_EQUAL(base, res->base()->base());
}

BOOST_AUTO_TEST_CASE(unclearProductBaseFractionExp)
    /* (a*b)^(2/3) can't be simplified, because a*b could be > 0 with a, b < 0. */
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtr res = Power::create(Product::create(a, b), exp);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(Product::create(a, b), res->base());
    BOOST_CHECK_EQUAL(exp, res->exp());
}

BOOST_AUTO_TEST_CASE(extractAllFactorsOfProductBase)
    /* (a*b*c*d*e*pi)^8 = a^8*b^8*c^8*d^8*e^8*pi^8. */
{
    const BasePtr base = Product::create({ a, b, c, d, e, pi });
    const BasePtr res = Power::create(base, eight);

    BOOST_TEST(res->isProduct());

    for (const auto& factor : res->operands()) {
        BOOST_TEST(factor->isPower());
        BOOST_CHECK_EQUAL(eight, factor->exp());
    }
}

BOOST_AUTO_TEST_CASE(extractPositiveFactorsOfProductBase)
    /* (a*b*c*d*e*pi)^(2/3) = a^(2/3)*b^(2/3)*pi^(2/3)*(c*d*e)^(2/3) for a, b, > 0. */
{
    const BasePtr exp = Numeric::create(2, 3);
    const BasePtrList expected{ Power::create(pi, exp), Power::create(aPos, exp),
        Power::create(bPos, exp), Power::create(Product::create(c, d, e), exp) };
    const BasePtr base = Product::create({ aPos, bPos, c, d, e, pi });
    BasePtr res = Power::create(base, exp);

    BOOST_TEST(res->isProduct());

    BOOST_TEST(bplist::areEqual(expected, res->operands()));
}

BOOST_AUTO_TEST_CASE(negProductBaseFractionExp)
    /* (-a*b)^(2/3) is undefined for a, b > 0. */
{
    const BasePtr base = Product::create(Numeric::mOne(), aPos, bPos);
    const BasePtr res = Power::create(base, Numeric::create(2, 3));

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(zeroExponent)
{
    const BasePtr res = Power::create(a, zero);

    BOOST_TEST(res->isNumeric());
    BOOST_CHECK_EQUAL(1, res->numericEval());
}

BOOST_AUTO_TEST_CASE(symbolicPowerBaseSymbolExp)
    /* (a^b)^c isn't simplified. */
{
    const BasePtr base = Power::create(a, b);
    const BasePtr res = Power::create(base, c);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(base, res->base());
    BOOST_CHECK_EQUAL(c, res->exp());
}

BOOST_AUTO_TEST_CASE(zeroBaseToPosExponent)
    /* 0^a = 0 with a > 0. */
{
    const BasePtr res = Power::create(zero, aPos);

    BOOST_TEST(res->isZero());
}

BOOST_AUTO_TEST_CASE(zeroBaseToUnclearExponent)
    /* 0^a = 0. */
{
    const BasePtr res = Power::create(zero, a);

    BOOST_TEST(res->isZero());
}

BOOST_AUTO_TEST_CASE(zeroBaseToNegExponent, noLogs())
    /* 0^(-a) with a > 0 is Undefined. */
{
    const BasePtr exp = Product::minus(aPos);
    BasePtr res;

    res = Power::create(zero, exp);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_CASE(identity)
{
    const BasePtr res = Power::create(a, one);
    const Name expected("a");

    BOOST_TEST(res->isSymbol());
    BOOST_CHECK_EQUAL(expected, res->name());
}

BOOST_AUTO_TEST_CASE(powerWithPowerExp)
{
    const BasePtr exp = Power::create(Sum::create(a, b), two);
    const BasePtr res = Power::create(a, exp);

    BOOST_TEST(res->isPower());
    BOOST_TEST(res->exp()->isPower());

    BOOST_CHECK_EQUAL(a, res->base());
    BOOST_CHECK_EQUAL(exp, res->exp());
}

BOOST_AUTO_TEST_CASE(multiplySymbolExp)
    /* (2^a)^a = 2^(a^2). */
{
    const BasePtr pow1 = Power::create(two, a);
    const BasePtr res = Power::create(pow1, a);

    BOOST_TEST(res->isPower());

    BOOST_TEST(res->base()->isNumeric());
    BOOST_TEST(res->exp()->isPower());

    BOOST_CHECK_EQUAL(2, res->base()->numericEval());
    BOOST_CHECK_EQUAL(2, res->exp()->exp()->numericEval());
}

BOOST_AUTO_TEST_CASE(multiplyExpWithZeroExp)
    /* (a^2)^0 = 1. */
{
    const BasePtr pow1 = Power::create(a, two);
    BasePtr res;

    res = Power::create(pow1, zero);

    BOOST_CHECK_EQUAL(one, res);
}

BOOST_AUTO_TEST_CASE(multiplyNumericExp)
    /* (2^(2/3))^(6/7) = 2^(4/7). */
{
    const BasePtr expected = Power::create(two, Numeric::create(4, 7));
    const BasePtr pow1 = Power::create(two, Numeric::create(2, 3));
    BasePtr res;

    res = Power::create(pow1, Numeric::create(6, 7));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(multiplyNumericExpToDouble)
    /* (sqrt(2))^1.23456789 shall be evaluated to a double numeric. */
{
    const double exp = 1.23456789;
    const BasePtr expected = Numeric::create(std::pow(std::pow(2, 0.5), exp));
    BasePtr res;

    res = Power::create(sqrtTwo, Numeric::create(exp));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(wrongDoubleEvaluationRequest)
{
    const BasePtr pow = Power::create(a, two);

    BOOST_CHECK_THROW(pow->numericEval(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(multiplyExponentsByCreation)
    /* ((a^2)^3)^1 = a^6. */
{
    const BasePtr pow1 = Power::create(a, two);
    const BasePtr pow2 = Power::create(pow1, three);
    const BasePtr res = Power::create(pow2, one);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(a, res->base());
    BOOST_CHECK_EQUAL(six, res->exp());
}

BOOST_AUTO_TEST_CASE(squareOfSymbolSquareRoot)
    /* (sqrt(a))^2 isn't simplified, if the sign of a is unknown. */
{
    const BasePtr res = Power::create(Power::sqrt(a), two);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(two, res->exp());
    BOOST_CHECK_EQUAL(Power::sqrt(a), res->base());
}

BOOST_AUTO_TEST_CASE(squareOfPosSymbolSquareRoot)
    /* (sqrt(a))^2 = a for positive a. */
{
    const BasePtr res = Power::create(Power::sqrt(aPos), two);

    BOOST_CHECK_EQUAL(aPos, res);
}

BOOST_AUTO_TEST_CASE(squareRootOfPosSymbolSquare)
    /* sqrt(a^2) = a for positive a. */
{
    const BasePtr res = Power::sqrt(Power::create(aPos, two));

    BOOST_CHECK_EQUAL(aPos, res);
}

BOOST_AUTO_TEST_CASE(powerOfNegSymbolSquare)
    /* ((-a - b)^2)^(-1/2) = (a + b)^(-1) for positive a, b. */
{
    const BasePtr base = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr res = Power::create(Power::create(base, two), minusHalf);

    BOOST_CHECK_EQUAL(Power::oneOver(Sum::create(aPos, bPos)), res);
}

BOOST_AUTO_TEST_CASE(powerOfNegSymbolSumChangesSign)
    /* ((-a - b)^2)^(1/6) = (a + b)^(1/3) for a, b, > 0. */
{
    const BasePtr base = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr res = Power::create(Power::create(base, two), Numeric::create(1, 6));
    const BasePtr expected = Power::create(Sum::create(aPos, bPos), oneThird);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(applyExponentToProduct)
    /* (a*b^2)^3 = (a^2)*(b^6) */
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr product = Product::create(a, bSquare);
    const BasePtr res = Power::create(product, three);
    auto it = cbegin(res->operands());
    BasePtr fac;

    BOOST_TEST(res->isProduct());

    fac = *it;
    BOOST_TEST(fac->isPower());
    BOOST_TEST(fac->base()->name() == Name("a"));
    BOOST_CHECK_EQUAL(three, fac->exp());

    ++it;
    fac = *it;
    BOOST_TEST(fac->isPower());
    BOOST_TEST(fac->base()->name() == Name("b"));
    BOOST_CHECK_EQUAL(six, fac->exp());
}

BOOST_AUTO_TEST_CASE(sumBase)
    /* No simplification of (a + b)^(a + b). */
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr res = Power::create(sum, sum);

    BOOST_CHECK_EQUAL(sum, res->exp());
    BOOST_CHECK_EQUAL(sum, res->base());
}

BOOST_AUTO_TEST_CASE(constantBase)
    /* No simplification of Pi^(a + b). */
{
    const BasePtr exp = Sum::create(a, b);
    const BasePtr res = Power::create(pi, exp);

    BOOST_CHECK_EQUAL(exp, res->exp());
    BOOST_CHECK_EQUAL(pi, res->base());
}

BOOST_AUTO_TEST_CASE(numericBaseConstantExp)
    /* No simplification of 2^Pi. */
{
    const BasePtr res = Power::create(two, pi);

    BOOST_CHECK_EQUAL(pi, res->exp());
    BOOST_CHECK_EQUAL(two, res->base());
}

BOOST_AUTO_TEST_CASE(productBaseConstantExp)
    /* (a*b)^Pi = a^Pi*b^Pi for a > 0, b unclear. */
{
    const BasePtr res = Power::create(Product::create(aPos, b), pi);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(Power::create(aPos, pi), res->operands().front());
    BOOST_CHECK_EQUAL(Power::create(b, pi), res->operands().back());
}

BOOST_AUTO_TEST_CASE(functionBase)
    /* No simplification of sin(a)^a. */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr res = Power::create(sin, a);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(sin, res->base());
    BOOST_CHECK_EQUAL(a, res->exp());
}

BOOST_AUTO_TEST_CASE(eulerBaseLogExp)
    /* e^log(a + b + sqrt(2)) = a + b + sqrt(2). */
{
    const BasePtr arg = Sum::create(a, b, Power::sqrt(two));
    const BasePtr result = Power::create(Constant::createE(), Logarithm::create(arg));

    BOOST_CHECK_EQUAL(arg, result);
}

BOOST_AUTO_TEST_CASE(negativePowByOddExp)
    /* (-aPos)^(3/5) is undefined. */
{
    const BasePtr pow = Power::create(Product::minus(aPos), Numeric::create(3, 5));

    BOOST_TEST(pow->isUndefined());
}

BOOST_AUTO_TEST_CASE(unclearSymbolBaseLeftUnchanged)
{
    const BasePtr base = Power::create(a, oneThird);
    const BasePtr res = Power::create(base, three);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(three, res->exp());
    BOOST_CHECK_EQUAL(base, res->base());
}

BOOST_AUTO_TEST_CASE(squareRootOfNegBaseSquared)
    /* ((-a - b)^2)^(-1/2) = (a + b)^(-1). */
{
    const BasePtr arg = Sum::create(Product::minus(aPos), Product::minus(bPos));
    const BasePtr base = Power::create(arg, two);
    const BasePtr res = Power::create(base, minusHalf);

    BOOST_CHECK_EQUAL(Power::oneOver(Sum::create(aPos, bPos)), res);
}

BOOST_AUTO_TEST_CASE(negativeBaseOfPowerToTheOneThird)
    /* ((-a - b)^2)^(1/3) = (a + b)^(2/3). */
{
    const BasePtr arg = Product::minus(Sum::create(aPos, bPos));
    const BasePtr res = Power::create(Power::create(arg, two), oneThird);
    const BasePtr expected = Power::create(Sum::create(aPos, bPos), Numeric::create(2, 3));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseDoubleExpContraction)
    /* The exponents of (a^(0.123456789))^1.23456789 are contracted. */
{
    const BasePtr e1 = Numeric::create(0.123456789);
    const BasePtr e2 = Numeric::create(1.23456789);
    const BasePtr res = Power::create(Power::create(a, e1), e2);
    const BasePtr expected = Power::create(a, Product::create(e1, e2));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseExpContractionWithConstantFirstExp)
    /* (a^pi)^3 = a^(3*pi). */
{
    const BasePtr res = Power::create(Power::create(a, pi), three);
    const BasePtr expected = Power::create(a, Product::create(three, pi));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseExpContractionWithConstantSecondExp)
    /* (a^3)^(-pi) = a^(-3*pi). */
{
    const BasePtr res = Power::create(Power::create(a, three), pi);
    const BasePtr expected = Power::create(a, Product::create(three, pi));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseExpContractionWithConstantFirstExpSecondEvenInt)
    /* ((-a)^pi)^(-8) = (-a)^(-8*pi). */
{
    const BasePtr base = Product::minus(a);
    const BasePtr res = Power::create(Power::create(base, pi), Numeric::create(-8));
    const BasePtr expected = Power::create(base, Product::minus(eight, pi));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(unclearBaseNoExpContractionWithConstantSecondExp)
    /* (a^8)^pi isn't simplified. */
{
    const BasePtr res = Power::create(Power::create(a, eight), pi);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(pi, res->exp());

    BOOST_TEST(res->base()->isPower());
    BOOST_CHECK_EQUAL(eight, res->base()->exp());
    BOOST_CHECK_EQUAL(a, res->base()->base());
}

BOOST_AUTO_TEST_CASE(unclearBaseNoExpContractionWithSymbolSecondExp)
    /* (a^(1/4))^b isn't simplified because a^(1/4) can be Undefined, while b = 4 is possible. */
{
    const BasePtr res = Power::create(Power::create(a, Numeric::fourth()), b);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(b, res->exp());

    BOOST_TEST(res->base()->isPower());
    BOOST_CHECK_EQUAL(Numeric::fourth(), res->base()->exp());
    BOOST_CHECK_EQUAL(a, res->base()->base());
}

BOOST_AUTO_TEST_CASE(numericallyEvaluableExpToUndefined)
    /* (-2)^pi is Undefined. */
{
    const BasePtr res = Power::create(Numeric::create(-2), pi);

    BOOST_TEST(res->isUndefined());
}

BOOST_AUTO_TEST_SUITE_END()
