
#include <cmath>
#include <limits>
#include "abc.h"
#include "product.h"
#include "symbol.h"
#include "undefined.h"
#include "numeric.h"
#include "constant.h"
#include "options.h"
#include "trigonometric.h"
#include "numpowersimpl.h"
#include "power.h"
#include "sum.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Product)
{
    const Int primeResolutionLimit = options::getMaxPrimeResolution();
    const BasePtr& half = Numeric::half();
    const BasePtr minusOneHalf = Numeric::create(-1, 2);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sqrtSix = Power::sqrt(six);
    const BasePtr& oneThird = Numeric::third();
    const BasePtr& oneFourth = Numeric::fourth();

    void teardown()
    {
        options::setMaxPrimeResolution(primeResolutionLimit);
    }
};

TEST(Product, typeString)
{
    const std::string expected("Product");
    const BasePtr ptr = Product::create(a, b);

    CHECK_EQUAL(expected, ptr->typeStr());
}

TEST(Product, undefined)
    /* If one factor is undefined, a product is, too. */
{
    const BasePtr u = Undefined::create();
    const BasePtr p = Product::create(u, a);

    CHECK(p->isUndefined());
}

TEST(Product, zero)
    /* 0*(a*b) = 0. */
{
    const BasePtr p1 = Product::create(a, b);
    const BasePtr p2 = Product::create(p1, Numeric::zero());

    CHECK(p2->isZero());
}

TEST(Product, twoNumericFactors)
    /* 2*3 = 6. */
{
    const BasePtr p = Product::create(two, three);

    CHECK(p->isNumeric());
    CHECK_EQUAL(6, p->numericEval());
}

TEST(Product, twoNumericFactorsResultingInOne)
    /* (1/2)*2 = 1. */
{
    const BasePtr p = Product::create(two, Numeric::half());

    CHECK(p->isNumeric());
    CHECK_EQUAL(1, p->numericEval());
}

TEST(Product, numberTimesNumericPower)
    /* 2*sqrt(2) is not altered. */
{
    const BasePtr p = Product::create(two, sqrtTwo);

    CHECK(p->isProduct());
    CHECK(p->isConst());

    CHECK_EQUAL(2, p->operands().front()->numericEval());
    CHECK(p->operands().back()->isNumericPower());
}

TEST(Product, numberTimesResolvableNumPow)
    /* (-2)*sqrt(9) = -6. */
{
    const BasePtr res = Product::create(Numeric::create(-2), Power::sqrt(nine));
    const BasePtr expected = Numeric::create(-6);

    CHECK_EQUAL(expected, res);
}

TEST(Product, resolvableNumPowNegativeBase)
    /* 2*(-1)*2^(-1/2) = -sqrt(2). */
{
    const BasePtr expected = Product::minus(sqrtTwo);
    const BasePtr res = Product::create({ two,
            Product::minus(Power::create(two, Numeric::create(-1, 2))) });

    CHECK_EQUAL(expected, res);
}

TEST(Product, resolvableNumPowNegativeNumericFactor)
    /* (-2)*2^(-1/2) = -sqrt(2). */
{
    const BasePtr expected = Product::minus(sqrtTwo);
    const BasePtr res = Product::create({ Numeric::create(-2),
            Power::create(two, Numeric::create(-1, 2)) });

    CHECK_EQUAL(expected, res);
}

TEST(Product, resolvableNumPowNegativeNumericFractionFactor)
    /* b*(-1/2)*sqrt(2)*a = -2^(-1/2)*a*b. */
{
    const BasePtr expected = Product::minus(a, b, Power::create(two, Numeric::create(-1, 2)));
    const BasePtr res = Product::create({ b, Numeric::create(-1, 2), sqrtTwo, a });

    CHECK_EQUAL(expected, res);
}

TEST(Product, resolvableNumPowMixedWithSymbol)
    /* 2*a*(-1)*2^(-1/2) = -sqrt(2)*a. */
{
    const BasePtr expected = Product::minus(sqrtTwo, a);
    const BasePtr res = Product::create({ two, a,
            Product::minus(Power::create(two, Numeric::create(-1, 2))) });

    CHECK_EQUAL(expected, res);
}

TEST(Product, constantTimesNumeric)
    /* Pi*2 = 2*Pi. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr result = Product::create(pi, two);

    CHECK(result->isProduct());
    CHECK_EQUAL(two, result->operands().front());
    CHECK_EQUAL(pi, result->operands().back());
}

TEST(Product, numericPowerTimesNumber)
    /* sqrt(2)*2 = 2*sqrt(2). */
{
    const BasePtr p = Product::create(sqrtTwo, two);

    CHECK(p->isProduct());
    CHECK(p->isConst());

    CHECK_EQUAL(two, p->operands().front());
    CHECK_EQUAL(sqrtTwo, p->operands().back());
}

TEST(Product, intTimesNumericPower)
    /* -1/4*sqrt(2) = -1/2*2^(-1/2). */
{
    const BasePtr res = Product::create(Numeric::create(-1, 4), sqrtTwo);

    CHECK(res->isProduct());
    CHECK_EQUAL(Numeric::create(-1, 2), res->operands().front());
    CHECK_EQUAL(Power::create(two, Numeric::create(-1, 2)), res->operands().back());
}

TEST(Product, intTimesNumericPowerTimesOne)
    /* -1/4*sqrt(2)*1 = -1/2*2^(-1/2). */
{
    const BasePtr res = Product::create(Numeric::create(-1, 4), sqrtTwo, one);

    CHECK(res->isProduct());
    CHECK_EQUAL(Numeric::create(-1, 2), res->operands().front());
    CHECK_EQUAL(Power::create(two, Numeric::create(-1, 2)), res->operands().back());
}

TEST(Product, numberAndNumericPowerEqualBase)
    /* 2*2^(2/3) isn't altered. */
{
    const BasePtr pow = Power::create(two, Numeric::create(2, 3));
    const BasePtr res = Product::create(two, pow);

    CHECK(res->isProduct());
    CHECK_EQUAL(two, res->operands().front());
    CHECK_EQUAL(pow, res->operands().back());
}

TEST(Product, numericPowersEqualBase)
    /* 2^(1/2)*2^(1/3) = 2^(2/3) */
{
    const BasePtr twoToTheThird = Power::create(two, Numeric::third());
    const BasePtr p = Product::create(sqrtTwo, twoToTheThird);

    CHECK(p->isNumericPower());
    DOUBLES_EQUAL(std::pow(2, 5.0/6.0), p->numericEval().toDouble(), 1.e-10);
}

TEST(Product, numericPowersDifferentExpSameBase)
    /* 2^(1/3)*2(1/4) = 2^(7/12). */
{
    const Number expectedExp(7, 12);
    const BasePtr np1 = Power::create(two, oneThird);
    const BasePtr np2 = Power::create(two, oneFourth);
    const BasePtr res = Product::create(np1, np2);

    CHECK(res->isPower());
    CHECK(res->isNumericPower());
    CHECK_EQUAL(2, res->base()->numericEval());

    CHECK_EQUAL(expectedExp, res->exp()->numericEval());
}

TEST(Product, numericPowersDifferentExpDifferentBase)
    /* No simplification of 2^(1/3)*3^(1/4). */
{
    const BasePtr np1 = Power::create(two, oneThird);
    const BasePtr np2 = Power::create(three, oneFourth);
    const BasePtr res = Product::create(np1, np2);

    CHECK(res->isProduct());
    CHECK_EQUAL(np1, res->operands().front());
    CHECK_EQUAL(np2, res->operands().back());
}

TEST(Product, reductionOfDifferentNumericPowers)
    /* (3/2)*sqrt(2/3) = sqrt(3/2). */
{
    const BasePtr threeOverTwo = Numeric::create(3, 2);
    const BasePtr origPow = Power::sqrt(Numeric::create(2, 3));
    const BasePtr res = Product::create(threeOverTwo, origPow);

    CHECK(res->isPower());
    CHECK_EQUAL(half, res->exp());
    CHECK_EQUAL(threeOverTwo, res->base());
}

TEST(Product, reductionOfSameNumericBaseDifferentExp)
    /* 2/sqrt(2) = sqrt(2). */
{
    const BasePtr res = Product::create(two, Power::create(two, minusOneHalf));

    CHECK(res->isPower());
    CHECK_EQUAL(half, res->exp());
    CHECK_EQUAL(two, res->base());
}

TEST(Product, reductionOfSameNumericBaseDifferentExponent)
    /* sqrt(2)/2 = 1/sqrt(2). */
{
    const BasePtr expected = Power::create(two, minusOneHalf);
    const BasePtr res = Product::create(sqrtTwo, half);

    CHECK(res->isPower());
    CHECK_EQUAL(minusOneHalf, res->exp());
    CHECK_EQUAL(two, res->base());
}

TEST(Product, mergeNumericsOfSimplifiedNumericPower)
    /* (2*sqrt(3))*sqrt(6) = 6*sqrt(2). */
{
    const BasePtr f1 = Product::create(two, sqrtThree);
    const BasePtr f2 = Power::sqrt(six);
    BasePtr res;

    res = Product::create(f1, f2);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(six, res->operands().front());
    CHECK_EQUAL(sqrtTwo, res->operands().back());
}

TEST(Product, differentNumericPowerProduct)
    /* sqrt(2)*sqrt(3) = sqrt(6). */
{
    const BasePtr p = Product::create(sqrtTwo, sqrtThree);

    CHECK(p->isNumericPower());
    CHECK_EQUAL(six, p->base());
    CHECK_EQUAL(Number(1, 2), p->exp()->numericEval());
}

TEST(Product, simpleNumericPowersDifferentExpSign)
    /* 17^(1/3)*3^(-1/3) = (17/3)^(1/3). */
{
    const BasePtr expected = Power::create(Numeric::create(17, 3), oneThird);
    const BasePtr pow1 = Power::create(Numeric::create(17), oneThird);
    const BasePtr pow2 = Power::create(three, Numeric::create(-1, 3));
    BasePtr res;

    res = Product::create(pow1, pow2);

    CHECK_EQUAL(expected, res);
}

TEST(Product, numericPowersDifferentExpSign)
    /* (3/5)^(12/23)*(9/11)^(-12/23)*2^(-12/23)*(1/10)^(12/23) = (11/300)^(12/23). */
{
    const BasePtr exp = Numeric::create(12, 23);
    const BasePtr expected = Power::create(Numeric::create(11, 300), exp);
    BasePtrList fac;
    BasePtr res;

    fac.push_back(Power::create(Numeric::create(3, 5), exp));
    fac.push_back(Power::create(Numeric::create(9, 11), Product::minus(exp)));
    fac.push_back(Power::create(two, Product::minus(exp)));
    fac.push_back(Power::create(Numeric::create(1, 10), exp));

    res = Product::create(fac);

    CHECK_EQUAL(expected, res);
}

TEST(Product, contractionOfNumerics)
    /* (1/4)*(2/5)*11*(3/7) = 33/70. */
{
    const BasePtr res = Product::create({ Numeric::fourth(), Numeric::create(2, 5),
            Numeric::create(11), Numeric::create(3, 7) });

    CHECK_EQUAL(Numeric::create(33, 70), res);
}

TEST(Product, contractionOfConstPowers)
    /* 3/2*(1/3)^(1/3)*sqrt(2/3) = 3^(2/3)/sqrt(6). */
{
    const BasePtr expected = Product::create(Power::create(three, Numeric::create(2, 3)),
            Power::create(six, Numeric::create(-1, 2)));
    BasePtrList fac;
    BasePtr res;

    fac.push_back(Numeric::create(3, 2));
    fac.push_back(Power::create(oneThird, oneThird));
    fac.push_back(Power::sqrt(Numeric::create(2, 3)));

    res = Product::create(fac);

    CHECK_EQUAL(expected, res);
}

TEST(Product, preSortingOfConstPowers)
    /* The same as before, but without presorted ordering. 3/2*sqrt(2/3)*(1/3)^(1/3) =
     * 3^(2/3)/sqrt(6). */
{
    const BasePtr expected = Product::create(Power::create(three, Numeric::create(2, 3)),
            Power::create(six, Numeric::create(-1, 2)));
    BasePtrList fac;
    BasePtr res;

    fac.push_back(Numeric::create(3, 2));
    fac.push_back(Power::sqrt(Numeric::create(2, 3)));
    fac.push_back(Power::create(oneThird, oneThird));

    res = Product::create(fac);

    CHECK_EQUAL(expected, res);
}

TEST(Product, orderingOfConstPowers)
    /* (3^(1/3)*5^(1/5))*2^(1/5) = 3^(1/3)*10^(1/5). */
{
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr f1 = Power::create(three, oneThird);
    const BasePtr f2 = Power::create(five, oneFifth);
    const BasePtr f3 = Power::create(two, oneFifth);
    const BasePtr p1 = Product::create(f1, f2);
    const BasePtr res = Product::create(p1, f3);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(f1, res->operands().front());
    CHECK_EQUAL(Power::create(ten, oneFifth), res->operands().back());
}

TEST(Product, expansionOfConstPowerAndSum)
    /* (1 + sqrt(2))*sqrt(3) = sqrt(3) + sqrt(6). */
{
    const BasePtr sum = Sum::create(one, sqrtTwo);
    const BasePtr res = Product::create(sum, sqrtThree);

    CHECK(res->isSum());
    CHECK_EQUAL(sqrtThree, res->operands().front());
    CHECK_EQUAL(sqrtSix, res->operands().back());
}

TEST(Product, expansionOfNumberConstPowerAndSum)
    /* 2*sqrt(2)*(a + b) = 2*sqrt(2)*a + 2*sqrt(2)*b. */
{
    const BasePtr fac = Product::create(two, sqrtTwo);
    const BasePtr res = Product::create(fac, Sum::create(a, b));
    const BasePtr expected = Sum::create(Product::create(fac, a), Product::create(fac, b));

    CHECK_EQUAL(expected, res);
}

TEST(Product, expansionOfConstProductAndSum)
    /* 2*(1 + sqrt(3))*(a + b) = (2 + 2*sqrt(3))*a + (2 + 2*sqrt(3))*b. */
{
    const BasePtr aux = Sum::create(two, Product::create(two, sqrtThree));
    const BasePtr expected = Sum::create(Product::create(aux, a), Product::create(aux, b));
    BasePtrList fac;
    BasePtr res;

    fac.push_back(two);
    fac.push_back(Sum::create(one, sqrtThree));
    fac.push_back(Sum::create(a, b));

    res = Product::create(fac);

    CHECK_EQUAL(expected, res);
}

TEST(Product, contractNumSqrt)
    /* sqrt(2)*sqrt(3)*sqrt(5)*sqrt(6)*sqrt(7)*sqrt(10) = 30*sqrt(14). */
{
    const BasePtr expected = Product::create(Numeric::create(30), Power::sqrt(Numeric::create(14)));
    BasePtrList fac;
    BasePtr res;

    fac.push_back(sqrtTwo);
    fac.push_back(sqrtThree);
    fac.push_back(Power::sqrt(five));
    fac.push_back(Power::sqrt(six));
    fac.push_back(Power::sqrt(seven));
    fac.push_back(Power::sqrt(ten));

    res = Product::create(fac);

    CHECK_EQUAL(expected, res);
}

TEST(Product, contractNumPowWithEqualExp)
    /* 2^(1/3)*sqrt(3)*4^(1/3)*5^(1/3)*6^(1/3)*7^(1/4)*8^(1/3) = 4*sqrt(3)*7^(1/4)*30^(1/3). */
{
    const BasePtr sevenToTheFourth = Power::create(seven, Numeric::fourth());
    BasePtr expected;
    BasePtrList fac;
    BasePtr res;

    fac.push_back(Power::create(two, oneThird));
    fac.push_back(sqrtThree);
    fac.push_back(Power::create(four, oneThird));
    fac.push_back(Power::create(five, oneThird));
    fac.push_back(Power::create(six, oneThird));
    fac.push_back(sevenToTheFourth);
    fac.push_back(Power::create(eight, oneThird));

    res = Product::create(fac);

    fac.clear();
    fac.push_back(four);
    fac.push_back(sqrtThree);
    fac.push_back(sevenToTheFourth);
    fac.push_back(Power::create(Numeric::create(30), oneThird));

    expected = Product::create(fac);

    CHECK_EQUAL(expected, res);
}

TEST(Product, symbolTimesOne)
    /* a*1 = a. */
{
    const BasePtr res = Product::create(a, one);

    CHECK_EQUAL(a, res);
}

TEST(Product, oneTimesSymbol)
    /* 1*a = a. */
{
    const BasePtr res = Product::create(one, a);

    CHECK_EQUAL(a, res);
}

TEST(Product, multipleOnesTimesSum)
    /* 1*(a + b)*1*1*1 = a + b. */
{
    const BasePtr aPlusB = Sum::create(a, b);
    const BasePtr res = Product::create({ one, aPlusB, one, one, one });

    CHECK_EQUAL(aPlusB, res);
}

TEST(Product, minusOneTimesNegProduct)
    /* (-1)*(-a) = a. */
{
    const BasePtr res = Product::create(Numeric::mOne(), Product::minus(a));

    CHECK_EQUAL(a, res);
}

TEST(Product, equalSymbolBasesToPower)
    /* a*a = a^2. */
{
    const BasePtr res = Product::create(a, a);

    CHECK(res->isPower());
    CHECK_EQUAL(2, res->exp()->numericEval());
}

TEST(Product, equalPowerBasesToPower)
    /* b^2*b^3 = b^5. */
{
    const BasePtr pow1 = Power::create(b, two);
    const BasePtr pow2 = Power::create(b, three);
    const BasePtr res = Product::create(pow1, pow2);

    CHECK(res->isPower());
    CHECK_EQUAL(5, res->exp()->numericEval());
}

TEST(Product, equalPowerBasesToOne)
    /* a^(-1)*a = 1. */
{
    const BasePtr pow = Power::oneOver(a);
    const BasePtr res = Product::create(pow, a);

    CHECK(res->isOne());
}

TEST(Product, equalSumBasesToPower)
    /* (a + b)^3*(a + b) = (a + b)^4. */
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow1 = Power::create(sum, three);
    const BasePtr res = Product::create(pow1, sum);

    CHECK(res->isPower());
    CHECK_EQUAL(4, res->exp()->numericEval());
    CHECK_EQUAL(sum, res->base());
}

TEST(Product, rearrangeTwoSymbols)
    /* Simple ordering: b*a = a*b. */
{
    const BasePtr res = Product::create(b, a);
    BasePtr first;
    BasePtr second;

    CHECK(res->isProduct());

    first = res->operands().front();
    second = res->operands().back();

    CHECK_EQUAL(a, first);
    CHECK_EQUAL(b, second);
}

TEST(Product, productOfThreeSymbols)
    /* No simplification necessary: a*b*c. */
{
    const BasePtr res = Product::create(a, b, c);

    CHECK(res->isProduct());
}

TEST(Product, twoProducts)
    /* (2*a*c*e)*(3*b*d*e) = 6*a*b*c*d*e^2. */
{
    BasePtrList::const_iterator it;
    BasePtr expected[6];
    BasePtr res;
    int i;

    res = Product::create(Product::create(two, a, c, e), Product::create(three, b, d, e));

    CHECK(res->isProduct());

    expected[0] = six;
    expected[1] = a;
    expected[2] = b;
    expected[3] = c;
    expected[4] = d;
    expected[5] = Power::create(e, two);

    for (i = 0, it = cbegin(res->operands()); it != cend(res->operands()); ++it, ++i)
        CHECK_EQUAL(expected[i], *it);
}

TEST(Product, equalProductBasesToPower)
    /* (a*b^2)*(a*b^2) = (a^2)*(b^4). */
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr product = Product::create(a, bSquare);
    BasePtr res = Product::create(product, product);
    BasePtrList factors;

    CHECK(res->isProduct());

    factors = res->operands();
    CHECK_EQUAL(2, factors.size());

    res = factors.front();
    CHECK(res->isPower());
    CHECK_EQUAL(two, res->exp());

    res = factors.back();
    CHECK(res->isPower());
    CHECK_EQUAL(four, res->exp());
}

TEST(Product, threeNumericFactors)
    /* 2*3*2 = 12. */
{
    const BasePtr res = Product::create(two, three, two);

    CHECK_EQUAL(Numeric::create(12), res);
}

TEST(Product, productOfSymbolAndProduct)
    /* (a*c)*b = a*b*c. */
{
    const BasePtr product1 = Product::create(a, c);
    const BasePtr res = Product::create(product1, b);
    const BasePtrList& fac(res->operands());
    auto it = cbegin(fac);

    CHECK(res->isProduct());

    CHECK_EQUAL(3, fac.size());

    CHECK_EQUAL(a, *it);
    CHECK_EQUAL(b, *++it);
    CHECK_EQUAL(c, *++it);
}

TEST(Product, productOfProductAndSymbol)
    /* a*(e*b) = a*b*e. */
{
    const BasePtr p2 = Product::create(e, b);
    const BasePtr res = Product::create(a, p2);
    const BasePtrList& fac(res->operands());
    auto it = cbegin(fac);

    CHECK(res->isProduct());
    CHECK_EQUAL(3, fac.size());

    CHECK_EQUAL(a, *it);
    CHECK_EQUAL(b, *++it);
    CHECK_EQUAL(e, *++it);
}

TEST(Product, productOfThreeProducts)
    /* (a*c)*(b*e)*(a*d) = a^2*b*c*d*e. */
{
    const BasePtr p1 = Product::create(a, c);
    const BasePtr p2 = Product::create(b, e);
    const BasePtr p3 = Product::create(a, d);
    const BasePtr res = Product::create(p1, p2, p3);
    auto it = cbegin(res->operands());

    CHECK(res->isProduct());

    CHECK_EQUAL(two, (*it)->exp());
    CHECK_EQUAL(a, (*it)->base());
    CHECK_EQUAL(b, *++it);
    CHECK_EQUAL(c, *++it);
    CHECK_EQUAL(d, *++it);
    CHECK_EQUAL(e, *++it);
}

TEST(Product, rearrangeSymbolAndPi)
    /* b*Pi*a = Pi*a*b. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Product::create(b, pi, a);
    const BasePtrList expected{pi, a, b};

    CHECK_EQUAL(expected, res->operands());
}

TEST(Product, contractNumericsAndPi)
    /* 2*Pi*3*Pi*4 = 24*Pi^2. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Product::create({ two, pi, three, pi, four });

    CHECK_EQUAL(Numeric::create(24), res->operands().front());
    CHECK_EQUAL(Power::create(pi, two), res->operands().back());
}

TEST(Product, piDividedByPi)
    /* Pi/Pi = 1. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Product::create(pi, Power::oneOver(pi));

    CHECK_EQUAL(one, res);
}

TEST(Product, orderingOfFunctionsNumbersAndSymbols)
    /* A correct ordering is 2*sqrt(2)*a*atan(a)*b*c*cos(1)*cos(d)*cos(d*e)*sin(a). */
{
    const BasePtr factors[] = { two, sqrtTwo, a, Trigonometric::createAtan(a), b, c,
        Trigonometric::createCos(one), Trigonometric::createCos(d),
        Trigonometric::createCos(Product::create(d, e)), Trigonometric::createSin(a) };
    BasePtrList expected;
    const BasePtr product = Product::create({ factors[1], factors[0], factors[5], factors[8],
            factors[9], factors[3], factors[6], factors[2], factors[7], factors[4] });

    for (size_t i = 0; i < sizeof(factors)/sizeof(factors[0]); ++i)
        expected.push_back(factors[i]);

    CHECK_EQUAL(expected, product->operands());
}

TEST(Product, productOfEqualFunctionsEqualArguments)
    /* sin(a)*2*sin(a)*a = 2*a*sin(a)^2. */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr pow = Power::create(sin, two);
    const BasePtr res = Product::create(sin, two, sin, a);
    const BasePtrList expected{ two, a, pow };

    CHECK_EQUAL(expected, res->operands());
}

TEST(Product, numericEvaluation)
    /* -2*sqrt(3)*4^(1/3)*Pi . */
{
    const Number expected(-2.0*std::sqrt(3.0)*std::pow(4.0, 1.0/3.0)*M_PI);
    const BasePtr res = Product::minus(two, sqrtThree, Power::create(four, oneThird),
            Constant::createPi());

    CHECK(res->isNumericallyEvaluable());
    CHECK_EQUAL(expected, res->numericEval());
}

TEST(Product, equalNonNumericBaseNumExp)
    /* a^b*a^c = a^(b*c) = a^d with b, c and d being numerics. */
{
    const BasePtr exp1 = Numeric::create(Int("2039840928430928094328094"));
    const BasePtr exp2 = Numeric::create(Int("2093840928430998324"));
    const BasePtr pow1 = Power::create(a, exp1);
    const BasePtr pow2 = Power::create(a, exp2);
    const BasePtr expected = Power::create(a, Sum::create(exp1, exp2));
    const BasePtr res = Product::create(pow1, pow2);

    CHECK_EQUAL(expected, res);
}

TEST(Product, numPowEqualDenomInPosExp)
    /* 2^(2/3)*3^(1/3) = 12^(1/3). */
{
    const BasePtr expected = Power::create(Numeric::create(12), oneThird);
    const BasePtr f1 = Power::create(two, Numeric::create(2, 3));
    const BasePtr f2 = Power::create(three, oneThird);
    const BasePtr result = Product::create(f1, f2);

    CHECK_EQUAL(expected, result);
}

TEST(Product, numPowEqualDenomInNegExp)
    /* (6/7)^(-2/5)*3^(-1/5) = (7/6)^(2/5)*(1/3)^(1/5) = (49/108)^(1/5). */
{
    const BasePtr f1 = Power::create(Numeric::create(6, 7), Numeric::create(-2, 5));
    const BasePtr f2 = Power::create(three, Numeric::create(-1, 5));
    const BasePtr result = Product::create(f1, f2);
    const BasePtr expected = Power::create(Numeric::create(49, 108), Numeric::create(1, 5));

    CHECK_EQUAL(expected, result);
}

TEST(Product, numPowEqualDenomInMixedSignExp)
    /* 7^(1/6)*5^(-5/6) = (7/3125)^(1/6) if the NumPowerSimpl limit for prime factorization is high
     * enough. */
{
    const BasePtr posExp = Numeric::create(1, 6);
    const BasePtr f1 = Power::create(seven, posExp);
    const BasePtr f2 = Power::create(five, Product::minus(five, posExp));
    const BasePtr expected = Power::create(Numeric::create(7, 3125), posExp);
    BasePtr result;

    options::setMaxPrimeResolution(100);

    result = Product::create(f1, f2);

    CHECK(result->isProduct());
    CHECK_EQUAL(f1, result->operands().front());
    CHECK_EQUAL(f2, result->operands().back());

    options::setMaxPrimeResolution(3125);

    result = Product::create(f1, f2);

    CHECK_EQUAL(expected, result);
}

TEST(Product, numPowEqualDenomExpToProduct)
    /* 6^(1/3)*3^(2/3) = 3*2^(1/3). */
{
    const BasePtr& exp = Numeric::third();
    const BasePtr pow1 = Power::create(six, exp);
    const BasePtr pow2 = Power::create(three, Numeric::create(2, 3));
    const BasePtr result = Product::create(pow1, pow2);
    const BasePtr expected = Product::create(three, Power::create(two, exp));

    CHECK_EQUAL(expected, result);
}

TEST(Product, numPowEqualDenomExpToProductInLargeList)
    /* 2*6^(1/3)*sqrt(5)*12*sqrt(17)*3^(2/3) = . */
{
    const BasePtr expected = Product::create(Numeric::create(72), Power::sqrt(Numeric::create(85)),
                Power::create(two, oneThird));
    BasePtrList fac;
    BasePtr res;

    fac.push_back(two);
    fac.push_back(Power::create(six, Numeric::third()));
    fac.push_back(Power::sqrt(five));
    fac.push_back(Numeric::create(12));
    fac.push_back(Power::sqrt(Numeric::create(17)));
    fac.push_back(Power::create(three, Numeric::create(2, 3)));

    res = Product::create(fac);

    CHECK_EQUAL(expected, res);
}

TEST(Product, numPowEqualDenomExpNotContracted)
    /* 7^(1/23)*17^(21/23) isn't contracted due to the too large result of 17^21. */
{
    const BasePtr pow1 = Power::create(seven, Numeric::create(1, 23));
    const BasePtr pow2 = Power::create(Numeric::create(17), Numeric::create(21, 23));
    BasePtr result;

    result = Product::create(pow1, pow2);

    CHECK(result->isProduct());
    CHECK_EQUAL(pow1, result->operands().front());
    CHECK_EQUAL(pow2, result->operands().back());
}

TEST(Product, sineOverCosineSameArgument)
    /* Sin(2 + a + b)/cos(2 + a + b) = tan(2 + a + b). */
{
    const BasePtr arg = Sum::create(a, b, two);
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr res = Product::create(sin, Power::oneOver(cos));
    const BasePtr expected = Trigonometric::createTan(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Product, sineOverCosineDifferentArgument)
    /* No simplification of sin(3*a*b)/cos(a + b). */
{
    const BasePtr sin = Trigonometric::createSin(Product::create(three, a, b));
    const BasePtr cos = Trigonometric::createCos(Sum::create(a, b));
    const BasePtr res = Product::create(sin, Power::oneOver(cos));

    CHECK(res->isProduct());
    CHECK_EQUAL(Power::oneOver(cos), res->operands().front());
    CHECK_EQUAL(sin, res->operands().back());
}

TEST(Product, cosineOverSineSameArgument)
    /* Cos(10)/sin(10) = tan(10). */
{
    const BasePtr sin = Trigonometric::createSin(ten);
    const BasePtr cos = Trigonometric::createCos(ten);
    const BasePtr res = Product::create(cos, Power::oneOver(sin));
    const BasePtr expected = Power::oneOver(Trigonometric::createTan(ten));

    CHECK_EQUAL(expected, res);
}

TEST(Product, cosineOverSineDifferentArgument)
    /* No simplification of cos(7)/sin(4). */
{
    const BasePtr sin = Trigonometric::createSin(four);
    const BasePtr cos = Trigonometric::createCos(seven);
    const BasePtr res = Product::create(cos, Power::oneOver(sin));

    CHECK(res->isProduct());
    CHECK_EQUAL(cos, res->operands().front());
    CHECK_EQUAL(Power::oneOver(sin), res->operands().back());
}

TEST(Product, sinOverTanSameNumericalArgument)
    /* Sin(7^(2/3))/tan(7^(2/3)) = cos(7^(2/3)). */
{
    const BasePtr arg = Power::create(seven, Numeric::create(2, 3));
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr tan = Trigonometric::createTan(arg);
    BasePtr res;

    res = Product::create(sin, Power::oneOver(tan));

    CHECK_EQUAL(cos, res);
}

TEST(Product, tanTimesCosSameArgument)
    /* Tan(a + b)*cos(a + b) = sin(a + b). */
{
    const BasePtr arg = Sum::create(a, b);
    const BasePtr tan = Trigonometric::createTan(arg);
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr res = Product::create(tan, cos);

    CHECK_EQUAL(sin, res);
}

TEST(Product, tanTimesCosDifferentArgument)
    /* No simplification of tan(sqrt(2))*cos(a). */
{
    const BasePtr tan = Trigonometric::createTan(Power::sqrt(two));
    const BasePtr cos = Trigonometric::createCos(a);
    const BasePtr res = Product::create(tan, cos);

    CHECK(res->isProduct());
    CHECK_EQUAL(cos, res->operands().front());
    CHECK_EQUAL(tan, res->operands().back());
}

TEST(Product, tanTimesCosWithNumExpSameArgument)
    /* Tan(1)^(2/3)*cos(1)^(-1/2) = sin(1)^(2/3)*cos(1)(-7/6). */
{
    const BasePtr tan = Trigonometric::createTan(one);
    const BasePtr cos = Trigonometric::createCos(one);
    const BasePtr sin = Trigonometric::createSin(one);
    const BasePtr res = Product::create(Power::create(tan, Numeric::create(2, 3)),
                Power::create(cos, Numeric::create(-1, 2)));
    const BasePtr expected = Product::create(Power::create(sin, Numeric::create(2, 3)),
                Power::create(cos, Numeric::create(-7, 6)));

    CHECK_EQUAL(expected, res);
}

TEST(Product, sinOverCosineWithExpTimesCosine)
    /* Sin(a)^(5/7)*cos(a)^(-2/3)*cos(a) = sin(a)^(5/7)*cos(a)^(1/3). This simplification is not
     * related to the handling of sin(...)/cos(...) = tan(...), but due to the equal base cos(a). */
{
    const BasePtr sinPow = Power::create(Trigonometric::createSin(a), Numeric::create(5, 7));
    const BasePtr cos = Trigonometric::createCos(a);
    const BasePtr cosPow = Power::create(cos, Numeric::create(-2, 3));
    const BasePtr res = Product::create(sinPow, cosPow, cos);
    const BasePtr expected = Product::create(sinPow, Power::create(cos, Numeric::third()));

    CHECK_EQUAL(expected, res);
}

TEST(Product, sinSquareOverCosSameArg)
    /* Sin^2(a)/cos(a) = sin(a)*tan(a). */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr sinSquare = Product::create(sin, sin);
    const BasePtr res = Product::create(sinSquare, Power::oneOver(Trigonometric::createCos(a)));
    const BasePtr expected = Product::create(sin, Trigonometric::createTan(a));

    CHECK_EQUAL(expected, res);
}

TEST(Product, sqrtTanTimesCosPositiveSameArg)
    /* Sqrt(tan(1))*cos(1) = sqrt(sin(1))*sqrt(cos(1)). */
{
    const BasePtr sin = Trigonometric::createSin(one);
    const BasePtr cos = Trigonometric::createCos(one);
    const BasePtr tan = Trigonometric::createTan(one);
    const BasePtr res = Product::create(Power::sqrt(tan), cos);
    const BasePtr expected = Power::sqrt(Product::create(sin, cos));

    CHECK_EQUAL(expected, res);
}

TEST(Product, sqrtTanTimesCosSameArg)
    /* Sqrt(tan(a))*cos(a) can't be simplified because sin(a), cos(a) are neither < 0 nor > 0. */
{
    const BasePtr cos = Trigonometric::createCos(a);
    const BasePtr sqrtTan = Power::sqrt(Trigonometric::createTan(a));
    const BasePtr res = Product::create(sqrtTan, cos);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(cos, res->operands().front());
    CHECK_EQUAL(sqrtTan, res->operands().back());
}

TEST(Product, mixedTrigonometricFunctions01)
    /* Sin(a)*cos(b)*tan(a)^(1/3)*sin(b)^(-2)*cos(a)^3 =
     * 1/tan(b)*sin(b)^(-1)*sin(a)^(4/3)*cos(a)^(8/3). for a = 1/2, b = 2. */
{
    const BasePtr sinA = Trigonometric::createSin(half);
    const BasePtr cosB = Trigonometric::createCos(two);
    const BasePtr tanA = Trigonometric::createTan(half);
    const BasePtr sinB = Trigonometric::createSin(two);
    const BasePtr cosA = Trigonometric::createCos(half);
    BasePtrList factors;
    BasePtr expected;
    BasePtr res;

    factors.push_back(sinA);
    factors.push_back(cosB);
    factors.push_back(Power::create(tanA, Numeric::third()));
    factors.push_back(Power::create(sinB, Numeric::create(-2)));
    factors.push_back(Power::create(cosA, three));

    res = Product::create(factors);

    factors.clear();
    factors.push_back(Power::oneOver(Trigonometric::createTan(two)));
    factors.push_back(Power::oneOver(sinB));
    factors.push_back(Power::create(sinA, Numeric::create(4, 3)));
    factors.push_back(Power::create(cosA, Numeric::create(8, 3)));

    expected = Product::create(factors);

    CHECK_EQUAL(expected, res);
}

TEST(Product, mixedTrigonometricFunctions02)
    /* (1/Cos(a/b))*tan(a/b)*cos(b/a)*sin(a/b)/sin(b/a) = tan^2(a/b)/tan(b/a). */
{
    const BasePtr aB = Product::create(a, Power::oneOver(b));
    const BasePtr tanAb = Trigonometric::createTan(aB);
    const BasePtr bA = Power::oneOver(aB);
    BasePtrList factors;
    BasePtr expected;
    BasePtr res;

    factors.push_back(Power::oneOver(Trigonometric::createCos(aB)));
    factors.push_back(tanAb);
    factors.push_back(Trigonometric::createCos(bA));
    factors.push_back(Trigonometric::createSin(aB));
    factors.push_back(Power::oneOver(Trigonometric::createSin(bA)));

    res = Product::create(factors);

    factors.clear();
    factors.push_back(Power::create(tanAb, two));
    factors.push_back(Power::oneOver(Trigonometric::createTan(bA)));

    expected = Product::create(factors);

    CHECK_EQUAL(expected, res);
}

TEST(Product, noSumOfExpUnclearBasePowers)
    /* sqrt(a)*sqrt(a) can't be simplified to a because a could be < 0. */
{
    const BasePtr sqrtA = Power::sqrt(a);
    const BasePtr res = Product::create(sqrtA, sqrtA);

    CHECK(res->isProduct());
    CHECK_EQUAL(2, res->operands().size());
    CHECK_EQUAL(sqrtA, res->operands().front());
    CHECK_EQUAL(sqrtA, res->operands().back());
}

TEST(Product, sumOfExpUnclearBasePowers)
    /* a^2*a^(3/4) = a^(11/4). */
{
    const BasePtr res = Product::create(Power::create(a, two), Power::create(a,
                Numeric::create(3, 4)));
    const BasePtr expected = Power::create(a, Numeric::create(11, 4));

    CHECK_EQUAL(expected, res);
}
