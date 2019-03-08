
#include <cmath>
#include <limits>
#include "basefct.h"
#include "constant.h"
#include "fixtures.h"
#include "logarithm.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct SumFixture : public AbcFixture {
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr cosA = Trigonometric::createCos(a);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
};

BOOST_FIXTURE_TEST_SUITE(TestSum, SumFixture)

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Sum");
    const BasePtr ptr = Sum::create(a, b);

    BOOST_CHECK_EQUAL(expected, ptr->typeStr());
}

BOOST_AUTO_TEST_CASE(undefinedToUndefined)
{
    const BasePtr p = Sum::create(undefined, a);

    BOOST_TEST(isUndefined(*p));
}

BOOST_AUTO_TEST_CASE(createWithOneSummandGivenAsList)
{
    BasePtrList summands;
    BasePtr res;

    summands.push_back(a);

    res = Sum::create(summands);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(twoNumericSummands)
/* 2 + 3 = 5. */
{
    const BasePtr sum = Sum::create(two, three);

    BOOST_TEST(isNumeric(*sum));
    BOOST_CHECK_EQUAL(5, sum->numericEval().value());
}

BOOST_AUTO_TEST_CASE(twoNumericSummandsResultingInZero)
/* 2 - 2 = 0. */
{
    const BasePtr minusTwo = Numeric::create(-2);
    const BasePtr res = Sum::create(two, minusTwo);

    BOOST_TEST(isZero(*res));
}

BOOST_AUTO_TEST_CASE(zeroPlusZero)
{
    const BasePtr res = Sum::create(zero, zero);

    BOOST_TEST(isZero(*res));
}

BOOST_AUTO_TEST_CASE(symbolPlusZero)
/* a + 0 = a. */
{
    const BasePtr res = Sum::create(a, zero);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(zeroPlusSymbol)
/* 0 + a = a. */
{
    const BasePtr res = Sum::create(zero, a);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(rearrangeTwoSymbols)
/* Check simple ordering: b + a = a + b. */
{
    const BasePtr res = Sum::create(b, a);

    BOOST_TEST(isSum(*res));
    BOOST_CHECK_EQUAL(a, res->operands().front());
    BOOST_CHECK_EQUAL(b, res->operands().back());
}

BOOST_AUTO_TEST_CASE(rearrangeFiveSymbols)
/*  More complex ordering: e + c + a + b + d = a + b + c + d + e. */
{
    const BasePtr res = Sum::create({e, c, a, b, d});
    const BasePtrList expected{a, b, c, d, e};
    const BasePtrList& summands = res->operands();

    BOOST_TEST(isSum(*res));

    BOOST_TEST(expected == summands, per_element());
}

BOOST_AUTO_TEST_CASE(orderingOfMixedTerms)
/* a - sqrt(2)*a is equal to -a*sqrt(2) + a. */
{
    const BasePtr result1 = Sum::create(a, Product::minus(sqrtTwo, a));
    const BasePtr result2 = Sum::create(Product::minus(sqrtTwo, a), a);

    BOOST_CHECK_EQUAL(result1, result2);
}

BOOST_AUTO_TEST_CASE(collectSymbols)
/* a + a = 2*a. */
{
    const BasePtr res = Sum::create(a, a);
    const BasePtrList& summands(res->operands());

    BOOST_TEST(isProduct(*res));

    BOOST_CHECK_EQUAL(2, summands.size());

    BOOST_CHECK_EQUAL(two, summands.front());
    BOOST_CHECK_EQUAL(a, summands.back());
}

BOOST_AUTO_TEST_CASE(collectProducts)
/* 2*a*b + 3*a*b = 5*a*b. */
{
    const BasePtr ab = Product::create(a, b);
    const BasePtr twoAB = Product::create(two, ab);
    const BasePtr threeAB = Product::create(three, ab);
    const BasePtr res = Sum::create(twoAB, threeAB);
    const BasePtrList expected{five, a, b};

    BOOST_TEST(isProduct(*res));

    BOOST_TEST(expected == res->operands(), per_element());
}

BOOST_AUTO_TEST_CASE(collectProductOfSymbols)
/* 2*a*b + a*b = 3*a*b. */
{
    const BasePtr ab = Product::create(a, b);
    const BasePtr expected = Product::create(three, ab);
    BasePtr res;

    res = Sum::create(Product::create(two, ab), ab);

    BOOST_CHECK_EQUAL(res, expected);
}

BOOST_AUTO_TEST_CASE(collectPower)
/* 3*a^3 + a^3 = 4*a^3. */
{
    const BasePtr pow = Power::create(a, three);
    const BasePtr s1 = Product::create(three, pow);
    const BasePtr res = Sum::create(s1, pow);
    const BasePtrList& factors(res->operands());

    BOOST_TEST(isProduct(*res));

    BOOST_CHECK_EQUAL(2, factors.size());

    BOOST_CHECK_EQUAL(four, factors.front());
    BOOST_CHECK_EQUAL(a, factors.back()->base());
    BOOST_CHECK_EQUAL(three, factors.back()->exp());
}

BOOST_AUTO_TEST_CASE(collectProductOfPower)
/* a^2*sqrt(b) - a^2*sqrt(b) = 0. */
{
    const BasePtr pow1 = Power::create(a, two);
    const BasePtr pow2 = Power::sqrt(b);
    const BasePtr p1 = Product::create(pow1, pow2);
    const BasePtr p2 = Product::minus(p1);
    const BasePtr res = Sum::create(p1, p2);

    BOOST_TEST(isZero(*res));
}

BOOST_AUTO_TEST_CASE(twoSums)
/* (a + b) + (a + c) = 2*a + b + c. */
{
    const BasePtr sum1 = Sum::create(a, b);
    const BasePtr sum2 = Sum::create(a, c);
    const BasePtr res = Sum::create(sum1, sum2);
    const BasePtrList expected{Product::create(two, a), b, c};

    BOOST_TEST(isSum(*res));

    BOOST_TEST(expected == res->operands(), per_element());
}

BOOST_AUTO_TEST_CASE(sumOfSumAndSymbols)
/* (a + d) + e + c = a + c + d + e. */
{
    const BasePtr sum1 = Sum::create(a, d);
    const BasePtrList expected{a, c, d, e};
    const BasePtr res = Sum::create({sum1, e, c});
    const BasePtrList& summands{res->operands()};

    BOOST_TEST(isSum(*res));

    BOOST_TEST(expected == summands, per_element());
}

BOOST_AUTO_TEST_CASE(cancellationOfNumbersInTwoSums)
/* (1 + a) + (-1 + b) = a + b. */
{
    const BasePtr sum1 = Sum::create(one, a);
    const BasePtr sum2 = Sum::create(Numeric::mOne(), b);
    const BasePtr res = Sum::create(sum1, sum2);

    BOOST_TEST(isSum(*res));

    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(a, res->operands().front());
    BOOST_CHECK_EQUAL(b, res->operands().back());
}

BOOST_AUTO_TEST_CASE(simpleCollectionOfNumericPowers)
/* sqrt(2) + sqrt(2) = 2*sqrt(2). */
{
    const BasePtr res = Sum::create(sqrtTwo, sqrtTwo);

    BOOST_TEST(isProduct(*res));
    BOOST_CHECK_EQUAL(Product::create(two, sqrtTwo), res);
}

BOOST_AUTO_TEST_CASE(collectionOfNumericPowers)
/* 2*sqrt(2) + sqrt(2) = 3*sqrt(2). */
{
    const BasePtr res = Sum::create(Product::create(two, sqrtTwo), sqrtTwo);

    BOOST_TEST(isProduct(*res));
    BOOST_CHECK_EQUAL(Product::create(three, sqrtTwo), res);
}

BOOST_AUTO_TEST_CASE(collectionOfNumericPowersInProduct)
/* 2*sqrt(2)*3^(1/4) + sqrt(2)*3^(1/4) = 3*sqrt(2)*3^(1/4). */
{
    const BasePtr product = Product::create(sqrtTwo, Power::create(three, Numeric::fourth()));
    const BasePtr res = Sum::create(Product::create(two, product), product);

    BOOST_TEST(isProduct(*res));

    BOOST_CHECK_EQUAL(Product::create(three, product), res);
}

BOOST_AUTO_TEST_CASE(collectNumericPowerProduct)
/* (2 + sqrt(2)*sqrt(3)) + (1 + 2*sqrt(2)*sqrt(3)) = 3 + 3*sqrt(6). */
{
    const BasePtr expected = Sum::create(three, Product::create(three, Power::sqrt(six)));
    const BasePtr term = Product::create(sqrtTwo, sqrtThree);
    const BasePtr sum1 = Sum::create(two, term);
    const BasePtr sum2 = Sum::create(one, Product::create(two, term));
    const BasePtr res = Sum::create(sum1, sum2);

    BOOST_TEST(isSum(*res));
    BOOST_TEST(res->isConst());
    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(collectionOfConstCoeff)
/* No collection of 2*a + sqrt(3)*a (would be expanded afterwards). */
{
    const BasePtr s1 = Product::create(two, a);
    const BasePtr s2 = Product::create(sqrtThree, a);
    const BasePtr res = Sum::create(s1, s2);

    BOOST_TEST(isSum(*res));
    BOOST_CHECK_EQUAL(s1, res->operands().front());
    BOOST_CHECK_EQUAL(s2, res->operands().back());
}

BOOST_AUTO_TEST_CASE(collectionOfPi)
/* 2*Pi + Pi = 3*Pi. */
{
    const BasePtr res = Sum::create(Product::create(two, pi), pi);

    BOOST_CHECK_EQUAL(Product::create(three, pi), res);
}

BOOST_AUTO_TEST_CASE(collectionOfE)
/* 2*e + sqrt(3)*e = (2 + sqrt(3))*e. */
{
    const BasePtr e = Constant::createE();
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr res = Sum::create(Product::create(two, e), Product::create(sqrtThree, e));

    BOOST_CHECK_EQUAL(Product::create(Sum::create(two, sqrtThree), e), res);
}

BOOST_AUTO_TEST_CASE(collectionOfPiTimesSymbol)
/* 2*Pi*a*b + Pi*a*b = 3*Pi*a*b. */
{
    const BasePtr piAb = Product::create(pi, a, b);
    const BasePtr res = Sum::create(Product::create(two, piAb), piAb);
    const BasePtr expected = Product::create(three, piAb);

    BOOST_TEST(isProduct(*res));
    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(noCollectionOfPiInDifferentProducts)
/* No collection of terms for 3*Pi + 2*Pi*a. */
{
    const BasePtr s1 = Product::create(three, pi);
    const BasePtr s2 = Product::create(two, pi, a);
    const BasePtr res = Sum::create(s1, s2);

    BOOST_TEST(isSum(*res));
    BOOST_CHECK_EQUAL(s1, res->operands().front());
    BOOST_CHECK_EQUAL(s2, res->operands().back());
}

BOOST_AUTO_TEST_CASE(noCollectionOfNumberAndPi)
/* No collection of terms for 2*a*b + Pi*a*b. */
{
    const BasePtr ab = Product::create(a, b);
    const BasePtr s1 = Product::create(two, ab);
    const BasePtr s2 = Product::create(pi, ab);
    const BasePtr res = Sum::create(s1, s2);

    BOOST_TEST(isSum(*res));
    BOOST_CHECK_EQUAL(s1, res->operands().front());
    BOOST_CHECK_EQUAL(s2, res->operands().back());
}

BOOST_AUTO_TEST_CASE(collectionOfNumberNumPowAndPi)
/* 2*sqrt(3)*Pi + 4*sqrt(3)*Pi = 6*sqrt(3)*Pi. */
{
    const BasePtr fac = Product::create(sqrtThree, pi);
    const BasePtr expected = Product::create(six, fac);
    const BasePtr s1 = Product::create(two, fac);
    const BasePtr s2 = Product::create(four, fac);
    const BasePtr res = Sum::create(s1, s2);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(nonEqualConstTermsWithPi)
/* 2*Pi*a*b + sqrt(3)*Pi*a*b is not collected, as the factor stays a sum (would be expanded) */
{
    const BasePtr piAb = Product::create(pi, a, b);
    const BasePtr s1 = Product::create(two, piAb);
    const BasePtr s2 = Product::create(sqrtThree, piAb);
    const BasePtr res = Sum::create(s1, s2);

    BOOST_TEST(isSum(*res));
    BOOST_CHECK_EQUAL(s1, res->operands().front());
    BOOST_CHECK_EQUAL(s2, res->operands().back());
}

BOOST_AUTO_TEST_CASE(noCollectionOfPiNumericPowCoeff)
/* 2*Pi + sqrt(3)*Pi isn't collected, because it would be expanded later on. */
{
    const BasePtr s1 = Product::create(two, pi);
    const BasePtr s2 = Product::create(sqrtThree, pi);
    const BasePtr res = Sum::create(s1, s2);

    BOOST_TEST(isSum(*res));
    BOOST_CHECK_EQUAL(s1, res->operands().front());
    BOOST_CHECK_EQUAL(s2, res->operands().back());
}

BOOST_AUTO_TEST_CASE(sumOfEqualFunctionsEqualArgs)
/* sin(a) + 2*sin(a) = 3*sin(a). */
{
    const BasePtr expected = Product::create(three, sinA);
    const BasePtr res = Sum::create(sinA, Product::create(two, sinA));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sumOfEqualFunctionsDifferentArguments)
/* No simplification of sin(a) + sin(b). */
{
    const BasePtr sinB = Trigonometric::createSin(b);
    const BasePtr res = Sum::create(sinA, sinB);

    BOOST_TEST(isSum(*res));
    BOOST_CHECK_EQUAL(sinA, res->operands().front());
    BOOST_CHECK_EQUAL(sinB, res->operands().back());
}

BOOST_AUTO_TEST_CASE(sumOfLogarithmDifferentArguments)
/* No simplification of log(2) + log(3). */
{
    const BasePtr logTwo = Logarithm::create(two);
    const BasePtr logThree = Logarithm::create(three);
    const BasePtr sum = Sum::create(logTwo, logThree);

    BOOST_TEST(isSum(*sum));
    BOOST_CHECK_EQUAL(logTwo, sum->operands().front());
    BOOST_CHECK_EQUAL(logThree, sum->operands().back());
}

BOOST_AUTO_TEST_CASE(sumOfLogarithmNoSimplification)
/* No simplification of log(a) - log(b). */
{
    const BasePtr logA = Logarithm::create(a);
    const BasePtr minusLogB = Product::minus(Logarithm::create(b));
    const BasePtr sum = Sum::create(logA, minusLogB);

    BOOST_TEST(isSum(*sum));
    BOOST_CHECK_EQUAL(logA, sum->operands().front());
    BOOST_CHECK_EQUAL(minusLogB, sum->operands().back());
}

BOOST_AUTO_TEST_CASE(simpleNumericEvaluation)
/* Numeric evaluation of sqrt(2) + e. */
{
    const BasePtr sum = Sum::create(sqrtTwo, Constant::createE());
    const double expected = std::sqrt(2.0) + M_E;

    BOOST_CHECK_EQUAL(expected, sum->numericEval().value());
}

BOOST_AUTO_TEST_CASE(numericEvaluation)
/* 1 + Pi + sqrt(2) + sqrt(3)*4^(1/17) can be numerically evaluated. */
{
    const double expected = 1.0 + M_PI + std::sqrt(2.0) + std::sqrt(3.0) * std::pow(4.0, 1.0 / 17.0);
    BasePtrList summands;
    BasePtr res;

    summands.push_back(one);
    summands.push_back(pi);
    summands.push_back(sqrtTwo);
    summands.push_back(Product::create(sqrtThree, Power::create(four, Numeric::create(1, 17))));

    res = Sum::create(summands);

    BOOST_CHECK_EQUAL(expected, res->numericEval().value());
}

BOOST_AUTO_TEST_CASE(contractableSinCosSquareWithoutPrefactor)
/* sin(a)^2 + cos(a)^2 = 1. */
{
    const BasePtr result = Sum::create(Power::create(sinA, two), Power::create(cosA, two));

    BOOST_TEST(isOne(*result));
}

BOOST_AUTO_TEST_CASE(contractableSinCosSquarePrefactorMinusOne)
{
    const BasePtr result =
      Sum::create(Product::minus(Power::create(sinA, two)), Product::minus(Power::create(cosA, two)));

    BOOST_CHECK_EQUAL(Numeric::mOne(), result);
}

BOOST_AUTO_TEST_CASE(contractableSinCosSquareNegNumericPrefactor)
{
    const BasePtr fac = Numeric::create(-17, 18);
    const BasePtr result =
      Sum::create(Product::create(fac, Power::create(sinA, two)), Product::create(fac, Power::create(cosA, two)));

    BOOST_CHECK_EQUAL(fac, result);
}

BOOST_AUTO_TEST_CASE(contractableSinCosSquarePosNumPowPrefactor)
{
    const BasePtr fac = Power::create(Numeric::create(12), Numeric::create(1, 5));
    const BasePtr result =
      Sum::create(Product::create(fac, Power::create(cosA, two)), Product::create(fac, Power::create(sinA, two)));

    BOOST_CHECK_EQUAL(fac, result);
}

BOOST_AUTO_TEST_CASE(contractableSinCosSquareNegNumPowPrefactor)
{
    const BasePtr result = Sum::create(
      Product::create(sqrtThree, Power::create(cosA, two)), Product::create(sqrtThree, Power::create(sinA, two)));

    BOOST_CHECK_EQUAL(sqrtThree, result);
}

BOOST_AUTO_TEST_CASE(nonContractableSinCosSquareNonEqualPrefactor)
{
    const BasePtr s1 = Product::create(three, Power::create(sinA, two));
    const BasePtr s2 = Product::create(two, Power::create(cosA, two));
    const BasePtr result = Sum::create(s1, s2);

    BOOST_TEST(isSum(*result));
    BOOST_CHECK_EQUAL(s2, result->operands().front());
    BOOST_CHECK_EQUAL(s1, result->operands().back());
}

BOOST_AUTO_TEST_SUITE_END()
