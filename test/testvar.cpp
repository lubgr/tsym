
#include <algorithm>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include "base.h"
#include "baseptr.h"
#include "fixtures.h"
#include "globals.h"
#include "name.h"
#include "number.h"
#include "numeric.h"
#include "tsymtests.h"
#include "var.h"

using namespace tsym;

struct VarFixture {
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

BOOST_FIXTURE_TEST_SUITE(TestVar, VarFixture)

BOOST_AUTO_TEST_CASE(undefinedType, noLogs())
{
    Var u = 1 / a;

    u = subst(u, a, 0);

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, u.type());
}

BOOST_AUTO_TEST_CASE(symbolType)
{
    BOOST_CHECK_EQUAL(Var::Type::SYMBOL, a.type());
    BOOST_CHECK_EQUAL("a", name(a));
}

BOOST_AUTO_TEST_CASE(emptyStringCreation, noLogs())
{
    const Var undefined("");

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, undefined.type());
}

BOOST_AUTO_TEST_CASE(simpleSubcriptParsing)
{
    const Var var("a_b");
    const Name fullName(var.get()->name());

    BOOST_CHECK_EQUAL("a_b", name(var));
    BOOST_CHECK_EQUAL("a", fullName.getName());
    BOOST_CHECK_EQUAL("b", fullName.getSubscript());
    BOOST_CHECK_EQUAL("", fullName.getSuperscript());
}

BOOST_AUTO_TEST_CASE(subscriptParsing)
{
    const Var var("abc_{10}");
    const Name name(var.get()->name());

    BOOST_CHECK_EQUAL("abc", name.getName());
    BOOST_CHECK_EQUAL("10", name.getSubscript());
    BOOST_TEST(name.getSuperscript().empty());
}

BOOST_AUTO_TEST_CASE(smallIntParsing)
{
    const Var n("-12345");

    BOOST_CHECK_EQUAL(Var(-12345), n);
    BOOST_CHECK_EQUAL(Var::Type::INT, n.type());
}

BOOST_AUTO_TEST_CASE(bigIntParsing)
{
    const char* intStr = "20394809284309283409820943820938409284309209438";
    const BasePtr numeric = Numeric::create(Int(intStr));
    const Var n(intStr);

    BOOST_CHECK_EQUAL(Var::Type::INT, n.type());
    BOOST_CHECK_EQUAL(numeric, n.get());
}

BOOST_AUTO_TEST_CASE(posIntWithPosSign)
{
    const Var var("1234567", Var::Sign::POSITIVE);

    BOOST_CHECK_EQUAL(Var(1234567), var);
}

BOOST_AUTO_TEST_CASE(negIntWithPosSign, noLogs())
{
    const Var var("-1234567", Var::Sign::POSITIVE);

    /* The inconsistent sign is ignored: */
    BOOST_CHECK_EQUAL(Var(-1234567), var);
}

BOOST_AUTO_TEST_CASE(parsingError, noLogs())
{
    const Var var("Pi*2");

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, var.type());
}

BOOST_AUTO_TEST_CASE(illegalCharacter, noLogs())
{
    const Var illegal("Ä");

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, illegal.type());
    BOOST_CHECK_EQUAL("", name(illegal));
}

BOOST_AUTO_TEST_CASE(illegalSymbolName, noLogs())
{
    const Var illegal("12345.678");

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, illegal.type());
    BOOST_CHECK_EQUAL("", name(illegal));
}

BOOST_AUTO_TEST_CASE(constructPositiveSymbol)
{
    const Var aPos("a", Var::Sign::POSITIVE);

    BOOST_TEST(isPositive(aPos));
    BOOST_TEST(!isPositive(a));
}

BOOST_AUTO_TEST_CASE(numberTypes)
{
    BOOST_CHECK_EQUAL(Var::Type::INT, one.type());
    BOOST_CHECK_EQUAL(Var::Type::DOUBLE, Var(1.23456789).type());
    BOOST_CHECK_EQUAL(Var::Type::FRACTION, Var(3, 5).type());
}

BOOST_AUTO_TEST_CASE(numberRequest)
{
    BOOST_CHECK_EQUAL(1, static_cast<int>(one));
}

BOOST_AUTO_TEST_CASE(failingFractionToIntegerCast)
{
    const Var fraction(2, 3);

    BOOST_CHECK_THROW((void) static_cast<int>(fraction), std::domain_error);
}

BOOST_AUTO_TEST_CASE(failingCastOfTwiceMaxIntToPrimitiveInt)
{
    Var n(std::numeric_limits<int>::max());

    n *= 2;

    BOOST_CHECK_THROW((void) static_cast<int>(n), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(failingCastOfBigIntToPrimitiveInt)
{
    BOOST_CHECK_THROW(
      (void) static_cast<int>(Var("2384729384609865192859238659823659287589273985723029348")), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(illegalNumberRequestSymbol)
{
    BOOST_CHECK_THROW((void) static_cast<int>(a), std::domain_error);
}

BOOST_AUTO_TEST_CASE(toDouble)
{
    const Var frac(2, 3);

    BOOST_CHECK_CLOSE(2.0 / 3.0, static_cast<double>(frac), 1.e-10);
}

BOOST_AUTO_TEST_CASE(toDoubleNonNumeric)
{
    BOOST_CHECK_THROW((void) static_cast<double>(a), std::domain_error);
}

BOOST_AUTO_TEST_CASE(powerType)
{
    const Var res = pow(a, 2);

    BOOST_CHECK_EQUAL(Var::Type::POWER, res.type());
}

BOOST_AUTO_TEST_CASE(productType)
{
    const Var p(a * b * c);

    BOOST_CHECK_EQUAL(Var::Type::PRODUCT, p.type());
}

BOOST_AUTO_TEST_CASE(constantPi)
{
    BOOST_CHECK_EQUAL(Var::Type::CONSTANT, pi().type());
    BOOST_CHECK_EQUAL("pi", name(pi()));
    BOOST_TEST(operands(pi()).empty());
}

BOOST_AUTO_TEST_CASE(functionType)
{
    Var res;

    res = sin(Var(1, 4));

    BOOST_CHECK_EQUAL(Var::Type::FUNCTION, res.type());
    BOOST_CHECK_EQUAL("sin", name(res));
}

BOOST_AUTO_TEST_CASE(sumType)
{
    const Var s(a + b + d);

    BOOST_CHECK_EQUAL(Var::Type::SUM, s.type());
}

BOOST_AUTO_TEST_CASE(sumHasSymbol)
{
    const Var sum(a + b + c);

    BOOST_TEST(has(sum, a));
    BOOST_TEST(!has(sum, a + b));
}

BOOST_AUTO_TEST_CASE(substSymbolInProduct)
{
    const Var p(2 * a * b * c);
    Var result;

    result = subst(p, b, 4);

    BOOST_CHECK_EQUAL(8 * a * c, result);
}

BOOST_AUTO_TEST_CASE(substituteInSimpleSum)
{
    const Var orig = a / b + 1 / (5 * b);
    const Var expected = 3 * a + Var(3, 5);

    BOOST_CHECK_EQUAL(expected, subst(orig, b, Var(1, 3)));
}

BOOST_AUTO_TEST_CASE(defaultAssignment)
{
    Var var;

    var = Var();

    BOOST_CHECK_EQUAL(0, var);
}

BOOST_AUTO_TEST_CASE(equalityOfSymbols)
{
    BOOST_TEST(a == a);
}

BOOST_AUTO_TEST_CASE(emptyOperandsList)
{
    BOOST_TEST(operands(a).empty());
}

BOOST_AUTO_TEST_CASE(symbolMinusSymbolToZero)
{
    BOOST_CHECK_EQUAL(zero, a - a);
}

BOOST_AUTO_TEST_CASE(symbolsMinusSymbolsToZero)
{
    BOOST_CHECK_EQUAL(zero, a + b + c + d - (a + b + c + d));
}

BOOST_AUTO_TEST_CASE(addSameSymbol)
{
    BOOST_CHECK_EQUAL(8 * a, 6 * a + 2 * a);
}

BOOST_AUTO_TEST_CASE(addSameSymbolThreeTimes)
{
    BOOST_CHECK_EQUAL(3 * a, a + a + a);
}

BOOST_AUTO_TEST_CASE(addDifferentSymbols)
{
    Var res;

    res = a + b;

    BOOST_CHECK_EQUAL(Var::Type::SUM, res.type());
    BOOST_CHECK_EQUAL(a, operands(res).front());
    BOOST_CHECK_EQUAL(b, operands(res).back());
}

BOOST_AUTO_TEST_CASE(addDifferentSymbolsOperator)
{
    Var res(a);

    res += b;

    BOOST_CHECK_EQUAL(a + b, res);
}

BOOST_AUTO_TEST_CASE(subtractDifferentSymbolsOperator)
{
    Var res(a);

    res -= b;

    BOOST_CHECK_EQUAL(a - b, res);
}

BOOST_AUTO_TEST_CASE(multiplyDifferentSymbolsOperator)
{
    Var res(a);

    res *= b;

    BOOST_CHECK_EQUAL(a * b, res);
}

BOOST_AUTO_TEST_CASE(addSymbolAndNumbers)
{
    BOOST_CHECK_EQUAL(4 + a, 3 + a + 1);
}

BOOST_AUTO_TEST_CASE(addSameSymbolAndNumbers)
{
    BOOST_CHECK_EQUAL(14 + 3 * a, a + 10 + 2 * a + 4);
}

BOOST_AUTO_TEST_CASE(addSameSymbolMultipledByFrac)
{
    BOOST_CHECK_EQUAL(a, Var(1, 4) * a + Var(3, 4) * a);
}

BOOST_AUTO_TEST_CASE(addSameSymbolMultipledByDoubleAndInt)
{
    BOOST_CHECK_EQUAL(6.2 * a, 3.5 * a + 2.7 * a);
}

BOOST_AUTO_TEST_CASE(productOfNumbersAndDifferentSymbols)
{
    BOOST_CHECK_EQUAL(1.0581942857142859 * a * b, 1.23456 * a * b * Var(6, 7));
}

BOOST_AUTO_TEST_CASE(collectMultipleSymbolsInSum)
{
    BOOST_CHECK_EQUAL(a + b + 2 * c + 2 * d, a + c + d + b + c + d);
}

BOOST_AUTO_TEST_CASE(unaryPlusOperator)
{
    BOOST_CHECK_EQUAL(a, +a);
}

BOOST_AUTO_TEST_CASE(unaryMinusOperator)
{
    BOOST_CHECK_EQUAL((-1) * a, -a);
}

BOOST_AUTO_TEST_CASE(divisionToPower)
/* (1/b)*a = a*b^(-1). */
{
    const Var expected(a * tsym::pow(b, -1));

    BOOST_CHECK_EQUAL(expected, 1 / b * a);
}

BOOST_AUTO_TEST_CASE(divisionToFrac)
{
    BOOST_CHECK_EQUAL(Var(1, 3) * a * b, a * b / 3);
}

BOOST_AUTO_TEST_CASE(productDividedByNumber)
{
    BOOST_CHECK_EQUAL(3 * b * c, 12 * c * b / 4);
}

BOOST_AUTO_TEST_CASE(multiplicationOfExpPosSymbol)
{
    const Var aPos("a", Var::Sign::POSITIVE);
    const Var expected(tsym::pow(aPos, Var(10, 3)));
    Var res(aPos);

    res = pow(res, 2);
    res = pow(res, 5);
    res = pow(res, Var(1, 3));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(noMultiplicationOfExpUnclearSymbol)
{
    Var res(tsym::pow(a, Var(1, 3)));

    res = pow(res, 3);

    BOOST_CHECK_EQUAL(Var::Type::POWER, res.type());
}

BOOST_AUTO_TEST_CASE(multiplicationOfExpUnclearSymbol)
{
    const Var expected(tsym::pow(a, Var(-2, 3)));
    Var res(1 / a);

    res = pow(res, Var(2, 3));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(expansionPowerOfProduct)
{
    const Var expected(a * a * b * b * c * c);
    Var res;

    res = a * b * c;
    res = tsym::pow(res, 2);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(divisionByItself)
{
    Var res(a * b * c * d);

    res /= res;

    BOOST_CHECK_EQUAL(1, res);
}

BOOST_AUTO_TEST_CASE(fracOfSymbolsMultipliedByInverse)
{
    BOOST_CHECK_EQUAL(1, (a / b) * (b / a));
}

BOOST_AUTO_TEST_CASE(expAdditionEqualBase)
{
    const Var expected(tsym::pow(a * b, 23));
    Var pow1(a * b);
    Var pow2(b * a);

    pow1 = pow(pow1, 15);
    pow2 = pow(pow2, 8);

    BOOST_CHECK_EQUAL(expected, pow1 * pow2);
}

BOOST_AUTO_TEST_CASE(powerWithZeroBase)
{
    const Var result = pow(zero, 2);

    BOOST_CHECK_EQUAL(0, result);
}

BOOST_AUTO_TEST_CASE(powerWithZeroBaseNegExp, noLogs())
{
    const Var res = pow(zero, -2);

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

BOOST_AUTO_TEST_CASE(powerWithBaseOne)
{
    const Var res = pow(one, 123);

    BOOST_CHECK_EQUAL(res, one);
}

BOOST_AUTO_TEST_CASE(powerWithZeroExp)
{
    const Var res = pow(a, 0);

    BOOST_CHECK_EQUAL(1, res);
}

BOOST_AUTO_TEST_CASE(powerWithExpOne)
{
    const Var res = pow(a, 1);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(productWithZero)
{
    BOOST_CHECK_EQUAL(0, a * b * d * 0);
}

BOOST_AUTO_TEST_CASE(productOfNumberAndSum)
{
    /* Is expanded because the factor 2 is of numeric type. */
    BOOST_CHECK_EQUAL(2 * a + 2 * b, 2 * (a + b));
}

BOOST_AUTO_TEST_CASE(productOfSymbolAndSum)
/* No automatic expansion of a*(b + c). */
{
    Var res;

    res = a * (b + c);

    /* Is not expanded into a sum. */
    BOOST_CHECK_EQUAL(Var::Type::PRODUCT, res.type());
}

BOOST_AUTO_TEST_CASE(productOfConstantSumAndSum)
/* A sum of constants is treated as a constant when multiplied with a sum:
 * (2 + sqrt(2))*(a + b) = (2 + sqrt(2))*a + (2 + sqrt(2))*b. */
{
    Var res;

    res = (two + sqrtTwo) * (a + b);

    BOOST_CHECK_EQUAL(Var::Type::SUM, res.type());
    BOOST_CHECK_EQUAL((two + sqrtTwo) * a + (two + sqrtTwo) * b, res);
}

BOOST_AUTO_TEST_CASE(productOfConstAndConstSum)
/* sqrt(2)*(2 + sqrt(3)) = 2*sqrt(2) + sqrt(6). */
{
    BOOST_CHECK_EQUAL(2 * sqrtTwo + sqrtSix, sqrtTwo * (2 + sqrtThree));
}

BOOST_AUTO_TEST_CASE(productOfConstPowerAndConstSum)
/* (1 + sqrt(2))*sqrt(3) = sqrt(3) + sqrt(6). */
{
    Var res;

    res = (1 + sqrtTwo) * sqrtThree;

    BOOST_CHECK_EQUAL(sqrtThree + sqrtSix, res);
}

BOOST_AUTO_TEST_CASE(productOfNumberAndConstSum)
/* 3*(4 + sqrt(2)) = 12 + 4*sqrt(2). */
{
    Var res;

    res = 3 * (4 + sqrtTwo);

    BOOST_CHECK_EQUAL(12 + 3 * sqrtTwo, res);
}

BOOST_AUTO_TEST_CASE(productOfNumberNumPowAndSum)
/* 2*sqrt(2)*(a + b) = 2*sqrt(2)*a + 2*sqrt(2)*b. */
{
    Var res;

    res = 2 * sqrtTwo * (a + b);

    BOOST_CHECK_EQUAL(2 * sqrtTwo * a + 2 * sqrtTwo * b, res);
}

BOOST_AUTO_TEST_CASE(productOfTwoConstantSums)
/* Products of constant sums are expanded. (2 + sqrt(2))*(3 + sqrt(3)) = 6 + 3*sqrt(2) +
 * 2*sqrt(3) + sqrt(6). */
{
    Var res;

    res = (two + sqrtTwo) * (three + sqrtThree);

    BOOST_CHECK_EQUAL(Var::Type::SUM, res.type());
    BOOST_CHECK_EQUAL(6 + 3 * sqrtTwo + 2 * sqrtThree + sqrtSix, res);
}

BOOST_AUTO_TEST_CASE(minusOneSquare)
/* (-1)^2 = 1. */
{
    const Var res = tsym::pow(Var(-1), 2);

    BOOST_CHECK_EQUAL(1, res);
}

BOOST_AUTO_TEST_CASE(minusOneCubic)
/* (-1)^3 = -1. */
{
    const Var res = tsym::pow(Var(-1), 3);

    BOOST_CHECK_EQUAL(-1, res);
}

BOOST_AUTO_TEST_CASE(numPowerToUndefined)
/* (-1)^(1/3) is undefined. */
{
    const Var res = tsym::pow(Var(-1), Var(1, 3));

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

BOOST_AUTO_TEST_CASE(numPowerToUndefinedEvenDenomExp)
/* (-1)^(5/4) is undefined. */
{
    const Var res = tsym::pow(Var(-1), Var(4, 5));

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

BOOST_AUTO_TEST_CASE(numPowerToUndefinedFractionBase)
/* (-1/2)^(1/2) is undefined. */
{
    const Var res = tsym::sqrt(Var(-1, 2));

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, res.type());
}

BOOST_AUTO_TEST_CASE(simpleNumericPowerSimplification)
/* 2/3*sqrt(3) = 2*3^(-1/2). */
{
    const Var expected(2 * tsym::pow(3, Var(-1, 2)));
    Var twoThird(2, 3);

    BOOST_CHECK_EQUAL(expected, twoThird * sqrtThree);
}

BOOST_AUTO_TEST_CASE(numericPowerSimplification)
/* (9/11)^(-12/23)*2^(-12/23)*(1/7)^(12/23) = (126/11)^(-12/23). */
{
    const Var exp(12, 23);
    const Var expected(tsym::pow(Var(126, 11), -exp));
    Var res;

    res = tsym::pow(Var(9, 11), -exp) * tsym::pow(2, -exp) * tsym::pow(Var(1, 7), exp);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(simpleNumPowExtraction)
/* 3/2*(1/3)^(1/3) = 1/2*3^(2/3). */
{
    const Var expected(Var(1, 2) * tsym::pow(3, Var(2, 3)));
    Var res;

    res = Var(3, 2) * tsym::pow(Var(1, 3), Var(1, 3));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(orderingOfProductOfConstants)
/* 3*sqrt(3)*10*sqrt(2)*3*sqrt(2) = 180*sqrt(3). */
{
    Var res;

    res = 3 * sqrtThree * 10 * sqrtTwo * 3 * sqrtTwo;

    BOOST_CHECK_EQUAL(180 * sqrtThree, res);
}

BOOST_AUTO_TEST_CASE(constProductsEqualBaseAfterExtraction)
/* 17^(2/3)*sqrt(833) = 7*17^(1/6). */
{
    const Var expected(119 * tsym::pow(17, Var(1, 6)));
    Var res;

    res = tsym::pow(17, Var(2, 3)) * tsym::sqrt(833);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(orderingOfLargeProductOfConstants)
/* 5*(2/9)*sqrt(3)*2^(1/5)*sqrt(17)*(10/11)*sqrt(2)*sqrt(7)*4^(1/5)*17^(2/3)*(1/4)*sqrt(7) =
 * 25/33*2^(3/5)*17^(2/3)*sqrt(1666/3). */
{
    const Var expected(Var(25, 33) * tsym::pow(2, Var(3, 5)) * tsym::pow(17, Var(2, 3)) * tsym::sqrt(Var(1666, 3)));
    Var res;

    res = 5 * Var(2, 9) * tsym::sqrt(3) * tsym::pow(2, Var(1, 5)) * tsym::sqrt(17) * Var(10, 11) * tsym::sqrt(2)
      * tsym::sqrt(7) * tsym::pow(4, Var(1, 5)) * tsym::pow(17, Var(2, 3)) * Var(1, 4) * tsym::sqrt(7);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(constPowerfracExpGreaterThanOne)
/* 2^(3/2) = 2*sqrt(2). */
{
    Var res;

    res = tsym::pow(2, Var(3, 2));

    BOOST_CHECK_EQUAL(Var::Type::PRODUCT, res.type());
    BOOST_CHECK_EQUAL(2 * sqrtTwo, res);
}

BOOST_AUTO_TEST_CASE(piInSum)
/* 2 + Pi + 3*sqrt(5)*Pi + 5 + Pi. */
{
    Var res;

    res = 2 + pi() + 3 * tsym::sqrt(5) * pi() + 5 + pi();

    BOOST_CHECK_EQUAL(7 + 2 * pi() + 3 * tsym::sqrt(5) * pi(), res);
}

BOOST_AUTO_TEST_CASE(simpleSumWithEqualNonConstTerms)
/* 2*a + sqrt(2)*a stays as it is. */
{
    Var res;

    res = two * a + sqrtTwo * a;

    BOOST_CHECK_EQUAL(Var::Type::SUM, res.type());
    BOOST_CHECK_EQUAL((two + sqrtTwo) * a, res);
}

BOOST_AUTO_TEST_CASE(collectConstTerms)
/* 2*a + sqrt(2)*a + sqrt(3)*a + 5*a = (7 + sqrt(2)*a + sqrt(3))*a. */
{
    Var res;

    res = 2 * a + sqrtTwo * a + sqrtThree * a + 5 * a;

    BOOST_CHECK_EQUAL((7 + sqrtTwo + sqrtThree) * a, res);
}

BOOST_AUTO_TEST_CASE(collectConstTermsWithSum)
/* (1 + sqrt(2))*a + sqrt(3)*a = (1 + sqrt(2) + sqrt(3))*a. */
{
    Var res;

    res = (1 + sqrtTwo) * a + sqrtThree * a;

    BOOST_CHECK_EQUAL(Var::Type::SUM, res.type());
    BOOST_CHECK_EQUAL(a + sqrtTwo * a + sqrtThree * a, res);
}

BOOST_AUTO_TEST_CASE(expandProductOfConstTerms)
/* No collection of (1 + 2*sqrt(2)*sqrt(3))*a + (2 + sqrt(2)*sqrt(3))*a. */
{
    Var res;

    res = (1 + 2 * sqrtTwo * sqrtThree) * a + (2 + sqrtTwo * sqrtThree) * a;

    BOOST_CHECK_EQUAL(Var::Type::SUM, res.type());
    BOOST_CHECK_EQUAL(3 * a + 3 * tsym::sqrt(6) * a, res);
}

BOOST_AUTO_TEST_CASE(simpleExpansion)
/* a*(b + c)*d = a*b*d + a*c*d. */
{
    const Var expected = a * b * d + a * c * d;
    const Var orig = a * (b + c) * d;
    Var result;

    result = expand(orig);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(dontCollectProductOfConstTerms)
/* 2*sqrt(3)*sqrt(2) + 3*sqrt(3)*sqrt(5) = 2*sqrt(6) + 3*sqrt(15). */
{
    const Var sqrtFive(tsym::sqrt(5));
    const Var sqrtFifteen(tsym::sqrt(15));
    Var res;

    res = 2 * sqrtThree * sqrtTwo + 3 * sqrtThree * sqrtFive;

    BOOST_CHECK_EQUAL(2 * sqrtSix + 3 * sqrtFifteen, res);
}

BOOST_AUTO_TEST_CASE(largerSumWithEqualNonConstTerms)
/* (2 + sqrt(2))*(a + b + c) + (1 + sqrt(3))*(a + b + c) = const*a + const*b + const*c, where
 * const = 3 + sqrt(2) + sqrt(3). */
{
    const Var constVar(3 + sqrtTwo + sqrtThree);
    Var res;

    res = (two + sqrtTwo) * (a + b + c) + (one + sqrtThree) * (a + b + c);

    BOOST_CHECK_EQUAL(Var::Type::SUM, res.type());
    BOOST_CHECK_EQUAL(constVar * a + constVar * b + constVar * c, res);
}

BOOST_AUTO_TEST_CASE(sumWithNumericAndNumPowProducts)
/* 2*sqrt(2)*a - 3/5*sqrt(2)*a = 7/5*sqrt(2)*a. */
{
    const Var expected(Var(7, 5) * sqrtTwo * a);
    Var res;

    res = 2 * sqrtTwo * a - Var(3, 5) * sqrtTwo * a;

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(expandNumericFactorResultingInZero)
/* a + b - (a + b) = 0. */
{
    Var res(a + b);

    res -= (a + b);

    BOOST_CHECK_EQUAL(zero, res);
}

BOOST_AUTO_TEST_CASE(simpleExpandNumPowFactorResultingInZero)
/* sqrt(2)*a - sqrt(2)*a = 0. */
{
    const Var res(sqrtTwo * a - sqrtTwo * a);

    BOOST_CHECK_EQUAL(zero, res);
}

BOOST_AUTO_TEST_CASE(expandNumPowFactorResultingInZero)
/* (sqrt(2)*a - sqrt(3)*b) - (-sqrt(3)*b + sqrt(2)*a) = 0. */
{
    Var res;

    res = (sqrtTwo * a - sqrtThree * b) - (-sqrtThree * b + sqrtTwo * a);

    BOOST_CHECK_EQUAL(zero, res);
}

BOOST_AUTO_TEST_CASE(expandPowerFactorResultingInZero)
/* d*a^(b + c) - d*a^(b + c) = 0. */
{
    Var res(d * tsym::pow(a, b + c));

    res -= d * tsym::pow(a, b + c);

    BOOST_CHECK_EQUAL(zero, res);
}

BOOST_AUTO_TEST_CASE(sumOfProduts)
/* 2*(a + b)*(c + d) - (a + b)*(c + d) = (a + b)*(c + d). */
{
    const Var ex((a + b) * (c + d));
    Var res;

    res = 2 * ex - ex;

    BOOST_CHECK_EQUAL(ex, res);
}

BOOST_AUTO_TEST_CASE(largeMixedTerm01)
/* a + 2*sqrt(2)*b + 3*c - 7*a*sqrt(2)*sqrt(3)*5^(1/3) + 4*d^(2*b + 2*a) - b*2/sqrt(2) -
 * b*sqrt(2) = (1 - 7*5^(1/3)*sqrt(6))*a + 3*c + 4*d^(2*a + 2*b). */
{
    const Var expected((1 - 7 * tsym::pow(5, Var(1, 3)) * tsym::sqrt(6)) * a + 3 * c + 4 * tsym::pow(d, 2 * a + 2 * b));
    Var res;

    res = a + 2 * tsym::sqrt(2) * b + 3 * c - 7 * a * tsym::sqrt(2) * tsym::sqrt(3) * tsym::pow(5, Var(1, 3))
      + 4 * tsym::pow(d, 2 * b + 2 * a) - b * 2 / tsym::sqrt(2) - b * tsym::sqrt(2);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(largeMixedTerm02)
/* 2^(1/3)*(a + b)*2^(1/4)*sqrt(2) + (b + a)*2^(1/12) + (a + b)*(d + c) = 3*2^(1/12)*a +
 * 3*2^(1/12)*b + (a + b)*(d + c). */
{
    const Var fac(3 * tsym::pow(2, Var(1, 12)));
    const Var expected(fac * a + fac * b + (a + b) * (c + d));
    Var res;

    res = tsym::pow(2, Var(1, 3)) * (a + b) * tsym::pow(2, Var(1, 4)) * tsym::sqrt(2)
      + (b + a) * tsym::pow(2, Var(1, 12)) + (a + b) * (d + c);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(largeMixedTerm03)
/* (c*a*b*(d + e)^(d + e))/(a*e*d*(e + d))*14/15 = 14/15*(b*c)/(d*e)*(d + e)^(-1 + d + e). */
{
    const Var expected(Var(14, 15) * b * c * tsym::pow(d, -1) * tsym::pow(e, -1) * tsym::pow(d + e, -1 + d + e));
    Var res;

    res = (c * a * b * tsym::pow(d + e, d + e)) / (a * e * d * (e + d)) * Var(14, 15);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(largeMixedTerm04)
{
    const Var expected((10 + sqrtTwo) * a + 100 * tsym::pow(a, 2) + (3 + sqrtTwo) * b + 2 * c + Var(13, 9) * d);
    Var res;

    res = (a + 2 * b + 2 * c) + (9 * a + b + d) + tsym::sqrt(2) * (a + b) + 4 * d / 9 + 100 * a * a;

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(largeMixedTerm05)
/* a^(2/3)*e*sqrt(2)*sqrt(3)*b*2*d*e*(e + 2)*a*c*sqrt(3)*a^(1/5)*sqrt(5)*sqrt(3) =
 * 6*sqrt(30)*a^(28/15)*b*c*d*e^2*(2 + e). */
{
    const Var expected(6 * tsym::sqrt(30) * tsym::pow(a, Var(28, 15)) * b * c * d * tsym::pow(e, 2) * (2 + e));
    Var res;

    res = tsym::pow(a, Var(2, 3)) * e * tsym::sqrt(2) * tsym::sqrt(3) * b * 2 * d * e * (e + 2) * a * c * tsym::sqrt(3)
      * tsym::pow(a, Var(1, 5)) * tsym::sqrt(5) * tsym::sqrt(3);

    BOOST_CHECK_EQUAL(expected, res);
    BOOST_CHECK_EQUAL(8, operands(res).size());
}

BOOST_AUTO_TEST_CASE(acosOfCosOfThreePiFourth)
{
    const Var res = acos(cos(7 * pi() / 4));
    const Var expected = pi() / 4;

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(atan2OfResolvableArgs)
{
    const Var res(atan2(Var(-123, 28), Var(-123, 28)));

    BOOST_CHECK_EQUAL(225 * pi() / 180, res);
}

BOOST_AUTO_TEST_CASE(atan2OfNonResolvableNumericallyEvaluableArgs)
{
    const Var res(atan2(sqrtSix, sqrtThree));

    BOOST_CHECK_EQUAL(Var::Type::FUNCTION, res.type());
    BOOST_CHECK_EQUAL("atan", name(res));
    BOOST_CHECK_EQUAL(1, operands(res).size());
    BOOST_CHECK_EQUAL(sqrtTwo, operands(res).front());
}

BOOST_AUTO_TEST_CASE(diffOfSum)
{
    const Var sum(2 * tsym::pow(a, 3) + a * b);
    const Var result = diff(sum, a);
    const Var expected(6 * a * a + b);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(simplifyToZero)
{
    const Var sum = a * b + a * c - a * (b + c);

    BOOST_CHECK_EQUAL(0, expand(sum));
    BOOST_CHECK_EQUAL(0, simplify(sum));
}

BOOST_AUTO_TEST_CASE(simplifyExpandLessComplex)
{
    const Var orig = b - 3 * c - a * c - (-3 - a) * c;
    const Var simplified = simplify(orig);

    BOOST_CHECK_EQUAL(b, simplified);
    BOOST_TEST(complexity(simplified) < complexity(numerator(orig)));
}

BOOST_AUTO_TEST_CASE(simplifyWithUndefinedIntermediateResult, noLogs())
{
    const Var denom = a - a * b / (b + c) - a * c / (b + c);
    const Var orig = d / denom;

    const Var result = simplify(orig);

    BOOST_CHECK_EQUAL(Var::Type::UNDEFINED, result.type());
}

BOOST_AUTO_TEST_CASE(getNumAndDenomFromFraction)
{
    const Var frac(2, 3);

    BOOST_CHECK_EQUAL(2, static_cast<int>(numerator(frac)));
    BOOST_CHECK_EQUAL(3, static_cast<int>(denominator(frac)));
}

BOOST_AUTO_TEST_CASE(getNumAndDenomFromProduct)
{
    const Var res(a * b * b / (c * c));

    BOOST_CHECK_EQUAL(a * b * b, numerator(res));
    BOOST_CHECK_EQUAL(c * c, denominator(res));
}

BOOST_AUTO_TEST_CASE(negativeVar)
{
    const Var aPos("a", Var::Sign::POSITIVE);
    const Var bPos("b", Var::Sign::POSITIVE);
    Var res;

    res = -aPos * bPos + 2 - 3 * pi();

    BOOST_TEST(!isPositive(res));
    BOOST_TEST(isNegative(res));
}

BOOST_AUTO_TEST_CASE(comparisonPosAndNonPosSymbols)
{
    const Var aPos("a_3", Var::Sign::POSITIVE);
    const Var aNonPos("a_3");

    BOOST_TEST(aPos != aNonPos);
}

BOOST_AUTO_TEST_CASE(collectSymbolsFromSymbol)
{
    BOOST_CHECK_EQUAL(1, collectSymbols(a).size());
    BOOST_CHECK_EQUAL(a, collectSymbols(a)[0]);
}

BOOST_AUTO_TEST_CASE(collectSymbolsFromNumber)
{
    const Var n(2, 3);

    BOOST_TEST(collectSymbols(n).empty());
}

BOOST_AUTO_TEST_CASE(collectSymbolsFromMixedTerm)
{
    const Var term(a * b + b / 3 + 2 * c * d * d + c * c * c - tsym::log(e + 12 * tsym::pi()));
    const std::vector<Var> result(collectSymbols(term));

    BOOST_CHECK_EQUAL(5, result.size());
    BOOST_CHECK_EQUAL(b, result[0]);
    BOOST_CHECK_EQUAL(a, result[1]);
    BOOST_CHECK_EQUAL(c, result[2]);
    BOOST_CHECK_EQUAL(d, result[3]);
    BOOST_CHECK_EQUAL(e, result[4]);
}

BOOST_AUTO_TEST_CASE(collectSymbolsFromPower)
{
    const Var term(tsym::pow(a + b, a * d * (2 + tsym::pi() * e * tsym::euler())));
    const std::vector<Var> result(collectSymbols(term));

    BOOST_CHECK_EQUAL(4, result.size());
    BOOST_TEST((std::find(cbegin(result), cend(result), a) != cend(result)));
    BOOST_TEST((std::find(cbegin(result), cend(result), b) != cend(result)));
    BOOST_TEST((std::find(cbegin(result), cend(result), d) != cend(result)));
    BOOST_TEST((std::find(cbegin(result), cend(result), e) != cend(result)));
}

BOOST_AUTO_TEST_CASE(collectSymbolsFromFunction)
{
    const Var term(tsym::asin(a) + tsym::log(tsym::pi() * b) - tsym::tan(c));
    const std::vector<Var> result(collectSymbols(term));

    BOOST_CHECK_EQUAL(3, result.size());
    BOOST_TEST((std::find(cbegin(result), cend(result), a) != cend(result)));
    BOOST_TEST((std::find(cbegin(result), cend(result), b) != cend(result)));
    BOOST_TEST((std::find(cbegin(result), cend(result), c) != cend(result)));
}

BOOST_AUTO_TEST_CASE(printerOperator)
{
    const std::string expected("a");
    std::stringstream stream;

    stream << a;

    BOOST_CHECK_EQUAL(expected, stream.str());
}

BOOST_AUTO_TEST_CASE(printerOperatorTypeEnumSumProductPower)
{
    const std::string expected("SumProductPower");
    std::stringstream stream;

    stream << (a + b).type() << (a * b).type() << tsym::pow(a, b).type();

    BOOST_CHECK_EQUAL(expected, stream.str());
}

BOOST_AUTO_TEST_CASE(printerOperatorTypeEnumNumeric)
{
    const std::string expected("IntegerDoubleFraction");
    std::stringstream stream;

    stream << one.type() << Var(1.23456789).type() << Var(1, 2).type();

    BOOST_CHECK_EQUAL(expected, stream.str());
}

BOOST_AUTO_TEST_CASE(printerOperatorTypeEnumConstantFunctionSymbol)
{
    const std::string expected("ConstantFunctionSymbol");
    std::stringstream stream;

    stream << pi().type() << cos(a).type() << a.type();

    BOOST_CHECK_EQUAL(expected, stream.str());
}

BOOST_AUTO_TEST_CASE(printerOperatorTypeEnumUndefined, noLogs())
{
    const std::string expected("Undefined");
    std::stringstream stream;

    const Var u = tsym::tan(pi() / 2);

    stream << u;

    BOOST_CHECK_EQUAL(expected, stream.str());
}

BOOST_AUTO_TEST_CASE(unorderedMapInsertion)
{
    std::unordered_map<tsym::Var, int> map;

    map[a] = 0;
    map[b] = 1;
    map[c + d] = 2;

    BOOST_CHECK_EQUAL(0, map[a]);
    BOOST_CHECK_EQUAL(1, map[b]);
    BOOST_CHECK_EQUAL(2, map[c + d]);
}

BOOST_AUTO_TEST_CASE(equalHashes)
{
    BOOST_CHECK_EQUAL(std::hash<Var>{}(a + b + c), std::hash<Var>{}(a + b + c));
}

BOOST_AUTO_TEST_CASE(differentHashes)
{
    BOOST_TEST(std::hash<Var>{}(a) != std::hash<Var>{}(2 * a));
}

BOOST_AUTO_TEST_SUITE_END()
