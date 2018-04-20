
#include <cmath>
#include <limits>
#include "fixtures.h"
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

struct ProductFixture : public AbcFixture {
    const Int primeResolutionLimit = options::getMaxPrimeResolution();
    const BasePtr& half = Numeric::half();
    const BasePtr minusOneHalf = Numeric::create(-1, 2);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sqrtSix = Power::sqrt(six);
    const BasePtr& oneThird = Numeric::third();
    const BasePtr& oneFourth = Numeric::fourth();

    ~ProductFixture()
    {
        options::setMaxPrimeResolution(primeResolutionLimit);
    }
};

BOOST_FIXTURE_TEST_SUITE(TestProduct, ProductFixture)

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Product");
    const BasePtr ptr = Product::create(a, b);

    BOOST_CHECK_EQUAL(expected, ptr->typeStr());
}

BOOST_AUTO_TEST_CASE(undefined)
    /* If one factor is undefined, a product is, too. */
{
    const BasePtr u = Undefined::create();
    const BasePtr p = Product::create(u, a);

    BOOST_TEST(p->isUndefined());
}

BOOST_AUTO_TEST_CASE(zero)
    /* 0*(a*b) = 0. */
{
    const BasePtr p1 = Product::create(a, b);
    const BasePtr p2 = Product::create(p1, Numeric::zero());

    BOOST_TEST(p2->isZero());
}

BOOST_AUTO_TEST_CASE(twoNumericFactors)
    /* 2*3 = 6. */
{
    const BasePtr p = Product::create(two, three);

    BOOST_TEST(p->isNumeric());
    BOOST_CHECK_EQUAL(6, p->numericEval());
}

BOOST_AUTO_TEST_CASE(twoNumericFactorsResultingInOne)
    /* (1/2)*2 = 1. */
{
    const BasePtr p = Product::create(two, Numeric::half());

    BOOST_TEST(p->isNumeric());
    BOOST_CHECK_EQUAL(1, p->numericEval());
}

BOOST_AUTO_TEST_CASE(numberTimesNumericPower)
    /* 2*sqrt(2) is not altered. */
{
    const BasePtr p = Product::create(two, sqrtTwo);

    BOOST_TEST(p->isProduct());
    BOOST_TEST(p->isConst());

    BOOST_CHECK_EQUAL(2, p->operands().front()->numericEval());
    BOOST_TEST(p->operands().back()->isNumericPower());
}

BOOST_AUTO_TEST_CASE(numberTimesResolvableNumPow)
    /* (-2)*sqrt(9) = -6. */
{
    const BasePtr res = Product::create(Numeric::create(-2), Power::sqrt(nine));
    const BasePtr expected = Numeric::create(-6);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(resolvableNumPowNegativeBase)
    /* 2*(-1)*2^(-1/2) = -sqrt(2). */
{
    const BasePtr expected = Product::minus(sqrtTwo);
    const BasePtr res = Product::create({ two,
            Product::minus(Power::create(two, Numeric::create(-1, 2))) });

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(resolvableNumPowNegativeNumericFactor)
    /* (-2)*2^(-1/2) = -sqrt(2). */
{
    const BasePtr expected = Product::minus(sqrtTwo);
    const BasePtr res = Product::create({ Numeric::create(-2),
            Power::create(two, Numeric::create(-1, 2)) });

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(resolvableNumPowNegativeNumericFractionFactor)
    /* b*(-1/2)*sqrt(2)*a = -2^(-1/2)*a*b. */
{
    const BasePtr expected = Product::minus(a, b, Power::create(two, Numeric::create(-1, 2)));
    const BasePtr res = Product::create({ b, Numeric::create(-1, 2), sqrtTwo, a });

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(resolvableNumPowMixedWithSymbol)
    /* 2*a*(-1)*2^(-1/2) = -sqrt(2)*a. */
{
    const BasePtr expected = Product::minus(sqrtTwo, a);
    const BasePtr res = Product::create({ two, a,
            Product::minus(Power::create(two, Numeric::create(-1, 2))) });

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(constantTimesNumeric)
    /* Pi*2 = 2*Pi. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr result = Product::create(pi, two);

    BOOST_TEST(result->isProduct());
    BOOST_CHECK_EQUAL(two, result->operands().front());
    BOOST_CHECK_EQUAL(pi, result->operands().back());
}

BOOST_AUTO_TEST_CASE(numericPowerTimesNumber)
    /* sqrt(2)*2 = 2*sqrt(2). */
{
    const BasePtr p = Product::create(sqrtTwo, two);

    BOOST_TEST(p->isProduct());
    BOOST_TEST(p->isConst());

    BOOST_CHECK_EQUAL(two, p->operands().front());
    BOOST_CHECK_EQUAL(sqrtTwo, p->operands().back());
}

BOOST_AUTO_TEST_CASE(intTimesNumericPower)
    /* -1/4*sqrt(2) = -1/2*2^(-1/2). */
{
    const BasePtr res = Product::create(Numeric::create(-1, 4), sqrtTwo);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(Numeric::create(-1, 2), res->operands().front());
    BOOST_CHECK_EQUAL(Power::create(two, Numeric::create(-1, 2)), res->operands().back());
}

BOOST_AUTO_TEST_CASE(intTimesNumericPowerTimesOne)
    /* -1/4*sqrt(2)*1 = -1/2*2^(-1/2). */
{
    const BasePtr res = Product::create(Numeric::create(-1, 4), sqrtTwo, one);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(Numeric::create(-1, 2), res->operands().front());
    BOOST_CHECK_EQUAL(Power::create(two, Numeric::create(-1, 2)), res->operands().back());
}

BOOST_AUTO_TEST_CASE(numberAndNumericPowerEqualBase)
    /* 2*2^(2/3) isn't altered. */
{
    const BasePtr pow = Power::create(two, Numeric::create(2, 3));
    const BasePtr res = Product::create(two, pow);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(two, res->operands().front());
    BOOST_CHECK_EQUAL(pow, res->operands().back());
}

BOOST_AUTO_TEST_CASE(numericPowersEqualBase)
    /* 2^(1/2)*2^(1/3) = 2^(2/3) */
{
    const BasePtr twoToTheThird = Power::create(two, Numeric::third());
    const BasePtr p = Product::create(sqrtTwo, twoToTheThird);

    BOOST_TEST(p->isNumericPower());
    BOOST_CHECK_CLOSE(std::pow(2, 5.0/6.0), p->numericEval().toDouble(), 1.e-10);
}

BOOST_AUTO_TEST_CASE(numericPowersDifferentExpSameBase)
    /* 2^(1/3)*2(1/4) = 2^(7/12). */
{
    const Number expectedExp(7, 12);
    const BasePtr np1 = Power::create(two, oneThird);
    const BasePtr np2 = Power::create(two, oneFourth);
    const BasePtr res = Product::create(np1, np2);

    BOOST_TEST(res->isPower());
    BOOST_TEST(res->isNumericPower());
    BOOST_CHECK_EQUAL(2, res->base()->numericEval());

    BOOST_CHECK_EQUAL(expectedExp, res->exp()->numericEval());
}

BOOST_AUTO_TEST_CASE(numericPowersDifferentExpDifferentBase)
    /* No simplification of 2^(1/3)*3^(1/4). */
{
    const BasePtr np1 = Power::create(two, oneThird);
    const BasePtr np2 = Power::create(three, oneFourth);
    const BasePtr res = Product::create(np1, np2);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(np1, res->operands().front());
    BOOST_CHECK_EQUAL(np2, res->operands().back());
}

BOOST_AUTO_TEST_CASE(reductionOfDifferentNumericPowers)
    /* (3/2)*sqrt(2/3) = sqrt(3/2). */
{
    const BasePtr threeOverTwo = Numeric::create(3, 2);
    const BasePtr origPow = Power::sqrt(Numeric::create(2, 3));
    const BasePtr res = Product::create(threeOverTwo, origPow);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(half, res->exp());
    BOOST_CHECK_EQUAL(threeOverTwo, res->base());
}

BOOST_AUTO_TEST_CASE(reductionOfSameNumericBaseDifferentExp)
    /* 2/sqrt(2) = sqrt(2). */
{
    const BasePtr res = Product::create(two, Power::create(two, minusOneHalf));

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(half, res->exp());
    BOOST_CHECK_EQUAL(two, res->base());
}

BOOST_AUTO_TEST_CASE(reductionOfSameNumericBaseDifferentExponent)
    /* sqrt(2)/2 = 1/sqrt(2). */
{
    const BasePtr expected = Power::create(two, minusOneHalf);
    const BasePtr res = Product::create(sqrtTwo, half);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(minusOneHalf, res->exp());
    BOOST_CHECK_EQUAL(two, res->base());
}

BOOST_AUTO_TEST_CASE(mergeNumericsOfSimplifiedNumericPower)
    /* (2*sqrt(3))*sqrt(6) = 6*sqrt(2). */
{
    const BasePtr f1 = Product::create(two, sqrtThree);
    const BasePtr f2 = Power::sqrt(six);
    BasePtr res;

    res = Product::create(f1, f2);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(six, res->operands().front());
    BOOST_CHECK_EQUAL(sqrtTwo, res->operands().back());
}

BOOST_AUTO_TEST_CASE(differentNumericPowerProduct)
    /* sqrt(2)*sqrt(3) = sqrt(6). */
{
    const BasePtr p = Product::create(sqrtTwo, sqrtThree);

    BOOST_TEST(p->isNumericPower());
    BOOST_CHECK_EQUAL(six, p->base());
    BOOST_CHECK_EQUAL(Number(1, 2), p->exp()->numericEval());
}

BOOST_AUTO_TEST_CASE(simpleNumericPowersDifferentExpSign)
    /* 17^(1/3)*3^(-1/3) = (17/3)^(1/3). */
{
    const BasePtr expected = Power::create(Numeric::create(17, 3), oneThird);
    const BasePtr pow1 = Power::create(Numeric::create(17), oneThird);
    const BasePtr pow2 = Power::create(three, Numeric::create(-1, 3));
    BasePtr res;

    res = Product::create(pow1, pow2);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(numericPowersDifferentExpSign)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(contractionOfNumerics)
    /* (1/4)*(2/5)*11*(3/7) = 33/70. */
{
    const BasePtr res = Product::create({ Numeric::fourth(), Numeric::create(2, 5),
            Numeric::create(11), Numeric::create(3, 7) });

    BOOST_CHECK_EQUAL(Numeric::create(33, 70), res);
}

BOOST_AUTO_TEST_CASE(contractionOfConstPowers)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(preSortingOfConstPowers)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(orderingOfConstPowers)
    /* (3^(1/3)*5^(1/5))*2^(1/5) = 3^(1/3)*10^(1/5). */
{
    const BasePtr oneFifth = Numeric::create(1, 5);
    const BasePtr f1 = Power::create(three, oneThird);
    const BasePtr f2 = Power::create(five, oneFifth);
    const BasePtr f3 = Power::create(two, oneFifth);
    const BasePtr p1 = Product::create(f1, f2);
    const BasePtr res = Product::create(p1, f3);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(f1, res->operands().front());
    BOOST_CHECK_EQUAL(Power::create(ten, oneFifth), res->operands().back());
}

BOOST_AUTO_TEST_CASE(expansionOfConstPowerAndSum)
    /* (1 + sqrt(2))*sqrt(3) = sqrt(3) + sqrt(6). */
{
    const BasePtr sum = Sum::create(one, sqrtTwo);
    const BasePtr res = Product::create(sum, sqrtThree);

    BOOST_TEST(res->isSum());
    BOOST_CHECK_EQUAL(sqrtThree, res->operands().front());
    BOOST_CHECK_EQUAL(sqrtSix, res->operands().back());
}

BOOST_AUTO_TEST_CASE(expansionOfNumberConstPowerAndSum)
    /* 2*sqrt(2)*(a + b) = 2*sqrt(2)*a + 2*sqrt(2)*b. */
{
    const BasePtr fac = Product::create(two, sqrtTwo);
    const BasePtr res = Product::create(fac, Sum::create(a, b));
    const BasePtr expected = Sum::create(Product::create(fac, a), Product::create(fac, b));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(expansionOfConstProductAndSum)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(contractNumSqrt)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(contractNumPowWithEqualExp)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(symbolTimesOne)
    /* a*1 = a. */
{
    const BasePtr res = Product::create(a, one);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(oneTimesSymbol)
    /* 1*a = a. */
{
    const BasePtr res = Product::create(one, a);

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(multipleOnesTimesSum)
    /* 1*(a + b)*1*1*1 = a + b. */
{
    const BasePtr aPlusB = Sum::create(a, b);
    const BasePtr res = Product::create({ one, aPlusB, one, one, one });

    BOOST_CHECK_EQUAL(aPlusB, res);
}

BOOST_AUTO_TEST_CASE(minusOneTimesNegProduct)
    /* (-1)*(-a) = a. */
{
    const BasePtr res = Product::create(Numeric::mOne(), Product::minus(a));

    BOOST_CHECK_EQUAL(a, res);
}

BOOST_AUTO_TEST_CASE(equalSymbolBasesToPower)
    /* a*a = a^2. */
{
    const BasePtr res = Product::create(a, a);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(2, res->exp()->numericEval());
}

BOOST_AUTO_TEST_CASE(equalPowerBasesToPower)
    /* b^2*b^3 = b^5. */
{
    const BasePtr pow1 = Power::create(b, two);
    const BasePtr pow2 = Power::create(b, three);
    const BasePtr res = Product::create(pow1, pow2);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(5, res->exp()->numericEval());
}

BOOST_AUTO_TEST_CASE(equalPowerBasesToOne)
    /* a^(-1)*a = 1. */
{
    const BasePtr pow = Power::oneOver(a);
    const BasePtr res = Product::create(pow, a);

    BOOST_TEST(res->isOne());
}

BOOST_AUTO_TEST_CASE(equalSumBasesToPower)
    /* (a + b)^3*(a + b) = (a + b)^4. */
{
    const BasePtr sum = Sum::create(a, b);
    const BasePtr pow1 = Power::create(sum, three);
    const BasePtr res = Product::create(pow1, sum);

    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(4, res->exp()->numericEval());
    BOOST_CHECK_EQUAL(sum, res->base());
}

BOOST_AUTO_TEST_CASE(rearrangeTwoSymbols)
    /* Simple ordering: b*a = a*b. */
{
    const BasePtr res = Product::create(b, a);
    BasePtr first;
    BasePtr second;

    BOOST_TEST(res->isProduct());

    first = res->operands().front();
    second = res->operands().back();

    BOOST_CHECK_EQUAL(a, first);
    BOOST_CHECK_EQUAL(b, second);
}

BOOST_AUTO_TEST_CASE(productOfThreeSymbols)
    /* No simplification necessary: a*b*c. */
{
    const BasePtr res = Product::create(a, b, c);

    BOOST_TEST(res->isProduct());
}

BOOST_AUTO_TEST_CASE(twoProducts)
    /* (2*a*c*e)*(3*b*d*e) = 6*a*b*c*d*e^2. */
{
    BasePtrList::const_iterator it;
    BasePtr expected[6];
    BasePtr res;
    int i;

    res = Product::create(Product::create(two, a, c, e), Product::create(three, b, d, e));

    BOOST_TEST(res->isProduct());

    expected[0] = six;
    expected[1] = a;
    expected[2] = b;
    expected[3] = c;
    expected[4] = d;
    expected[5] = Power::create(e, two);

    for (i = 0, it = cbegin(res->operands()); it != cend(res->operands()); ++it, ++i)
        BOOST_CHECK_EQUAL(expected[i], *it);
}

BOOST_AUTO_TEST_CASE(equalProductBasesToPower)
    /* (a*b^2)*(a*b^2) = (a^2)*(b^4). */
{
    const BasePtr bSquare = Power::create(b, two);
    const BasePtr product = Product::create(a, bSquare);
    BasePtr res = Product::create(product, product);
    BasePtrList factors;

    BOOST_TEST(res->isProduct());

    factors = res->operands();
    BOOST_CHECK_EQUAL(2, factors.size());

    res = factors.front();
    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(two, res->exp());

    res = factors.back();
    BOOST_TEST(res->isPower());
    BOOST_CHECK_EQUAL(four, res->exp());
}

BOOST_AUTO_TEST_CASE(threeNumericFactors)
    /* 2*3*2 = 12. */
{
    const BasePtr res = Product::create(two, three, two);

    BOOST_CHECK_EQUAL(Numeric::create(12), res);
}

BOOST_AUTO_TEST_CASE(productOfSymbolAndProduct)
    /* (a*c)*b = a*b*c. */
{
    const BasePtr product1 = Product::create(a, c);
    const BasePtr res = Product::create(product1, b);
    const BasePtrList& fac(res->operands());
    auto it = cbegin(fac);

    BOOST_TEST(res->isProduct());

    BOOST_CHECK_EQUAL(3, fac.size());

    BOOST_CHECK_EQUAL(a, *it);
    BOOST_CHECK_EQUAL(b, *++it);
    BOOST_CHECK_EQUAL(c, *++it);
}

BOOST_AUTO_TEST_CASE(productOfProductAndSymbol)
    /* a*(e*b) = a*b*e. */
{
    const BasePtr p2 = Product::create(e, b);
    const BasePtr res = Product::create(a, p2);
    const BasePtrList& fac(res->operands());
    auto it = cbegin(fac);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(3, fac.size());

    BOOST_CHECK_EQUAL(a, *it);
    BOOST_CHECK_EQUAL(b, *++it);
    BOOST_CHECK_EQUAL(e, *++it);
}

BOOST_AUTO_TEST_CASE(productOfThreeProducts)
    /* (a*c)*(b*e)*(a*d) = a^2*b*c*d*e. */
{
    const BasePtr p1 = Product::create(a, c);
    const BasePtr p2 = Product::create(b, e);
    const BasePtr p3 = Product::create(a, d);
    const BasePtr res = Product::create(p1, p2, p3);
    auto it = cbegin(res->operands());

    BOOST_TEST(res->isProduct());

    BOOST_CHECK_EQUAL(two, (*it)->exp());
    BOOST_CHECK_EQUAL(a, (*it)->base());
    BOOST_CHECK_EQUAL(b, *++it);
    BOOST_CHECK_EQUAL(c, *++it);
    BOOST_CHECK_EQUAL(d, *++it);
    BOOST_CHECK_EQUAL(e, *++it);
}

BOOST_AUTO_TEST_CASE(rearrangeSymbolAndPi)
    /* b*Pi*a = Pi*a*b. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Product::create(b, pi, a);
    const BasePtrList expected{pi, a, b};

    BOOST_CHECK_EQUAL(expected, res->operands());
}

BOOST_AUTO_TEST_CASE(contractNumericsAndPi)
    /* 2*Pi*3*Pi*4 = 24*Pi^2. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Product::create({ two, pi, three, pi, four });

    BOOST_CHECK_EQUAL(Numeric::create(24), res->operands().front());
    BOOST_CHECK_EQUAL(Power::create(pi, two), res->operands().back());
}

BOOST_AUTO_TEST_CASE(piDividedByPi)
    /* Pi/Pi = 1. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr res = Product::create(pi, Power::oneOver(pi));

    BOOST_CHECK_EQUAL(one, res);
}

BOOST_AUTO_TEST_CASE(orderingOfFunctionsNumbersAndSymbols)
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

    BOOST_CHECK_EQUAL(expected, product->operands());
}

BOOST_AUTO_TEST_CASE(productOfEqualFunctionsEqualArguments)
    /* sin(a)*2*sin(a)*a = 2*a*sin(a)^2. */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr pow = Power::create(sin, two);
    const BasePtr res = Product::create(sin, two, sin, a);
    const BasePtrList expected{ two, a, pow };

    BOOST_CHECK_EQUAL(expected, res->operands());
}

BOOST_AUTO_TEST_CASE(numericEvaluation)
    /* -2*sqrt(3)*4^(1/3)*Pi . */
{
    const Number expected(-2.0*std::sqrt(3.0)*std::pow(4.0, 1.0/3.0)*M_PI);
    const BasePtr res = Product::minus(two, sqrtThree, Power::create(four, oneThird),
            Constant::createPi());

    BOOST_TEST(res->isNumericallyEvaluable());
    BOOST_CHECK_EQUAL(expected, res->numericEval());
}

BOOST_AUTO_TEST_CASE(equalNonNumericBaseNumExp)
    /* a^b*a^c = a^(b*c) = a^d with b, c and d being numerics. */
{
    const BasePtr exp1 = Numeric::create(Int("2039840928430928094328094"));
    const BasePtr exp2 = Numeric::create(Int("2093840928430998324"));
    const BasePtr pow1 = Power::create(a, exp1);
    const BasePtr pow2 = Power::create(a, exp2);
    const BasePtr expected = Power::create(a, Sum::create(exp1, exp2));
    const BasePtr res = Product::create(pow1, pow2);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(numPowEqualDenomInPosExp)
    /* 2^(2/3)*3^(1/3) = 12^(1/3). */
{
    const BasePtr expected = Power::create(Numeric::create(12), oneThird);
    const BasePtr f1 = Power::create(two, Numeric::create(2, 3));
    const BasePtr f2 = Power::create(three, oneThird);
    const BasePtr result = Product::create(f1, f2);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(numPowEqualDenomInNegExp)
    /* (6/7)^(-2/5)*3^(-1/5) = (7/6)^(2/5)*(1/3)^(1/5) = (49/108)^(1/5). */
{
    const BasePtr f1 = Power::create(Numeric::create(6, 7), Numeric::create(-2, 5));
    const BasePtr f2 = Power::create(three, Numeric::create(-1, 5));
    const BasePtr result = Product::create(f1, f2);
    const BasePtr expected = Power::create(Numeric::create(49, 108), Numeric::create(1, 5));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(numPowEqualDenomInMixedSignExp)
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

    BOOST_TEST(result->isProduct());
    BOOST_CHECK_EQUAL(f1, result->operands().front());
    BOOST_CHECK_EQUAL(f2, result->operands().back());

    options::setMaxPrimeResolution(3125);

    result = Product::create(f1, f2);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(numPowEqualDenomExpToProduct)
    /* 6^(1/3)*3^(2/3) = 3*2^(1/3). */
{
    const BasePtr& exp = Numeric::third();
    const BasePtr pow1 = Power::create(six, exp);
    const BasePtr pow2 = Power::create(three, Numeric::create(2, 3));
    const BasePtr result = Product::create(pow1, pow2);
    const BasePtr expected = Product::create(three, Power::create(two, exp));

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(numPowEqualDenomExpToProductInLargeList)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(numPowEqualDenomExpNotContracted)
    /* 7^(1/23)*17^(21/23) isn't contracted due to the too large result of 17^21. */
{
    const BasePtr pow1 = Power::create(seven, Numeric::create(1, 23));
    const BasePtr pow2 = Power::create(Numeric::create(17), Numeric::create(21, 23));
    BasePtr result;

    result = Product::create(pow1, pow2);

    BOOST_TEST(result->isProduct());
    BOOST_CHECK_EQUAL(pow1, result->operands().front());
    BOOST_CHECK_EQUAL(pow2, result->operands().back());
}

BOOST_AUTO_TEST_CASE(sineOverCosineSameArgument)
    /* Sin(2 + a + b)/cos(2 + a + b) = tan(2 + a + b). */
{
    const BasePtr arg = Sum::create(a, b, two);
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr res = Product::create(sin, Power::oneOver(cos));
    const BasePtr expected = Trigonometric::createTan(arg);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sineOverCosineDifferentArgument)
    /* No simplification of sin(3*a*b)/cos(a + b). */
{
    const BasePtr sin = Trigonometric::createSin(Product::create(three, a, b));
    const BasePtr cos = Trigonometric::createCos(Sum::create(a, b));
    const BasePtr res = Product::create(sin, Power::oneOver(cos));

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(Power::oneOver(cos), res->operands().front());
    BOOST_CHECK_EQUAL(sin, res->operands().back());
}

BOOST_AUTO_TEST_CASE(cosineOverSineSameArgument)
    /* Cos(10)/sin(10) = tan(10). */
{
    const BasePtr sin = Trigonometric::createSin(ten);
    const BasePtr cos = Trigonometric::createCos(ten);
    const BasePtr res = Product::create(cos, Power::oneOver(sin));
    const BasePtr expected = Power::oneOver(Trigonometric::createTan(ten));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(cosineOverSineDifferentArgument)
    /* No simplification of cos(7)/sin(4). */
{
    const BasePtr sin = Trigonometric::createSin(four);
    const BasePtr cos = Trigonometric::createCos(seven);
    const BasePtr res = Product::create(cos, Power::oneOver(sin));

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(cos, res->operands().front());
    BOOST_CHECK_EQUAL(Power::oneOver(sin), res->operands().back());
}

BOOST_AUTO_TEST_CASE(sinOverTanSameNumericalArgument)
    /* Sin(7^(2/3))/tan(7^(2/3)) = cos(7^(2/3)). */
{
    const BasePtr arg = Power::create(seven, Numeric::create(2, 3));
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr tan = Trigonometric::createTan(arg);
    BasePtr res;

    res = Product::create(sin, Power::oneOver(tan));

    BOOST_CHECK_EQUAL(cos, res);
}

BOOST_AUTO_TEST_CASE(tanTimesCosSameArgument)
    /* Tan(a + b)*cos(a + b) = sin(a + b). */
{
    const BasePtr arg = Sum::create(a, b);
    const BasePtr tan = Trigonometric::createTan(arg);
    const BasePtr cos = Trigonometric::createCos(arg);
    const BasePtr sin = Trigonometric::createSin(arg);
    const BasePtr res = Product::create(tan, cos);

    BOOST_CHECK_EQUAL(sin, res);
}

BOOST_AUTO_TEST_CASE(tanTimesCosDifferentArgument)
    /* No simplification of tan(sqrt(2))*cos(a). */
{
    const BasePtr tan = Trigonometric::createTan(Power::sqrt(two));
    const BasePtr cos = Trigonometric::createCos(a);
    const BasePtr res = Product::create(tan, cos);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(cos, res->operands().front());
    BOOST_CHECK_EQUAL(tan, res->operands().back());
}

BOOST_AUTO_TEST_CASE(tanTimesCosWithNumExpSameArgument)
    /* Tan(1)^(2/3)*cos(1)^(-1/2) = sin(1)^(2/3)*cos(1)(-7/6). */
{
    const BasePtr tan = Trigonometric::createTan(one);
    const BasePtr cos = Trigonometric::createCos(one);
    const BasePtr sin = Trigonometric::createSin(one);
    const BasePtr res = Product::create(Power::create(tan, Numeric::create(2, 3)),
                Power::create(cos, Numeric::create(-1, 2)));
    const BasePtr expected = Product::create(Power::create(sin, Numeric::create(2, 3)),
                Power::create(cos, Numeric::create(-7, 6)));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sinOverCosineWithExpTimesCosine)
    /* Sin(a)^(5/7)*cos(a)^(-2/3)*cos(a) = sin(a)^(5/7)*cos(a)^(1/3). This simplification is not
     * related to the handling of sin(...)/cos(...) = tan(...), but due to the equal base cos(a). */
{
    const BasePtr sinPow = Power::create(Trigonometric::createSin(a), Numeric::create(5, 7));
    const BasePtr cos = Trigonometric::createCos(a);
    const BasePtr cosPow = Power::create(cos, Numeric::create(-2, 3));
    const BasePtr res = Product::create(sinPow, cosPow, cos);
    const BasePtr expected = Product::create(sinPow, Power::create(cos, Numeric::third()));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sinSquareOverCosSameArg)
    /* Sin^2(a)/cos(a) = sin(a)*tan(a). */
{
    const BasePtr sin = Trigonometric::createSin(a);
    const BasePtr sinSquare = Product::create(sin, sin);
    const BasePtr res = Product::create(sinSquare, Power::oneOver(Trigonometric::createCos(a)));
    const BasePtr expected = Product::create(sin, Trigonometric::createTan(a));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sqrtTanTimesCosPositiveSameArg)
    /* Sqrt(tan(1))*cos(1) = sqrt(sin(1))*sqrt(cos(1)). */
{
    const BasePtr sin = Trigonometric::createSin(one);
    const BasePtr cos = Trigonometric::createCos(one);
    const BasePtr tan = Trigonometric::createTan(one);
    const BasePtr res = Product::create(Power::sqrt(tan), cos);
    const BasePtr expected = Power::sqrt(Product::create(sin, cos));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(sqrtTanTimesCosSameArg)
    /* Sqrt(tan(a))*cos(a) can't be simplified because sin(a), cos(a) are neither < 0 nor > 0. */
{
    const BasePtr cos = Trigonometric::createCos(a);
    const BasePtr sqrtTan = Power::sqrt(Trigonometric::createTan(a));
    const BasePtr res = Product::create(sqrtTan, cos);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(cos, res->operands().front());
    BOOST_CHECK_EQUAL(sqrtTan, res->operands().back());
}

BOOST_AUTO_TEST_CASE(mixedTrigonometricFunctions01)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(mixedTrigonometricFunctions02)
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

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(noSumOfExpUnclearBasePowers)
    /* sqrt(a)*sqrt(a) can't be simplified to a because a could be < 0. */
{
    const BasePtr sqrtA = Power::sqrt(a);
    const BasePtr res = Product::create(sqrtA, sqrtA);

    BOOST_TEST(res->isProduct());
    BOOST_CHECK_EQUAL(2, res->operands().size());
    BOOST_CHECK_EQUAL(sqrtA, res->operands().front());
    BOOST_CHECK_EQUAL(sqrtA, res->operands().back());
}

BOOST_AUTO_TEST_CASE(sumOfExpUnclearBasePowers)
    /* a^2*a^(3/4) = a^(11/4). */
{
    const BasePtr res = Product::create(Power::create(a, two), Power::create(a,
                Numeric::create(3, 4)));
    const BasePtr expected = Power::create(a, Numeric::create(11, 4));

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_SUITE_END()
