
#include "numeric.h"
#include "abc.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Numeric) {};

TEST(Numeric, creationByNumber)
{
    const Number half(1, 2);
    const BasePtr ptr = Numeric::create(half);

    CHECK(ptr->isNumeric());
    CHECK_EQUAL(half, ptr->numericEval());
}

TEST(Numeric, creationByIntegerClass)
{
    const Int num = 400;
    const Int denom = 200;
    const BasePtr res = Numeric::create(num, denom);

    CHECK_EQUAL(two, res);
}

TEST(Numeric, creationByUndefinedNumber)
{
    const Number n(Number::createUndefined());
    BasePtr ptr;

    ptr = Numeric::create(n);

    CHECK(ptr->isUndefined());
}

TEST(Numeric, creationByInt)
{
    const BasePtr ptr = Numeric::one();

    CHECK(ptr->isNumeric());
    CHECK_EQUAL(1, ptr->numericEval());
}

TEST(Numeric, creationByFraction)
{
    const BasePtr res = Numeric::fourth();

    CHECK(res->isNumeric());
    CHECK_EQUAL(Number(1, 4), res->numericEval());
}

TEST(Numeric, creationByDouble)
{
    const double value = 1.23456789;
    const BasePtr res = Numeric::create(value);

    CHECK(res->isNumeric());
    CHECK_EQUAL(value, res->numericEval());
}

TEST(Numeric, numericEvaluation)
{
    const Number expected(2, 3);
    const BasePtr frac = Numeric::create(2, 3);

    CHECK(frac->isNumericallyEvaluable());
    CHECK_EQUAL(expected, frac->numericEval());
}

TEST(Numeric, undefined)
{
    BasePtr res;

    disableLog();
    res = Numeric::create(1, 0);
    enableLog();

    CHECK(res->isUndefined());
}

TEST(Numeric, typeString)
{
    const std::string expected("Numeric");
    const BasePtr ptr = Numeric::create(1.23456);

    CHECK_EQUAL(expected, ptr->typeStr());
}

TEST(Numeric, uselessNameRequest)
{
    const BasePtr n = Numeric::create(2, 3);
    const Name empty;

    CHECK_EQUAL(empty, n->name());
}
