
#include <cmath>
#include "constant.h"
#include "number.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

struct ConstantFixture : public AbcFixture {
    const BasePtr& e = Constant::createE();
};

BOOST_FIXTURE_TEST_SUITE(TestConstant, ConstantFixture)

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Constant");

    BOOST_CHECK_EQUAL(expected, pi->typeStr());
    BOOST_CHECK_EQUAL(expected, e->typeStr());
}

BOOST_AUTO_TEST_CASE(constRequest)
    /* A Constant is treated like a Symbol, so it isn't considered const. */
{
    BOOST_TEST(!pi->isConst());
    BOOST_TEST(!e->isConst());
}

BOOST_AUTO_TEST_CASE(typeRequest)
{
    BOOST_TEST(pi->isConstant());
    BOOST_TEST(e->isConstant());
}

BOOST_AUTO_TEST_CASE(getNameFromPi)
{
    const Name expected("pi");
    Name name;

    name = pi->name();

    BOOST_CHECK_EQUAL(expected, name);
}

BOOST_AUTO_TEST_CASE(getNameFromE)
{
    const Name expected("e");
    Name name;

    name = e->name();

    BOOST_CHECK_EQUAL(expected, name);
}

BOOST_AUTO_TEST_CASE(trivialEquality)
{
    BOOST_TEST(pi->isEqual(pi));
    BOOST_TEST(e->isEqual(e));
}

BOOST_AUTO_TEST_CASE(numericEvaluationPi)
{
    const double TOL = 1.e-12;

    BOOST_CHECK_CLOSE(M_PI, pi->numericEval().toDouble(), TOL);

    /* Should work with the internal tolerance of the Number class, too. */
    BOOST_CHECK_EQUAL(M_PI, pi->numericEval());
}

BOOST_AUTO_TEST_CASE(numericEvaluationE)
{
    const double TOL = 1.e-12;

    BOOST_CHECK_CLOSE(M_E, e->numericEval().toDouble(), TOL);

    /* Should work with the internal tolerance of the Number class, too. */
    BOOST_CHECK_EQUAL(M_E, e->numericEval());
}

BOOST_AUTO_TEST_CASE(numericTerm)
{
    BOOST_CHECK_EQUAL(one, pi->numericTerm());
    BOOST_CHECK_EQUAL(one, e->numericTerm());
}

BOOST_AUTO_TEST_CASE(nonNumericTerm)
{
    BOOST_CHECK_EQUAL(pi, pi->nonNumericTerm());
    BOOST_CHECK_EQUAL(e, e->nonNumericTerm());
}

BOOST_AUTO_TEST_CASE(constTerm)
{
    BOOST_CHECK_EQUAL(one, pi->constTerm());
    BOOST_CHECK_EQUAL(one, e->constTerm());
}

BOOST_AUTO_TEST_CASE(nonConstTerm)
{
    BOOST_CHECK_EQUAL(pi, pi->nonConstTerm());
    BOOST_CHECK_EQUAL(e, e->nonConstTerm());
}

BOOST_AUTO_TEST_SUITE_END()
