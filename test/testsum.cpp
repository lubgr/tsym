
#include <cmath>
#include <limits>
#include "abc.h"
#include "sum.h"
#include "product.h"
#include "symbol.h"
#include "undefined.h"
#include "numeric.h"
#include "power.h"
#include "constant.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Sum)
{
    const BasePtr sinA = Trigonometric::createSin(a);
    const BasePtr cosA = Trigonometric::createCos(a);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr pi = Constant::createPi();
};

TEST(Sum, typeString)
{
    const std::string expected("Sum");
    const BasePtr ptr = Sum::create(a, b);

    CHECK_EQUAL(expected, ptr->typeStr());
}

TEST(Sum, undefined)
    /* If one summand is undefined, the result is, too. */
{
    const BasePtr u = Undefined::create();
    const BasePtr p = Sum::create(u, a);

    CHECK(p->isUndefined());
}

TEST(Sum, createWithOneSummandGivenAsList)
{
    BasePtrCtr summands;
    BasePtr res;

    summands.push_back(a);

    res = Sum::create(summands);

    CHECK_EQUAL(a, res);
}

TEST(Sum, twoNumericSummands)
    /* 2 + 3 = 5. */
{
    const BasePtr sum = Sum::create(two, three);

    CHECK(sum->isNumeric());
    CHECK_EQUAL(5, sum->numericEval());
}

TEST(Sum, twoNumericSummandsResultingInZero)
    /* 2 - 2 = 0. */
{
    const BasePtr minusTwo = Numeric::create(-2);
    const BasePtr res = Sum::create(two, minusTwo);

    CHECK(res->isZero());
}

TEST(Sum, zeroPlusZero)
{
    const BasePtr res = Sum::create(zero, zero);

    CHECK(res->isZero());
}

TEST(Sum, symbolPlusZero)
    /* a + 0 = a. */
{
    const BasePtr res = Sum::create(a, zero);

    CHECK_EQUAL(a, res);
}

TEST(Sum, zeroPlusSymbol)
    /* 0 + a = a. */
{
    const BasePtr res = Sum::create(zero, a);

    CHECK_EQUAL(a, res);
}

TEST(Sum, rearrangeTwoSymbols)
    /* Check simple ordering: b + a = a + b. */
{
    const BasePtr res = Sum::create(b, a);

    CHECK(res->isSum());
    CHECK_EQUAL(a, res->operands().front());
    CHECK_EQUAL(b, res->operands().back());
}

TEST(Sum, rearrangeFiveSymbols)
    /*  More complex ordering: e + c + a + b + d = a + b + c + d + e. */
{
    const BasePtr res = Sum::create({ e, c, a, b, d });
    BasePtrCtr summands;

    CHECK(res->isSum());

    summands = res->operands();

    CHECK_EQUAL(5, summands.size());

    for (const auto& summand : summands)
        CHECK(summand->isSymbol());

    auto it = summands.begin();
    CHECK_EQUAL(a, *it);
    CHECK_EQUAL(b, *++it);
    CHECK_EQUAL(c, *++it);
    CHECK_EQUAL(d, *++it);
    CHECK_EQUAL(e, *++it);
}

TEST(Sum, orderingOfMixedTerms)
    /* a - sqrt(2)*a is equal to -a*sqrt(2) + a. */
{
    const BasePtr result1 = Sum::create(a, Product::minus(sqrtTwo, a));
    const BasePtr result2 = Sum::create(Product::minus(sqrtTwo, a), a);

    CHECK_EQUAL(result1, result2);
}

TEST(Sum, collectSymbols)
    /* a + a = 2*a. */
{
    const BasePtr res = Sum::create(a, a);
    const BasePtrCtr& summands(res->operands());

    CHECK(res->isProduct());

    CHECK_EQUAL(2, summands.size());

    CHECK_EQUAL(two, summands.front());
    CHECK_EQUAL(a, summands.back());
}

TEST(Sum, collectProducts)
    /* 2*a*b + 3*a*b = 5*a*b. */
{
    const BasePtr ab = Product::create(a, b);
    const BasePtr twoAB = Product::create(two, ab);
    const BasePtr threeAB = Product::create(three, ab);
    const BasePtr res = Sum::create(twoAB, threeAB);
    const BasePtrCtr& summands(res->operands());

    CHECK(res->isProduct());

    CHECK_EQUAL(3, summands.size());
    CHECK_EQUAL(five, summands.front());
    CHECK_EQUAL(a, *++summands.begin());
    CHECK_EQUAL(b, summands.back());
}

TEST(Sum, collectProductOfSymbols)
    /* 2*a*b + a*b = 3*a*b. */
{
    const BasePtr ab = Product::create(a, b);
    const BasePtr expected = Product::create(three, ab);
    BasePtr res;

    res = Sum::create(Product::create(two, ab), ab);

    CHECK_EQUAL(res, expected);
}

TEST(Sum, collectPower)
    /* 3*a^3 + a^3 = 4*a^3. */
{
    const BasePtr pow = Power::create(a, three);
    const BasePtr s1 = Product::create(three, pow);
    const BasePtr res = Sum::create(s1, pow);
    const BasePtrCtr& factors(res->operands());

    CHECK(res->isProduct());

    CHECK_EQUAL(2, factors.size());

    CHECK_EQUAL(four, factors.front());
    CHECK_EQUAL(a, factors.back()->base());
    CHECK_EQUAL(three, factors.back()->exp());
}

TEST(Sum, collectProductOfPower)
    /* a^2*sqrt(b) - a^2*sqrt(b) = 0. */
{
    const BasePtr pow1 = Power::create(a, two);
    const BasePtr pow2 = Power::sqrt(b);
    const BasePtr p1 = Product::create(pow1, pow2);
    const BasePtr p2 = Product::minus(p1);
    const BasePtr res = Sum::create(p1, p2);

    CHECK(res->isZero());
}

TEST(Sum, twoSums)
    /* (a + b) + (a + c) = 2*a + b + c. */
{
    const BasePtr sum1 = Sum::create(a, b);
    const BasePtr sum2 = Sum::create(a, c);
    const BasePtr res = Sum::create(sum1, sum2);
    const BasePtrCtr& summands(res->operands());

    CHECK(res->isSum());

    CHECK_EQUAL(3, summands.size());
    CHECK_EQUAL(Product::create(two, a), summands.front());
    CHECK_EQUAL(b, *++summands.begin());
    CHECK_EQUAL(c, summands.back());
}

TEST(Sum, sumOfSumAndSymbols)
    /* (a + d) + e + c = a + c + d + e. */
{
    const BasePtr sum1 = Sum::create(a, d);
    BasePtrCtr summands;
    BasePtr res;

    res = Sum::create({ sum1, e, c });

    CHECK(res->isSum());

    summands = res->operands();

    CHECK_EQUAL(4, summands.size());
    CHECK_EQUAL(a, summands.front());
    CHECK_EQUAL(c, *++summands.begin());
    CHECK_EQUAL(d, *++(++summands.begin()));
    CHECK_EQUAL(e, summands.back());
}

TEST(Sum, cancellationOfNumbersInTwoSums)
    /* (1 + a) + (-1 + b) = a + b. */
{
    const BasePtr sum1 = Sum::create(one, a);
    const BasePtr sum2 = Sum::create(Numeric::mOne(), b);
    const BasePtr res = Sum::create(sum1, sum2);

    CHECK(res->isSum());

    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(a, res->operands().front());
    CHECK_EQUAL(b, res->operands().back());
}

TEST(Sum, simpleCollectionOfNumericPowers)
    /* sqrt(2) + sqrt(2) = 2*sqrt(2). */
{
    const BasePtr res = Sum::create(sqrtTwo, sqrtTwo);

    CHECK(res->isProduct());
    CHECK_EQUAL(Product::create(two, sqrtTwo), res);
}

TEST(Sum, collectionOfNumericPowers)
    /* 2*sqrt(2) + sqrt(2) = 3*sqrt(2). */
{
    const BasePtr res = Sum::create(Product::create(two, sqrtTwo), sqrtTwo);

    CHECK(res->isProduct());
    CHECK_EQUAL(Product::create(three, sqrtTwo), res);
}

TEST(Sum, collectionOfNumericPowersInProduct)
    /* 2*sqrt(2)*3^(1/4) + sqrt(2)*3^(1/4) = 3*sqrt(2)*3^(1/4). */
{
    const BasePtr product = Product::create(sqrtTwo, Power::create(three, Numeric::fourth()));
    const BasePtr res = Sum::create(Product::create(two, product), product);

    CHECK(res->isProduct());

    CHECK_EQUAL(Product::create(three, product), res);
}

TEST(Sum, collectNumericPowerProduct)
    /* (2 + sqrt(2)*sqrt(3)) + (1 + 2*sqrt(2)*sqrt(3)) = 3 + 3*sqrt(6). */
{
    const BasePtr expected = Sum::create(three, Product::create(three, Power::sqrt(six)));
    const BasePtr term = Product::create(sqrtTwo, sqrtThree);
    const BasePtr sum1 = Sum::create(two, term);
    const BasePtr sum2 = Sum::create(one, Product::create(two, term));
    const BasePtr res = Sum::create(sum1, sum2);

    CHECK(res->isSum());
    CHECK(res->isConst());
    CHECK_EQUAL(expected, res);
}

TEST(Sum, collectionOfConstCoeff)
    /* No collection of 2*a + sqrt(3)*a (would be expanded afterwards). */
{
    const BasePtr s1 = Product::create(two, a);
    const BasePtr s2 = Product::create(sqrtThree, a);
    const BasePtr res = Sum::create(s1, s2);

    CHECK(res->isSum());
    CHECK_EQUAL(s1, res->operands().front());
    CHECK_EQUAL(s2, res->operands().back());
}

TEST(Sum, collectionOfPi)
    /* 2*Pi + Pi = 3*Pi. */
{
    const BasePtr res = Sum::create(Product::create(two, pi), pi);

    CHECK_EQUAL(Product::create(three, pi), res);
}

TEST(Sum, collectionOfE)
    /* 2*e + sqrt(3)*e = (2 + sqrt(3))*e. */
{
    const BasePtr e = Constant::createE();
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr res = Sum::create(Product::create(two, e), Product::create(sqrtThree, e));

    CHECK_EQUAL(Product::create(Sum::create(two, sqrtThree), e), res);
}

TEST(Sum, collectionOfPiTimesSymbol)
    /* 2*Pi*a*b + Pi*a*b = 3*Pi*a*b. */
{
    const BasePtr piAb = Product::create(pi, a, b);
    const BasePtr res = Sum::create(Product::create(two, piAb), piAb);
    const BasePtr expected = Product::create(three, piAb);

    CHECK(res->isProduct());
    CHECK_EQUAL(expected, res);
}

TEST(Sum, noCollectionOfPiInDifferentProducts)
    /* No collection of terms for 3*Pi + 2*Pi*a. */
{
    const BasePtr s1 = Product::create(three, pi);
    const BasePtr s2 = Product::create(two, pi, a);
    const BasePtr res = Sum::create(s1, s2);

    CHECK(res->isSum());
    CHECK_EQUAL(s1, res->operands().front());
    CHECK_EQUAL(s2, res->operands().back());
}

TEST(Sum, noCollectionOfNumberAndPi)
    /* No collection of terms for 2*a*b + Pi*a*b. */
{
    const BasePtr ab = Product::create(a, b);
    const BasePtr s1 = Product::create(two, ab);
    const BasePtr s2 = Product::create(pi, ab);
    const BasePtr res = Sum::create(s1, s2);

    CHECK(res->isSum());
    CHECK_EQUAL(s1, res->operands().front());
    CHECK_EQUAL(s2, res->operands().back());
}

TEST(Sum, collectionOfNumberNumPowAndPi)
    /* 2*sqrt(3)*Pi + 4*sqrt(3)*Pi = 6*sqrt(3)*Pi. */
{
    const BasePtr fac = Product::create(sqrtThree, pi);
    const BasePtr expected = Product::create(six, fac);
    const BasePtr s1 = Product::create(two, fac);
    const BasePtr s2 = Product::create(four, fac);
    const BasePtr res = Sum::create(s1, s2);

    CHECK_EQUAL(expected, res);
}

TEST(Sum, nonEqualConstTermsWithPi)
    /* 2*Pi*a*b + sqrt(3)*Pi*a*b is not collected, as the factor stays a sum (would be expanded) */
{
    const BasePtr piAb = Product::create(pi, a, b);
    const BasePtr s1 = Product::create(two, piAb);
    const BasePtr s2 = Product::create(sqrtThree, piAb);
    const BasePtr res = Sum::create(s1, s2);

    CHECK(res->isSum());
    CHECK_EQUAL(s1, res->operands().front());
    CHECK_EQUAL(s2, res->operands().back());
}

TEST(Sum, noCollectionOfPiNumericPowCoeff)
    /* 2*Pi + sqrt(3)*Pi isn't collected, because it would be expanded later on. */
{
    const BasePtr s1 = Product::create(two, pi);
    const BasePtr s2 = Product::create(sqrtThree, pi);
    const BasePtr res = Sum::create(s1, s2);

    CHECK(res->isSum());
    CHECK_EQUAL(s1, res->operands().front());
    CHECK_EQUAL(s2, res->operands().back());
}

TEST(Sum, sumOfEqualFunctionsEqualArgs)
    /* sin(a) + 2*sin(a) = 3*sin(a). */
{
    const BasePtr expected = Product::create(three, sinA);
    const BasePtr res = Sum::create(sinA, Product::create(two, sinA));

    CHECK_EQUAL(expected, res);
}

TEST(Sum, sumOfEqualFunctionsDifferentArguments)
    /* No simplification of sin(a) + sin(b). */
{
    const BasePtr sinB = Trigonometric::createSin(b);
    const BasePtr res = Sum::create(sinA, sinB);

    CHECK(res->isSum());
    CHECK_EQUAL(sinA, res->operands().front());
    CHECK_EQUAL(sinB, res->operands().back());
}

TEST(Sum, sumOfLogarithmDifferentArguments)
    /* No simplification of log(2) + log(3). */
{
    const BasePtr logTwo = Logarithm::create(two);
    const BasePtr logThree = Logarithm::create(three);
    const BasePtr sum = Sum::create(logTwo, logThree);

    CHECK(sum->isSum());
    CHECK_EQUAL(logTwo, sum->operands().front());
    CHECK_EQUAL(logThree, sum->operands().back());
}

TEST(Sum, sumOfLogarithmNoSimplification)
    /* No simplification of log(a) - log(b). */
{
    const BasePtr logA = Logarithm::create(a);
    const BasePtr minusLogB = Product::minus(Logarithm::create(b));
    const BasePtr sum = Sum::create(logA, minusLogB);

    CHECK(sum->isSum());
    CHECK_EQUAL(logA, sum->operands().front());
    CHECK_EQUAL(minusLogB, sum->operands().back());
}

TEST(Sum, simpleNumericEvaluation)
    /* Numeric evaluation of sqrt(2) + e. */
{
    const BasePtr sum = Sum::create(sqrtTwo, Constant::createE());
    const double expected = std::sqrt(2.0) + M_E;

    CHECK(sum->isNumericallyEvaluable());
    CHECK_EQUAL(expected, sum->numericEval());
}

TEST(Sum, numericEvaluation)
    /* 1 + Pi + sqrt(2) + sqrt(3)*4^(1/17) can be numerically evaluated. */
{
    const double expected = 1.0 + M_PI + std::sqrt(2.0) + std::sqrt(3.0)*std::pow(4.0, 1.0/17.0);
    BasePtrCtr summands;
    BasePtr res;

    summands.push_back(one);
    summands.push_back(pi);
    summands.push_back(sqrtTwo);
    summands.push_back(Product::create(sqrtThree, Power::create(four, Numeric::create(1, 17))));

    res = Sum::create(summands);

    CHECK(res->isNumericallyEvaluable());
    CHECK_EQUAL(expected, res->numericEval());
}

TEST(Sum, contractableSinCosSquareWithoutPrefactor)
    /* sin(a)^2 + cos(a)^2 = 1. */
{
    const BasePtr result = Sum::create(Power::create(sinA, two), Power::create(cosA, two));

    CHECK(result->isOne());
}

TEST(Sum, contractableSinCosSquarePrefactorMinusOne)
{
    const BasePtr result = Sum::create(Product::minus(Power::create(sinA, two)),
            Product::minus(Power::create(cosA, two)));

    CHECK_EQUAL(Numeric::mOne(), result);
}

TEST(Sum, contractableSinCosSquareNegNumericPrefactor)
{
    const BasePtr fac = Numeric::create(-17, 18);
    const BasePtr result = Sum::create(Product::create(fac, Power::create(sinA, two)),
            Product::create(fac, Power::create(cosA, two)));

    CHECK_EQUAL(fac, result);
}

TEST(Sum, contractableSinCosSquarePosNumPowPrefactor)
{
    const BasePtr fac = Power::create(Numeric::create(12), Numeric::create(1, 5));
    const BasePtr result = Sum::create(Product::create(fac, Power::create(cosA, two)),
            Product::create(fac, Power::create(sinA, two)));

    CHECK_EQUAL(fac, result);
}

TEST(Sum, contractableSinCosSquareNegNumPowPrefactor)
{
    const BasePtr result = Sum::create(Product::create(sqrtThree, Power::create(cosA, two)),
            Product::create(sqrtThree, Power::create(sinA, two)));

    CHECK_EQUAL(sqrtThree, result);
}

TEST(Sum, nonContractableSinCosSquareNonEqualPrefactor)
{
    const BasePtr s1 = Product::create(three, Power::create(sinA, two));
    const BasePtr s2 = Product::create(two, Power::create(cosA, two));
    const BasePtr result = Sum::create(s1, s2);

    CHECK(result->isSum());
    CHECK_EQUAL(s2, result->operands().front());
    CHECK_EQUAL(s1, result->operands().back());
}
