
#include <cmath>
#include "basefct.h"
#include "constant.h"
#include "fixtures.h"
#include "logarithm.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

struct LogarithmFixture : public AbcFixture {
    const BasePtr& euler = Constant::createE();
};

BOOST_FIXTURE_TEST_SUITE(TestLogarithm, LogarithmFixture)

BOOST_AUTO_TEST_CASE(logOfZero, noLogs())
{
    BasePtr result;

    result = Logarithm::create(zero);

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfOne)
{
    const BasePtr result = Logarithm::create(one);

    BOOST_TEST(isZero(*result));
}

BOOST_AUTO_TEST_CASE(logOfUndefinedArg, noLogs())
{
    BasePtr result;

    result = Logarithm::create(undefined);

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfE)
{
    const BasePtr arg = euler;
    const BasePtr result = Logarithm::create(arg);

    BOOST_TEST(isOne(*result));
}

BOOST_AUTO_TEST_CASE(logOfPi)
{
    const BasePtr result = Logarithm::create(pi);

    BOOST_TEST(result->isFunction());
    BOOST_CHECK_EQUAL(pi, result->operands().front());
}

BOOST_AUTO_TEST_CASE(logOfRationalNumericArg)
{
    const BasePtr arg = Numeric::create(5, 7);
    const BasePtr result = Logarithm::create(arg);

    BOOST_TEST(result->isFunction());
    BOOST_CHECK_EQUAL(arg, result->operands().front());
}

BOOST_AUTO_TEST_CASE(logOfDoubleNumericArg)
{
    const BasePtr expected = Numeric::create(std::log(9.87654321));
    const BasePtr arg = Numeric::create(9.87654321);
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(logOfValidNumericallyEvaluable)
{
    const BasePtr arg = Product::create(two, Power::sqrt(two));
    const BasePtr result = Logarithm::create(arg);

    BOOST_TEST(result->isFunction());
    BOOST_CHECK_EQUAL(arg, result->operands().front());
}

BOOST_AUTO_TEST_CASE(logOfNegativeArgument, noLogs())
{
    const BasePtr aPos = Symbol::createPositive("a");
    BasePtr log;

    log = Logarithm::create(Product::minus(aPos));

    BOOST_TEST(log->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfInvalidNumericallyEvaluableEqualZero, noLogs())
{
    const BasePtr arg = Sum::create(euler, Numeric::create(-M_E));
    BasePtr result;

    result = Logarithm::create(arg);

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfInvalidNumericallyEvaluableLessThanZero, noLogs())
{
    const BasePtr arg = Sum::create(Product::minus(pi), two);
    BasePtr result;

    result = Logarithm::create(arg);

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfExactDoubleEtoThe5)
{
    const BasePtr arg = Numeric::create(std::pow(M_E, 5.0));
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(five, result);
}

BOOST_AUTO_TEST_CASE(logOfSymbolicSum)
{
    const BasePtr arg = Sum::create(a, b);
    const BasePtr result = Logarithm::create(arg);

    BOOST_TEST(result->isFunction());
    BOOST_CHECK_EQUAL(arg, result->operands().front());
}

BOOST_AUTO_TEST_CASE(logOfPowerWithNegEulerBasePosNumericExp, noLogs())
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr arg = Product::minus(Power::create(euler, exp));
    BasePtr result;

    result = Logarithm::create(arg);

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfPowerWithEulerBaseNegNumericExp)
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr arg = Power::create(euler, exp);
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(exp, result);
}

BOOST_AUTO_TEST_CASE(logOfPowerWithEulerBasePosNumericExp)
{
    const BasePtr exp = Numeric::create(3, 10);
    const BasePtr arg = Power::create(euler, exp);
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(exp, result);
}

BOOST_AUTO_TEST_CASE(logOfPowerWithEulerBaseArbitraryExp)
{
    const BasePtr exp = Sum::create(a, b, Product::create(seven, c, d));
    const BasePtr arg = Power::create(euler, exp);
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(exp, result);
}

BOOST_AUTO_TEST_CASE(logOfPower)
/* log(a^2) = 2*log(a). */
{
    const BasePtr expected = Product::create(two, Logarithm::create(a));
    const BasePtr arg = Power::create(a, two);
    const BasePtr res = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(expected, res);
}

BOOST_AUTO_TEST_CASE(logOfSymbolicPower)
{
    const BasePtr expected = Product::create(two, b, Logarithm::create(a));
    const BasePtr arg = Power::create(a, Product::create(two, b));
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(expected, result);
}

BOOST_AUTO_TEST_CASE(numericEval)
{
    const BasePtr arg = Sum::create(two, euler);
    const BasePtr log = Logarithm::create(arg);
    const double expected = std::log(2.0 + M_E);

    BOOST_CHECK_EQUAL(expected, log->numericEval().value());
}

BOOST_AUTO_TEST_CASE(invalidNumericEval)
{
    const BasePtr arg = Product::create(two, a);
    const BasePtr log = Logarithm::create(arg);
    const auto num = log->numericEval();

    BOOST_CHECK(!num);
}

BOOST_AUTO_TEST_SUITE_END()
