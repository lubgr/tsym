
#include <cmath>
#include "cpputest.h"
#include "abc.h"
#include "constant.h"

using namespace tsym;

TEST_GROUP(Constant)
{
    BasePtr pi;
    BasePtr e;

    void setup()
    {
        pi = Constant::createPi();
        e = Constant::createE();
    }
};

TEST(Constant, typeString)
{
    const std::string expected("Constant");

    CHECK_EQUAL(expected, pi->typeStr());
    CHECK_EQUAL(expected, e->typeStr());
}

TEST(Constant, constRequest)
    /* A Constant is treated like a Symbol, so it isn't considered const. */
{
    CHECK(!pi->isConst());
    CHECK(!e->isConst());
}

TEST(Constant, typeRequest)
{
    CHECK(pi->isConstant());
    CHECK(e->isConstant());
}

TEST(Constant, getNameFromPi)
{
    const Name expected("pi");
    Name name;

    name = pi->name();

    CHECK_EQUAL(expected, name);
}

TEST(Constant, getNameFromE)
{
    const Name expected("e");
    Name name;

    name = e->name();

    CHECK_EQUAL(expected, name);
}

TEST(Constant, trivialEquality)
{
    CHECK(pi->isEqual(pi));
    CHECK(e->isEqual(e));
}

TEST(Constant, numericEvaluationPi)
{
    const double TOL = 1.e-12;

    DOUBLES_EQUAL(M_PI, pi->numericEval().toDouble(), TOL);

    /* Should work with the internal tolerance of the Number class, too. */
    CHECK_EQUAL(M_PI, pi->numericEval());
}

TEST(Constant, numericEvaluationE)
{
    const double TOL = 1.e-12;

    DOUBLES_EQUAL(M_E, e->numericEval().toDouble(), TOL);

    /* Should work with the internal tolerance of the Number class, too. */
    CHECK_EQUAL(M_E, e->numericEval());
}

TEST(Constant, numericTerm)
{
    CHECK_EQUAL(one, pi->numericTerm());
    CHECK_EQUAL(one, e->numericTerm());
}

TEST(Constant, nonNumericTerm)
{
    CHECK_EQUAL(pi, pi->nonNumericTerm());
    CHECK_EQUAL(e, e->nonNumericTerm());
}

TEST(Constant, constTerm)
{
    CHECK_EQUAL(one, pi->constTerm());
    CHECK_EQUAL(one, e->constTerm());
}

TEST(Constant, nonConstTerm)
{
    CHECK_EQUAL(pi, pi->nonConstTerm());
    CHECK_EQUAL(e, e->nonConstTerm());
}
