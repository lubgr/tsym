
#include "fixtures.h"
#include "number.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestUndefined, AbcFixture)

BOOST_AUTO_TEST_CASE(numericalEvaluation)
{
    BOOST_TEST(!undefined->isNumericallyEvaluable());
}

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Undefined");

    BOOST_CHECK_EQUAL(expected, undefined->typeStr());
}

BOOST_AUTO_TEST_CASE(numericTerm)
{
    BOOST_CHECK_EQUAL(one, undefined->numericTerm());
}

BOOST_AUTO_TEST_CASE(nonNumericTerm)
{
    BOOST_TEST(undefined->nonNumericTerm()->isUndefined());
}

BOOST_AUTO_TEST_CASE(constTerm)
{
    BOOST_CHECK_EQUAL(one, undefined->constTerm());
}

BOOST_AUTO_TEST_CASE(nonConstTerm)
{
    BOOST_TEST(undefined->nonConstTerm()->isUndefined());
}

BOOST_AUTO_TEST_CASE(numericEvaluability)
{
    BOOST_TEST(!undefined->isNumericallyEvaluable());
}

BOOST_AUTO_TEST_CASE(numericEvaluation)
{
    BOOST_CHECK_THROW(undefined->numericEval().value(), std::bad_optional_access);
}

BOOST_AUTO_TEST_CASE(equalityOtherUndefined, noLogs())
{
    const BasePtr otherUndefined = Undefined::create();

    BOOST_TEST(!undefined->isEqual(*undefined));
    BOOST_TEST(!undefined->isEqual(*otherUndefined));

    BOOST_TEST(undefined->isDifferent(*undefined));
    BOOST_TEST(undefined->isDifferent(*otherUndefined));
}

BOOST_AUTO_TEST_SUITE_END()
