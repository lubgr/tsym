
#include <unordered_map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "number.h"
#include "var.h"
#include "globals.h"
#include "tsymtests.h"
#include "baseptr.h"
#include "numeric.h"
#include "base.h"
#include "name.h"

using namespace tsym;

TEST_GROUP(Var)
{
    const Var zero = Var(0);
    const Var a = Var("a");
    const Var b = Var("b");
    const Var c = Var("c");
    const Var d = Var("d");
    const Var e = Var("e");
    const Var one = Var(1);
    const Var two = Var(2);
    const Var three = Var(3);
    const Var sqrtTwo = tsym::sqrt(2);
    const Var sqrtThree = tsym::sqrt(3);
    const Var sqrtSix = tsym::sqrt(6);
};

TEST(Var, undefinedType)
{
    Var u;

    disableLog();
    u = Var(1, 0);
    enableLog();

    CHECK_EQUAL(Var::Type::UNDEFINED, u.type());
}

TEST(Var, symbolType)
{
    CHECK_EQUAL(Var::Type::SYMBOL, a.type());
    CHECK_EQUAL("a", a.name());
}

TEST(Var, emptyStringCreation)
{
    disableLog();
    const Var undefined("");
    enableLog();

    CHECK_EQUAL(Var::Type::UNDEFINED, undefined.type());
}

TEST(Var, simpleSubcriptParsing)
{
    const Var var("a_b");
    const Name name(var.getBasePtr()->name());

    CHECK_EQUAL("a_b", var.name());
    CHECK_EQUAL("a", name.getName());
    CHECK_EQUAL("b", name.getSubscript());
    CHECK_EQUAL("", name.getSuperscript());
}

TEST(Var, subscriptParsing)
{
    const Var var("abc_{10}");
    const Name name(var.getBasePtr()->name());

    CHECK_EQUAL("abc", name.getName());
    CHECK_EQUAL("10", name.getSubscript());
    CHECK(name.getSuperscript().empty());
}

TEST(Var, smallIntParsing)
{
    const Var n("-12345");

    CHECK_EQUAL(Var(-12345), n);
    CHECK_EQUAL(Var::Type::INT, n.type());
}

TEST(Var, bigIntParsing)
{
    const char *intStr = "20394809284309283409820943820938409284309209438";
    const BasePtr numeric = Numeric::create(Int(intStr));
    const Var n(intStr);

    CHECK_EQUAL(Var::Type::INT, n.type());
    CHECK_FALSE(n.fitsIntoInt());
    CHECK_EQUAL(numeric, n.getBasePtr());
}

TEST(Var, posIntWithPosSign)
{
    const Var var("1234567", Var::Sign::POSITIVE);

    CHECK_EQUAL(Var(1234567), var);
}

TEST(Var, negIntWithPosSign)
{
    disableLog();
    const Var var("-1234567", Var::Sign::POSITIVE);
    enableLog();

    /* The inconsistent sign is ignored: */
    CHECK_EQUAL(Var(-1234567), var);
}

TEST(Var, parsingError)
{
    disableLog();
    const Var var("Pi*2");
    enableLog();

    CHECK_EQUAL(Var::Type::UNDEFINED, var.type());
}

TEST(Var, illegalCharacter)
{
    disableLog();
    const Var illegal("Ä");
    enableLog();

    CHECK_EQUAL(Var::Type::UNDEFINED, illegal.type());
    CHECK_EQUAL("", illegal.name());
}

TEST(Var, illegalSymbolName)
{
    disableLog();
    const Var illegal("12345.678");
    enableLog();

    CHECK_EQUAL(Var::Type::UNDEFINED, illegal.type());
    CHECK_EQUAL("", illegal.name());
}

TEST(Var, constructPositiveSymbol)
{
    const Var aPos("a", Var::Sign::POSITIVE);

    CHECK(aPos.isPositive());
    CHECK_FALSE(a.isPositive());
}

TEST(Var, numberTypes)
{
    CHECK_EQUAL(Var::Type::INT, one.type());
    CHECK_EQUAL(Var::Type::DOUBLE, Var(1.23456789).type());
    CHECK_EQUAL(Var::Type::FRACTION, Var(3, 5).type());
}

TEST(Var, numberRequest)
{
    CHECK_EQUAL(1, one.toInt());
}

TEST(Var, smallIntegerfitsIntoInt)
{
    CHECK(three.fitsIntoInt());
}

TEST(Var, nonIntegerfitsIntoInt)
{
    const Var fraction(2, 3);

    CHECK_FALSE(fraction.fitsIntoInt());
}

TEST(Var, doesntFitIntoInt)
{
    Var n(std::numeric_limits<int>::max());

    n *= 2;

    CHECK_FALSE(n.fitsIntoInt());
}

TEST(Var, toDouble)
{
    const Var frac(2, 3);

    DOUBLES_EQUAL(2.0/3.0, frac.toDouble(), 1.e-10);
}

TEST(Var, illegalNumberRequest)
    /* Requesting a number from a non-numeric expression should return zero. */
{
    disableLog();
    CHECK_EQUAL(0, a.toInt());
    enableLog();
}

TEST(Var, powerType)
{
    const Var res = a.toThe(2);

    CHECK_EQUAL(Var::Type::POWER, res.type());
}

TEST(Var, productType)
{
    const Var p(a*b*c);

    CHECK_EQUAL(Var::Type::PRODUCT, p.type());
}

TEST(Var, constantPi)
{
    const Var pi(Pi);

    CHECK_EQUAL(Var::Type::CONSTANT, pi.type());
    CHECK_EQUAL("pi", pi.name());
    CHECK(pi.operands().empty());
}

TEST(Var, functionType)
{
    Var res;

    res = sin(Var(1, 4));

    CHECK_EQUAL(Var::Type::FUNCTION, res.type());
    CHECK_EQUAL("sin", res.name());
}

TEST(Var, sumType)
{
    const Var s(a + b + d);

    CHECK_EQUAL(Var::Type::SUM, s.type());
}

TEST(Var, sumHasSymbol)
{
    const Var sum(a + b + c);

    CHECK(sum.has(a));
    CHECK_FALSE(sum.has(a + b));
}

TEST(Var, substSymbolInProduct)
{
    const Var p(2*a*b*c);
    Var result;

    result = p.subst(b, 4);

    CHECK_EQUAL(8*a*c, result);
}

TEST(Var, defaultAssignment)
{
    Var var;

    var = Var();

    CHECK(var.isZero());
}

TEST(Var, equalityOfSymbols)
{
    CHECK(a == a);
}

TEST(Var, emptyOperandsList)
{
    CHECK(a.operands().empty());
}

TEST(Var, symbolMinusSymbolToZero)
{
    CHECK_EQUAL(zero, a - a);
}

TEST(Var, symbolsMinusSymbolsToZero)
{
    CHECK_EQUAL(zero, a + b + c + d - (a + b + c + d));
}

TEST(Var, addSameSymbol)
{
    CHECK_EQUAL(8*a, 6*a + 2*a);
}

TEST(Var, addSameSymbolThreeTimes)
{
    CHECK_EQUAL(3*a, a + a + a);
}

TEST(Var, addDifferentSymbols)
{
    Var res;

    res = a + b;

    CHECK_EQUAL(Var::Type::SUM, res.type());
    CHECK_EQUAL(a, res.operands().front());
    CHECK_EQUAL(b, res.operands().back());
}

TEST(Var, addDifferentSymbolsOperator)
{
    Var res(a);

    res += b;

    CHECK_EQUAL(a + b, res);
}

TEST(Var, subtractDifferentSymbolsOperator)
{
    Var res(a);

    res -= b;

    CHECK_EQUAL(a - b, res);
}

TEST(Var, multiplyDifferentSymbolsOperator)
{
    Var res(a);

    res *= b;

    CHECK_EQUAL(a*b, res);
}

TEST(Var, addSymbolAndNumbers)
{
    CHECK_EQUAL(4 + a, 3 + a + 1);
}

TEST(Var, addSameSymbolAndNumbers)
{
    CHECK_EQUAL(14 + 3*a, a + 10 + 2*a + 4);
}

TEST(Var, addSameSymbolMultipledByFrac)
{
    CHECK_EQUAL(a, Var(1, 4)*a + Var(3, 4)*a);
}

TEST(Var, addSameSymbolMultipledByDoubleAndInt)
{
    CHECK_EQUAL(6.2*a, 3.5*a + 2.7*a);
}

TEST(Var, productOfNumbersAndDifferentSymbols)
{
    CHECK_EQUAL(1.0581942857142859*a*b, 1.23456*a*b*Var(6, 7));
}

TEST(Var, collectMultipleSymbolsInSum)
{
    CHECK_EQUAL(a + b + 2*c + 2*d, a + c + d + b + c + d);
}

TEST(Var, unaryPlusOperator)
{
    CHECK_EQUAL(a, +a);
}

TEST(Var, unaryMinusOperator)
{
    CHECK_EQUAL((-1)*a, -a);
}

TEST(Var, divisionToPower)
    /* (1/b)*a = a*b^(-1). */
{
    const Var expected(a*tsym::pow(b, -1));

    CHECK_EQUAL(expected, 1/b*a);
}

TEST(Var, divisionToFrac)
{
    CHECK_EQUAL(Var(1, 3)*a*b, a*b/3);
}

TEST(Var, productDividedByNumber)
{
    CHECK_EQUAL(3*b*c, 12*c*b/4);
}

TEST(Var, multiplicationOfExpPosSymbol)
{
    const Var aPos("a", Var::Sign::POSITIVE);
    const Var expected(tsym::pow(aPos, Var(10, 3)));
    Var res(aPos);

    res = res.toThe(2);
    res = res.toThe(5);
    res = res.toThe(Var(1, 3));

    CHECK_EQUAL(expected, res);
}

TEST(Var, noMultiplicationOfExpUnclearSymbol)
{
    Var res(tsym::pow(a, Var(1, 3)));

    res = res.toThe(3);

    CHECK_EQUAL(Var::Type::POWER, res.type());
}

TEST(Var, multiplicationOfExpUnclearSymbol)
{
    const Var expected(tsym::pow(a, Var(-2, 3)));
    Var res(1/a);

    res = res.toThe(Var(2, 3));

    CHECK_EQUAL(expected, res);
}

TEST(Var, expansionPowerOfProduct)
{
    const Var expected(a*a*b*b*c*c);
    Var res;

    res = a*b*c;
    res = tsym::pow(res, 2);

    CHECK_EQUAL(expected, res);
}

TEST(Var, divisionByItself)
{
    Var res(a*b*c*d);

    res /= res;

    CHECK_EQUAL(1, res);
}

TEST(Var, fracOfSymbolsMultipliedByInverse)
{
    CHECK_EQUAL(1, (a/b)*(b/a));
}

TEST(Var, expAdditionEqualBase)
{
    const Var expected(tsym::pow(a*b, 23));
    Var pow1(a*b);
    Var pow2(b*a);

    pow1 = pow1.toThe(15);
    pow2 = pow2.toThe(8);

    CHECK_EQUAL(expected, pow1*pow2);
}

TEST(Var, powerWithZeroBase)
{
    zero.toThe(2);

    CHECK(zero.isZero());
}

TEST(Var, powerWithZeroBaseNegExp)
{
    disableLog();
    const Var res = zero.toThe(-2);
    enableLog();

    CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

TEST(Var, powerWithBaseOne)
{
    one.toThe(123);

    CHECK_EQUAL(1, one);
}

TEST(Var, powerWithZeroExp)
{
    Var res(a);

    res = res.toThe(0);

    CHECK_EQUAL(1, res);
}

TEST(Var, powerWithExpOne)
{
    Var res(a);

    res.toThe(1);

    CHECK_EQUAL(a, res);
}

TEST(Var, productWithZero)
{
    CHECK_EQUAL(0, a*b*d*0);
}

TEST(Var, productOfNumberAndSum)
{
    /* Is expanded because the factor 2 is of numeric type. */
    CHECK_EQUAL(2*a + 2*b, 2*(a + b));
}

TEST(Var, productOfSymbolAndSum)
    /* No automatic expansion of a*(b + c). */
{
    Var res;

    res = a*(b + c);

    /* Is not expanded into a sum. */
    CHECK_EQUAL(Var::Type::PRODUCT, res.type());
}

TEST(Var, productOfConstantSumAndSum)
    /* A sum of constants is treated as a constant when multiplied with a sum:
     * (2 + sqrt(2))*(a + b) = (2 + sqrt(2))*a + (2 + sqrt(2))*b. */
{
    Var res;

    res = (two + sqrtTwo)*(a + b);

    CHECK_EQUAL(Var::Type::SUM, res.type());
    CHECK_EQUAL((two + sqrtTwo)*a + (two + sqrtTwo)*b, res);
}

TEST(Var, productOfConstAndConstSum)
    /* sqrt(2)*(2 + sqrt(3)) = 2*sqrt(2) + sqrt(6). */
{
    CHECK_EQUAL(2*sqrtTwo + sqrtSix, sqrtTwo*(2 + sqrtThree));
}

TEST(Var, productOfConstPowerAndConstSum)
    /* (1 + sqrt(2))*sqrt(3) = sqrt(3) + sqrt(6). */
{
    Var res;

    res = (1 + sqrtTwo)*sqrtThree;

    CHECK_EQUAL(sqrtThree + sqrtSix, res);
}

TEST(Var, productOfNumberAndConstSum)
    /* 3*(4 + sqrt(2)) = 12 + 4*sqrt(2). */
{
    Var res;

    res = 3*(4 + sqrtTwo);

    CHECK_EQUAL(12 + 3*sqrtTwo, res);
}

TEST(Var, productOfNumberNumPowAndSum)
    /* 2*sqrt(2)*(a + b) = 2*sqrt(2)*a + 2*sqrt(2)*b. */
{
    Var res;

    res = 2*sqrtTwo*(a + b);

    CHECK_EQUAL(2*sqrtTwo*a + 2*sqrtTwo*b, res);
}

TEST(Var, productOfTwoConstantSums)
    /* Products of constant sums are expanded. (2 + sqrt(2))*(3 + sqrt(3)) = 6 + 3*sqrt(2) +
     * 2*sqrt(3) + sqrt(6). */
{
    Var res;

    res = (two + sqrtTwo)*(three + sqrtThree);

    CHECK_EQUAL(Var::Type::SUM, res.type());
    CHECK_EQUAL(6 + 3*sqrtTwo + 2*sqrtThree + sqrtSix, res);
}

TEST(Var, minusOneSquare)
    /* (-1)^2 = 1. */
{
    const Var res = tsym::pow(Var(-1), 2);

    CHECK_EQUAL(1, res);
}

TEST(Var, minusOneCubic)
    /* (-1)^3 = -1. */
{
    const Var res = tsym::pow(Var(-1), 3);

    CHECK_EQUAL(-1, res);
}

TEST(Var, numPowerToUndefined)
    /* (-1)^(1/3) is undefined. */
{
    const Var res = tsym::pow(Var(-1), Var(1, 3));

    CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

TEST(Var, numPowerToUndefinedEvenDenomExp)
    /* (-1)^(5/4) is undefined. */
{
    const Var res = tsym::pow(Var(-1), Var(4, 5));

    CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

TEST(Var, numPowerToUndefinedFractionBase)
    /* (-1/2)^(1/2) is undefined. */
{
    const Var res = tsym::sqrt(Var(-1, 2));

    CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

TEST(Var, simpleNumericPowerSimplification)
    /* 2/3*sqrt(3) = 2*3^(-1/2). */
{
    const Var expected(2*tsym::pow(3, Var(-1, 2)));
    Var twoThird(2, 3);

    CHECK_EQUAL(expected, twoThird*sqrtThree);
}

TEST(Var, numericPowerSimplification)
    /* (9/11)^(-12/23)*2^(-12/23)*(1/7)^(12/23) = (126/11)^(-12/23). */
{
    const Var exp(12, 23);
    const Var expected(tsym::pow(Var(126, 11), -exp));
    Var res;

    res = tsym::pow(Var(9, 11), -exp)*tsym::pow(2, -exp)*tsym::pow(Var(1, 7), exp);

    CHECK_EQUAL(expected, res);
}

TEST(Var, simpleNumPowExtraction)
    /* 3/2*(1/3)^(1/3) = 1/2*3^(2/3). */
{
    const Var expected(Var(1, 2)*tsym::pow(3, Var(2, 3)));
    Var res;

    res = Var(3, 2)*tsym::pow(Var(1, 3), Var(1, 3));

    CHECK_EQUAL(expected, res);
}

TEST(Var, orderingOfProductOfConstants)
    /* 3*sqrt(3)*10*sqrt(2)*3*sqrt(2) = 180*sqrt(3). */
{
    Var res;

    res = 3*sqrtThree*10*sqrtTwo*3*sqrtTwo;

    CHECK_EQUAL(180*sqrtThree, res);
}

TEST(Var, constProductsEqualBaseAfterExtraction)
    /* 17^(2/3)*sqrt(833) = 7*17^(1/6). */
{
    const Var expected(119*tsym::pow(17, Var(1, 6)));
    Var res;

    res = tsym::pow(17, Var(2, 3))*tsym::sqrt(833);

    CHECK_EQUAL(expected, res);
}

TEST(Var, orderingOfLargeProductOfConstants)
    /* 5*(2/9)*sqrt(3)*2^(1/5)*sqrt(17)*(10/11)*sqrt(2)*sqrt(7)*4^(1/5)*17^(2/3)*(1/4)*sqrt(7) =
     * 25/33*2^(3/5)*17^(2/3)*sqrt(1666/3). */
{
    const Var expected(Var(25, 33)*tsym::pow(2, Var(3, 5))*tsym::pow(17, Var(2, 3))
        *tsym::sqrt(Var(1666, 3)));
    Var res;

    res = 5*Var(2, 9)*tsym::sqrt(3)*tsym::pow(2, Var(1, 5))*tsym::sqrt(17)*Var(10, 11)
        *tsym::sqrt(2)*tsym::sqrt(7)*tsym::pow(4, Var(1, 5))*tsym::pow(17, Var(2, 3))*Var(1, 4)
        *tsym::sqrt(7);

    CHECK_EQUAL(expected, res);
}

TEST(Var, constPowerfracExpGreaterThanOne)
    /* 2^(3/2) = 2*sqrt(2). */
{
    Var res;

    res = tsym::pow(2, Var(3, 2));

    CHECK_EQUAL(Var::Type::PRODUCT, res.type());
    CHECK_EQUAL(2*sqrtTwo, res);
}

TEST(Var, piInSum)
    /* 2 + Pi + 3*sqrt(5)*Pi + 5 + Pi. */
{
    Var res;

    res = 2 + Pi + 3*tsym::sqrt(5)*Pi + 5 + Pi;

    CHECK_EQUAL(7 + 2*Pi + 3*tsym::sqrt(5)*Pi, res);
}

TEST(Var, simpleSumWithEqualNonConstTerms)
    /* 2*a + sqrt(2)*a stays as it is. */
{
    Var res;

    res = two*a + sqrtTwo*a;

    CHECK_EQUAL(Var::Type::SUM, res.type());
    CHECK_EQUAL((two + sqrtTwo)*a, res);
}

TEST(Var, collectConstTerms)
    /* 2*a + sqrt(2)*a + sqrt(3)*a + 5*a = (7 + sqrt(2)*a + sqrt(3))*a. */
{
    Var res;

    res = 2*a + sqrtTwo*a + sqrtThree*a + 5*a;

    CHECK_EQUAL((7 + sqrtTwo + sqrtThree)*a, res);
}

TEST(Var, collectConstTermsWithSum)
    /* (1 + sqrt(2))*a + sqrt(3)*a = (1 + sqrt(2) + sqrt(3))*a. */
{
    Var res;

    res = (1 + sqrtTwo)*a + sqrtThree*a;

    CHECK_EQUAL(Var::Type::SUM, res.type());
    CHECK_EQUAL(a + sqrtTwo*a + sqrtThree*a, res);
}

TEST(Var, expandProductOfConstTerms)
    /* No collection of (1 + 2*sqrt(2)*sqrt(3))*a + (2 + sqrt(2)*sqrt(3))*a. */
{
    Var res;

    res = (1 + 2*sqrtTwo*sqrtThree)*a + (2 + sqrtTwo*sqrtThree)*a;

    CHECK_EQUAL(Var::Type::SUM, res.type());
    CHECK_EQUAL(3*a + 3*tsym::sqrt(6)*a, res);
}

TEST(Var, simpleExpansion)
    /* a*(b + c)*d = a*b*d + a*c*d. */
{
    const Var expected = a*b*d + a*c*d;
    const Var orig = a*(b + c)*d;
    Var result;

    result = orig.expand();

    CHECK_EQUAL(expected, result);
}

TEST(Var, dontCollectProductOfConstTerms)
    /* 2*sqrt(3)*sqrt(2) + 3*sqrt(3)*sqrt(5) = 2*sqrt(6) + 3*sqrt(15). */
{
    const Var sqrtFive(tsym::sqrt(5));
    const Var sqrtFifteen(tsym::sqrt(15));
    Var res;

    res = 2*sqrtThree*sqrtTwo + 3*sqrtThree*sqrtFive;

    CHECK_EQUAL(2*sqrtSix + 3*sqrtFifteen, res);
}

TEST(Var, largerSumWithEqualNonConstTerms)
    /* (2 + sqrt(2))*(a + b + c) + (1 + sqrt(3))*(a + b + c) = const*a + const*b + const*c, where
     * const = 3 + sqrt(2) + sqrt(3). */
{
    const Var constVar(3 + sqrtTwo + sqrtThree);
    Var res;

    res = (two + sqrtTwo)*(a + b + c) + (one + sqrtThree)*(a + b + c);

    CHECK_EQUAL(Var::Type::SUM, res.type())
    CHECK_EQUAL(constVar*a + constVar*b + constVar*c, res);
}

TEST(Var, sumWithNumericAndNumPowProducts)
    /* 2*sqrt(2)*a - 3/5*sqrt(2)*a = 7/5*sqrt(2)*a. */
{
    const Var expected(Var(7, 5)*sqrtTwo*a);
    Var res;

    res = 2*sqrtTwo*a - Var(3, 5)*sqrtTwo*a;

    CHECK_EQUAL(expected, res);
}

TEST(Var, expandNumericFactorResultingInZero)
    /* a + b - (a + b) = 0. */
{
    Var res(a + b);

    res -= (a + b);

    CHECK_EQUAL(zero, res);
}

TEST(Var, simpleExpandNumPowFactorResultingInZero)
    /* sqrt(2)*a - sqrt(2)*a = 0. */
{
    const Var res(sqrtTwo*a - sqrtTwo*a);

    CHECK_EQUAL(zero, res);
}

TEST(Var, expandNumPowFactorResultingInZero)
    /* (sqrt(2)*a - sqrt(3)*b) - (-sqrt(3)*b + sqrt(2)*a) = 0. */
{
    Var res;

    res = (sqrtTwo*a - sqrtThree*b) - (-sqrtThree*b + sqrtTwo*a);

    CHECK_EQUAL(zero, res);
}

TEST(Var, expandPowerFactorResultingInZero)
    /* d*a^(b + c) - d*a^(b + c) = 0. */
{
    Var res(d*tsym::pow(a, b + c));

    res -= d*tsym::pow(a, b + c);

    CHECK_EQUAL(zero, res);
}

TEST(Var, sumOfProduts)
    /* 2*(a + b)*(c + d) - (a + b)*(c + d) = (a + b)*(c + d). */
{
    const Var ex((a + b)*(c + d));
    Var res;

    res = 2*ex - ex;

    CHECK_EQUAL(ex, res);
}

TEST(Var, largeMixedTerm01)
    /* a + 2*sqrt(2)*b + 3*c - 7*a*sqrt(2)*sqrt(3)*5^(1/3) + 4*d^(2*b + 2*a) - b*2/sqrt(2) -
     * b*sqrt(2) = (1 - 7*5^(1/3)*sqrt(6))*a + 3*c + 4*d^(2*a + 2*b). */
{
    const Var expected((1 - 7*tsym::pow(5, Var(1, 3))*tsym::sqrt(6))*a + 3*c + 4*tsym::pow(d, 2*a +
    2*b));
    Var res;

    res = a + 2*tsym::sqrt(2)*b + 3*c - 7*a*tsym::sqrt(2)*tsym::sqrt(3)*tsym::pow(5, Var(1, 3)) + 4
    *tsym::pow(d, 2*b + 2*a) -
        b*2/tsym::sqrt(2) - b*tsym::sqrt(2);

    CHECK_EQUAL(expected, res);
}

TEST(Var, largeMixedTerm02)
    /* 2^(1/3)*(a + b)*2^(1/4)*sqrt(2) + (b + a)*2^(1/12) + (a + b)*(d + c) = 3*2^(1/12)*a +
     * 3*2^(1/12)*b + (a + b)*(d + c). */
{
    const Var fac(3*tsym::pow(2, Var(1, 12)));
    const Var expected(fac*a + fac*b + (a + b)*(c + d));
    Var res;

    res = tsym::pow(2, Var(1, 3))*(a + b)*tsym::pow(2, Var(1, 4))*tsym::sqrt(2) + (b + a)*tsym::pow
    (2, Var(1, 12))
        + (a + b)*(d + c);

    CHECK_EQUAL(expected, res);
}

TEST(Var, largeMixedTerm03)
    /* (c*a*b*(d + e)^(d + e))/(a*e*d*(e + d))*14/15 = 14/15*(b*c)/(d*e)*(d + e)^(-1 + d + e). */
{
    const Var expected(Var(14, 15)*b*c*tsym::pow(d, -1)*tsym::pow(e, -1)
            *tsym::pow(d + e, -1 + d + e));
    Var res;

    res = (c*a*b*tsym::pow(d + e, d + e))/(a*e*d*(e + d))*Var(14, 15);

    CHECK_EQUAL(expected, res);
}

TEST(Var, largeMixedTerm04)
{
    const Var expected((10 + sqrtTwo)*a + 100*tsym::pow(a, 2) + (3 + sqrtTwo)*b + 2*c + Var(13, 9)*
        d);
    Var res;

    res = (a + 2*b + 2*c) + (9*a + b + d) + tsym::sqrt(2)*(a + b) + 4*d/9 + 100*a*a;

    CHECK_EQUAL(expected, res);
}

TEST(Var, largeMixedTerm05)
    /* a^(2/3)*e*sqrt(2)*sqrt(3)*b*2*d*e*(e + 2)*a*c*sqrt(3)*a^(1/5)*sqrt(5)*sqrt(3) =
     * 6*sqrt(30)*a^(28/15)*b*c*d*e^2*(2 + e). */
{
    const Var expected(6*tsym::sqrt(30)*tsym::pow(a, Var(28, 15))*b*c*d*tsym::pow(e, 2)*(2 + e));
    Var res;

    res = tsym::pow(a, Var(2, 3))*e*tsym::sqrt(2)*tsym::sqrt(3)*b*2*d*e*(e + 2)*a*c*tsym::sqrt(3)*
    tsym::pow(a, Var(1, 5))*tsym::sqrt(5)*tsym::sqrt(3);

    CHECK_EQUAL(expected, res);
    CHECK_EQUAL(8, res.operands().size());
}

TEST(Var, acosOfCosOfThreePiFourth)
{
    const Var res = acos(cos(7*Pi/4));
    const Var expected = Pi/4;

    CHECK_EQUAL(expected, res);
}

TEST(Var, atan2OfResolvableArgs)
{
    const Var res(atan2(Var(-123, 28), Var(-123, 28)));

    CHECK_EQUAL(225*Pi/180, res);
}

TEST(Var, atan2OfNonResolvableNumericallyEvaluableArgs)
{
    const Var res(atan2(sqrtSix, sqrtThree));

    CHECK_EQUAL(Var::Type::FUNCTION, res.type());
    CHECK_EQUAL("atan", res.name());
    CHECK_EQUAL(1, res.operands().size());
    CHECK_EQUAL(sqrtTwo, res.operands().front());
}

TEST(Var, diffOfSum)
{
    const Var sum(2*tsym::pow(a, 3) + a*b);
    const Var result(sum.diff(a));
    const Var expected(6*a*a + b);

    CHECK_EQUAL(expected, result);
}

TEST(Var, normalToZero)
{
    const Var sum = a*b + a*c - a*(b + c);

    CHECK(sum.expand().isZero());
    CHECK(sum.normal().isZero());
}

TEST(Var, getNumAndDenomFromFraction)
{
    const Var frac(2, 3);

    CHECK_EQUAL(2, frac.numerator().toInt());
    CHECK_EQUAL(3, frac.denominator().toInt());
}

TEST(Var, getNumAndDenomFromProduct)
{
    const Var res(a*b*b/(c*c));

    CHECK_EQUAL(a*b*b, res.numerator());
    CHECK_EQUAL(c*c, res.denominator());
}

TEST(Var, negativeVar)
{
    const Var aPos("a", Var::Sign::POSITIVE);
    const Var bPos("b", Var::Sign::POSITIVE);
    Var res;

    res = -aPos*bPos + 2 - 3*Pi;

    CHECK_FALSE(res.isPositive());
    CHECK(res.isNegative());
}

TEST(Var, comparisonPosAndNonPosSymbols)
{
    const Var aPos("a_3", Var::Sign::POSITIVE);
    const Var aNonPos("a_3");

    CHECK(aPos != aNonPos);
}

TEST(Var, collectSymbolsFromSymbol)
{
    CHECK_EQUAL(1, a.collectSymbols().size());
    CHECK_EQUAL(a, a.collectSymbols()[0]);
}

TEST(Var, collectSymbolsFromNumber)
{
    const Var n(2, 3);

    CHECK(n.collectSymbols().empty());
}

TEST(Var, collectSymbolsFromMixedTerm)
{
    const Var term(a*b + b/3 + 2*c*d*d + c*c*c - tsym::log(e + 12*tsym::Pi));
    const std::vector<Var> result(term.collectSymbols());

    CHECK_EQUAL(5, result.size());
    CHECK_EQUAL(b, result[0]);
    CHECK_EQUAL(a, result[1]);
    CHECK_EQUAL(c, result[2]);
    CHECK_EQUAL(d, result[3]);
    CHECK_EQUAL(e, result[4]);
}

TEST(Var, collectSymbolsFromPower)
{
    const Var term(tsym::pow(a + b, a*d*(2 + tsym::Pi*e*tsym::Euler)));
    const std::vector<Var> result(term.collectSymbols());

    CHECK_EQUAL(4, result.size());
    CHECK(std::find(result.begin(), result.end(), a) != result.end());
    CHECK(std::find(result.begin(), result.end(), b) != result.end());
    CHECK(std::find(result.begin(), result.end(), d) != result.end());
    CHECK(std::find(result.begin(), result.end(), e) != result.end());
}

TEST(Var, collectSymbolsFromFunction)
{
    const Var term(tsym::asin(a) + tsym::log(tsym::Pi*b) - tsym::tan(c));
    const std::vector<Var> result(term.collectSymbols());

    CHECK_EQUAL(3, result.size());
    CHECK(std::find(result.begin(), result.end(), a) != result.end());
    CHECK(std::find(result.begin(), result.end(), b) != result.end());
    CHECK(std::find(result.begin(), result.end(), c) != result.end());
}

TEST(Var, printerOperator)
{
    const std::string expected("a");
    std::stringstream stream;

    stream << a;

    CHECK_EQUAL(expected, stream.str());
}

TEST(Var, printerOperatorTypeEnumSumProductPower)
{
    const std::string expected("SumProductPower");
    std::stringstream stream;

    stream << (a + b).type() << (a*b).type() << tsym::pow(a, b).type();

    CHECK_EQUAL(expected, stream.str());
}

TEST(Var, printerOperatorTypeEnumNumeric)
{
    const std::string expected("IntegerDoubleFraction");
    std::stringstream stream;

    stream << one.type() << Var(1.23456789).type() << Var(1, 2).type();

    CHECK_EQUAL(expected, stream.str());
}

TEST(Var, printerOperatorTypeEnumConstantFunctionSymbol)
{
    const std::string expected("ConstantFunctionSymbol");
    std::stringstream stream;

    stream << Pi.type() << cos(a).type() << a.type();

    CHECK_EQUAL(expected, stream.str());
}

TEST(Var, printerOperatorTypeEnumUndefined)
{
    const std::string expected("Undefined");
    std::stringstream stream;
    disableLog();
    const Var u(1, 0);
    enableLog();

    stream << u;

    CHECK_EQUAL(expected, stream.str());
}

TEST(Var, unorderedMapInsertion)
{
    std::unordered_map<tsym::Var, int> map;

    map[a] = 0;
    map[b] = 1;
    map[c + d] = 2;

    CHECK_EQUAL(0, map[a]);
    CHECK_EQUAL(1, map[b]);
    CHECK_EQUAL(2, map[c + d]);
}

TEST(Var, equalHashes)
{
    CHECK_EQUAL(std::hash<Var>{}(a + b + c), std::hash<Var>{}(a + b + c));
}

TEST(Var, differentHashes)
{
    CHECK_FALSE(std::hash<Var>{}(a) == std::hash<Var>{}(2*a));
}
