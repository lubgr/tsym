
#include "abc.h"
#include "undefined.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Undefined)
{
    const BasePtr undefined = Undefined::create();
};

TEST(Undefined, numericalEvaluation)
{
    CHECK_FALSE(undefined->isNumericallyEvaluable());
}

TEST(Undefined, typeString)
{
    const std::string expected("Undefined");

    CHECK_EQUAL(expected, undefined->typeStr());
}

TEST(Undefined, numericTerm)
{
    CHECK_EQUAL(one, undefined->numericTerm());
}

TEST(Undefined, nonNumericTerm)
{
    CHECK(undefined->nonNumericTerm()->isUndefined());
}

TEST(Undefined, constTerm)
{
    CHECK_EQUAL(one, undefined->constTerm());
}

TEST(Undefined, nonConstTerm)
{
    CHECK(undefined->nonConstTerm()->isUndefined());
}

TEST(Undefined, numericEvaluability)
{
    CHECK_FALSE(undefined->isNumericallyEvaluable());
}

TEST(Undefined, numericEvaluation)
{
    CHECK(undefined->numericEval().isUndefined());
}

TEST(Undefined, equalityOtherUndefined)
{
    const BasePtr otherUndefined = Undefined::create();

    disableLog();

    CHECK_FALSE(undefined->isEqual(undefined));
    CHECK_FALSE(undefined->isEqual(otherUndefined));

    CHECK(undefined->isDifferent(undefined));
    CHECK(undefined->isDifferent(otherUndefined));

    enableLog();
}
