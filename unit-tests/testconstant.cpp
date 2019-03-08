
#include <cmath>
#include "basefct.h"
#include "constant.h"
#include "fixtures.h"
#include "number.h"
#include "tsymtests.h"

using namespace tsym;

struct ConstantFixture : public AbcFixture {
    const BasePtr& euler = Constant::createE();
};

BOOST_FIXTURE_TEST_SUITE(TestConstant, ConstantFixture)

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Constant");

    BOOST_CHECK_EQUAL(expected, pi->typeStr());
    BOOST_CHECK_EQUAL(expected, euler->typeStr());
}

BOOST_AUTO_TEST_CASE(constRequest)
/* A Constant is treated like a Symbol, so it isn't considered const. */
{
    BOOST_TEST(!pi->isConst());
    BOOST_TEST(!euler->isConst());
}

BOOST_AUTO_TEST_CASE(typeRequest)
{
    BOOST_TEST(isConstant(*pi));
    BOOST_TEST(isConstant(*euler));
}

BOOST_AUTO_TEST_CASE(getNameFromPi)
{
    const Name expected{"pi"};
    Name name;

    name = pi->name();

    BOOST_CHECK_EQUAL(expected, name);
}

BOOST_AUTO_TEST_CASE(getNameFromE)
{
    const Name expected{"e"};
    Name name;

    name = euler->name();

    BOOST_CHECK_EQUAL(expected, name);
}

BOOST_AUTO_TEST_CASE(trivialEquality)
{
    BOOST_TEST(pi->isEqual(*pi));
    BOOST_TEST(euler->isEqual(*euler));
}

BOOST_AUTO_TEST_CASE(numericEvaluationPi)
{
    const double TOL = 1.e-12;

    BOOST_CHECK_CLOSE(M_PI, pi->numericEval().value().toDouble(), TOL);

    /* Should work with the internal tolerance of the Number class, too. */
    BOOST_CHECK_EQUAL(M_PI, pi->numericEval().value());
}

BOOST_AUTO_TEST_CASE(numericEvaluationE)
{
    const double TOL = 1.e-12;

    BOOST_CHECK_CLOSE(M_E, euler->numericEval()->toDouble(), TOL);

    /* Should work with the internal tolerance of the Number class, too. */
    BOOST_CHECK_EQUAL(M_E, *euler->numericEval());
}

BOOST_AUTO_TEST_CASE(numericTerm)
{
    BOOST_CHECK_EQUAL(one, pi->numericTerm());
    BOOST_CHECK_EQUAL(one, euler->numericTerm());
}

BOOST_AUTO_TEST_CASE(nonNumericTerm)
{
    BOOST_CHECK_EQUAL(pi, pi->nonNumericTerm());
    BOOST_CHECK_EQUAL(euler, euler->nonNumericTerm());
}

BOOST_AUTO_TEST_CASE(constTerm)
{
    BOOST_CHECK_EQUAL(one, pi->constTerm());
    BOOST_CHECK_EQUAL(one, euler->constTerm());
}

BOOST_AUTO_TEST_CASE(nonConstTerm)
{
    BOOST_CHECK_EQUAL(pi, pi->nonConstTerm());
    BOOST_CHECK_EQUAL(euler, euler->nonConstTerm());
}

BOOST_AUTO_TEST_SUITE_END()
