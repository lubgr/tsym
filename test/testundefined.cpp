
#include "cpputest.h"
#include "abc.h"
#include "undefined.h"

using namespace tsym;

TEST_GROUP(Undefined)
{
    BasePtr undefined;

    void setup()
    {
        undefined = Undefined::create();
    }
};

TEST(Undefined, numericalEvaluation)
{
    CHECK(!undefined->isNumericallyEvaluable());
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
    CHECK_EQUAL(undefined, undefined->nonNumericTerm());
}

TEST(Undefined, constTerm)
{
    CHECK_EQUAL(one, undefined->constTerm());
}

TEST(Undefined, nonConstTerm)
{
    CHECK_EQUAL(undefined, undefined->nonConstTerm());
}

TEST(Undefined, numericEvaluability)
{
    CHECK(!undefined->isNumericallyEvaluable());
}

TEST(Undefined, numericEvaluation)
{
    CHECK(undefined->numericEval().isUndefined());
}

TEST(Undefined, equalityOtherUndefined)
{
    const BasePtr otherUndefined = Undefined::create();

    CHECK(!undefined->isEqual(undefined));
    CHECK(!undefined->isEqual(otherUndefined));

    CHECK(undefined->isDifferent(undefined));
    CHECK(undefined->isDifferent(otherUndefined));
}
