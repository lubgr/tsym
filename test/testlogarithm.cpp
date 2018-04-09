
#include <cmath>
#include "logarithm.h"
#include "constant.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "numeric.h"
#include "undefined.h"
#include "symbol.h"
#include "abc.h"
#include "tsymtests.h"

using namespace tsym;

struct LogarithmFixture {
    const BasePtr e = Constant::createE();
};

BOOST_FIXTURE_TEST_SUITE(TestLogarithm, LogarithmFixture)

BOOST_AUTO_TEST_CASE(logOfZero)
{
    BasePtr result;

    disableLog();
    result = Logarithm::create(zero);
    enableLog();

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfOne)
{
    const BasePtr result = Logarithm::create(one);

    BOOST_TEST(result->isZero());
}

BOOST_AUTO_TEST_CASE(logOfUndefinedArg)
{
    BasePtr result;

    disableLog();
    result = Logarithm::create(Undefined::create());
    enableLog();

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfE)
{
    const BasePtr arg = e;
    const BasePtr result = Logarithm::create(arg);

    BOOST_TEST(result->isOne());
}

BOOST_AUTO_TEST_CASE(logOfPi)
{
    const BasePtr arg = Constant::createPi();
    const BasePtr result = Logarithm::create(arg);

    BOOST_TEST(result->isFunction());
    BOOST_CHECK_EQUAL(arg, result->operands().front());
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

BOOST_AUTO_TEST_CASE(logOfNegativeArgument)
{
    const BasePtr aPos = Symbol::createPositive("a");
    BasePtr log;

    disableLog();
    log = Logarithm::create(Product::minus(aPos));
    enableLog();

    BOOST_TEST(log->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfInvalidNumericallyEvaluableEqualZero)
{
    const BasePtr arg = Sum::create(e, Numeric::create(-M_E));
    BasePtr result;

    disableLog();
    result = Logarithm::create(arg);
    enableLog();

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfInvalidNumericallyEvaluableLessThanZero)
{
    const BasePtr arg = Sum::create(Product::minus(Constant::createPi()), two);
    BasePtr result;

    disableLog();
    result = Logarithm::create(arg);
    enableLog();

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

BOOST_AUTO_TEST_CASE(logOfPowerWithNegEulerBasePosNumericExp)
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr arg = Product::minus(Power::create(e, exp));
    BasePtr result;

    disableLog();
    result = Logarithm::create(arg);
    enableLog();

    BOOST_TEST(result->isUndefined());
}

BOOST_AUTO_TEST_CASE(logOfPowerWithEulerBaseNegNumericExp)
{
    const BasePtr exp = Numeric::create(-123);
    const BasePtr arg = Power::create(e, exp);
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(exp, result);
}

BOOST_AUTO_TEST_CASE(logOfPowerWithEulerBasePosNumericExp)
{
    const BasePtr exp = Numeric::create(3, 10);
    const BasePtr arg = Power::create(e, exp);
    const BasePtr result = Logarithm::create(arg);

    BOOST_CHECK_EQUAL(exp, result);
}

BOOST_AUTO_TEST_CASE(logOfPowerWithEulerBaseArbitraryExp)
{
    const BasePtr exp = Sum::create(a, b, Product::create(seven, c, d));
    const BasePtr arg = Power::create(e, exp);
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
    const BasePtr arg = Sum::create(two, e);
    const BasePtr log = Logarithm::create(arg);
    const double expected = std::log(2.0 + M_E);

    BOOST_CHECK_EQUAL(expected, log->numericEval());
}

BOOST_AUTO_TEST_CASE(invalidNumericEval)
{
    const BasePtr arg = Product::create(two, a);
    const BasePtr log = Logarithm::create(arg);

    BOOST_CHECK_THROW(log->numericEval(), std::logic_error);
}

BOOST_AUTO_TEST_SUITE_END()
