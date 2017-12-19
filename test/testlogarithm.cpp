
#include <cmath>
#include "logarithm.h"
#include "constant.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "abc.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Logarithm)
{
    const BasePtr e = Constant::createE();
};

TEST(Logarithm, logOfZero)
{
    BasePtr result;

    disableLog();
    result = Logarithm::create(zero);
    enableLog();

    CHECK(result->isUndefined());
}

TEST(Logarithm, logOfOne)
{
    const BasePtr result = Logarithm::create(one);

    CHECK(result->isZero());
}

TEST(Logarithm, logOfUndefinedArg)
{
    BasePtr result;

    disableLog();
    result = Logarithm::create(Undefined::create());
    enableLog();

    CHECK(result->isUndefined());
}

TEST(Logarithm, logOfE)
{
    const BasePtr arg = e;
    const BasePtr result = Logarithm::create(arg);

    CHECK(result->isOne());
}

TEST(Logarithm, logOfPi)
{
    const BasePtr arg = Constant::createPi();
    const BasePtr result = Logarithm::create(arg);

    CHECK(result->isFunction());
    CHECK_EQUAL(arg, result->operands().front());
}

TEST(Logarithm, logOfRationalNumericArg)
{
    const BasePtr arg = Numeric::create(5, 7);
    const BasePtr result = Logarithm::create(arg);

    CHECK(result->isFunction());
    CHECK_EQUAL(arg, result->operands().front());
}

TEST(Logarithm, logOfDoubleNumericArg)
{
    const BasePtr expected = Numeric::create(std::log(9.87654321));
    const BasePtr arg = Numeric::create(9.87654321);
    const BasePtr result = Logarithm::create(arg);

    CHECK_EQUAL(expected, result);
}

TEST(Logarithm, logOfValidNumericallyEvaluable)
{
    const BasePtr arg = Product::create(two, Power::sqrt(two));
    const BasePtr result = Logarithm::create(arg);

    CHECK(result->isFunction());
    CHECK_EQUAL(arg, result->operands().front());
}

TEST(Logarithm, logOfNegativeArgument)
{
    const BasePtr aPos = Symbol::createPositive("a");
    BasePtr log;

    disableLog();
    log = Logarithm::create(Product::minus(aPos));
    enableLog();

    CHECK(log->isUndefined());
}

TEST(Logarithm, logOfInvalidNumericallyEvaluableEqualZero)
{
    const BasePtr arg = Sum::create(e, Numeric::create(-M_E));
    BasePtr result;

    disableLog();
    result = Logarithm::create(arg);
    enableLog();

    CHECK(result->isUndefined());
}

TEST(Logarithm, logOfInvalidNumericallyEvaluableLessThanZero)
{
    const BasePtr arg = Sum::create(Product::minus(Constant::createPi()), two);
    BasePtr result;

    disableLog();
    result = Logarithm::create(arg);
    enableLog();

    CHECK(result->isUndefined());
}

TEST(Logarithm, logOfExactDoubleEtoThe5)
{
    const BasePtr arg = Numeric::create(std::pow(M_E, 5.0));
    const BasePtr result = Logarithm::create(arg);

    CHECK_EQUAL(five, result);
}

TEST(Logarithm, logOfSymbolicSum)
{
    const BasePtr arg = Sum::create(a, b);
    const BasePtr result = Logarithm::create(arg);

    CHECK(result->isFunction());
    CHECK_EQUAL(arg, result->operands().front());
}

TEST(Logarithm, logOfPowerWithNegEulerBasePosNumericExp)
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr arg = Product::minus(Power::create(e, exp));
    BasePtr result;

    disableLog();
    result = Logarithm::create(arg);
    enableLog();

    CHECK(result->isUndefined());
}

TEST(Logarithm, logOfPowerWithEulerBaseNegNumericExp)
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr arg = Power::create(e, exp);
    const BasePtr result = Logarithm::create(arg);

    CHECK_EQUAL(exp, result);
}

TEST(Logarithm, logOfPowerWithEulerBasePosNumericExp)
{
    const BasePtr exp = Numeric::create(3, 10);
    const BasePtr arg = Power::create(e, exp);
    const BasePtr result = Logarithm::create(arg);

    CHECK_EQUAL(exp, result);
}

TEST(Logarithm, logOfPowerWithEulerBaseArbitraryExp)
{
    const BasePtr exp = Sum::create(a, b, Product::create(seven, c, d));
    const BasePtr arg = Power::create(e, exp);
    const BasePtr result = Logarithm::create(arg);

    CHECK_EQUAL(exp, result);
}

TEST(Logarithm, logOfPower)
    /* log(a^2) = 2*log(a). */
{
    const BasePtr expected = Product::create(two, Logarithm::create(a));
    const BasePtr arg = Power::create(a, two);
    const BasePtr res = Logarithm::create(arg);

    CHECK_EQUAL(expected, res);
}

TEST(Logarithm, logOfSymbolicPower)
{
    const BasePtr expected = Product::create(two, b, Logarithm::create(a));
    const BasePtr arg = Power::create(a, Product::create(two, b));
    const BasePtr result = Logarithm::create(arg);

    CHECK_EQUAL(expected, result);
}

TEST(Logarithm, numericEval)
{
    const BasePtr arg = Sum::create(two, e);
    const BasePtr log = Logarithm::create(arg);
    const double expected = std::log(2.0 + M_E);

    CHECK_EQUAL(expected, log->numericEval());
}

TEST(Logarithm, invalidNumericEval)
{
    const BasePtr arg = Product::create(two, a);
    const BasePtr log = Logarithm::create(arg);
    const Number result = log->numericEval();

    CHECK(result.isUndefined());
}
