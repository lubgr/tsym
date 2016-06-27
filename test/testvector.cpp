
#include <sstream>
#include "vector.h"
#include "cpputest.h"

using namespace tsym;

TEST_GROUP(Vector)
{
    Var a;
    Var b;
    Var c;
    Vector abcVec;

    void setup()
    {
        a = Var("a");
        b = Var("b");
        c = Var("c");

        abcVec = Vector(3);
        abcVec(0) = a;
        abcVec(1) = b;
        abcVec(2) = c;
    }

    void checkAbcVector(const Vector& vec)
    {
        CHECK_EQUAL(3, vec.size());

        CHECK_EQUAL(a, vec(0));
        CHECK_EQUAL(b, vec(1));
        CHECK_EQUAL(c, vec(2));
    }
};

TEST(Vector, emptyVector)
{
    const Vector vec;

    CHECK_EQUAL(0, vec.size());
    CHECK(vec.isZero());
    CHECK(vec == Vector());
}

TEST(Vector, initialState)
{
    const Vector vec(4);

    CHECK_EQUAL(4, vec.size());
    CHECK(vec.isZero());
}

TEST(Vector, simpleEquality)
{
    Vector vec(3);

    vec(0) = a;
    vec(1) = b;
    vec(2) = c;

    CHECK(vec == abcVec);
}

TEST(Vector, simpleInequality)
{
    Vector vec(abcVec);

    vec(2) = 2*a*c;

    CHECK(vec != abcVec);
}

TEST(Vector, inequalityDueToDimensions)
{
    const Vector vec(12);

    CHECK(vec != abcVec);
}

TEST(Vector, assignAndRetainValues)
{
    Vector vec(3);

    vec(0) = a;
    vec(1) = 2*b;
    vec(2) = Number(1, 2)*c;

    CHECK(!vec.isZero());
    CHECK_EQUAL(a, vec(0));
    CHECK_EQUAL(2*b, vec(1));
    CHECK_EQUAL(Number(1, 2)*c, vec(2));
}

TEST(Vector, retainValueOutOfRange)
{
    disableLog();
    CHECK_EQUAL(0, abcVec(5));
    enableLog();
}

TEST(Vector, valueOutOfRangeZeroSize)
{
    const Vector constVec;
    Vector vec(0);

    disableLog();
    CHECK_EQUAL(0, vec(1));
    CHECK_EQUAL(0, constVec(0));
    enableLog();
}

TEST(Vector, valueOutOfRangeZeroSizeConst)
{
    const Vector vec(5);

    disableLog();
    CHECK_EQUAL(0, vec(6));
    enableLog();
}

TEST(Vector, retainValueOutOfRangeNegative)
    /* Passing a negative int will result in a very high unsigned int by implicit conversion. */
{
    disableLog();
    CHECK_EQUAL(0, abcVec((size_t)-1));
    enableLog();
}

TEST(Vector, retainValueOutOfRangeTwice)
    /* The non-const reference returned in this case a static member variable, it is thus possible
     * to modify it. Should after a modification still return zero. */
{
    disableLog();
    Var& res(abcVec(5));
    enableLog();

    res = a;

    disableLog();
    res = abcVec(5);
    enableLog();

    CHECK(res.isZero());
}

TEST(Vector, retainConstValueOutOfRange)
{
    const Vector copy(abcVec);

    disableLog();
    CHECK_EQUAL(0, copy(10));
    enableLog();
}

TEST(Vector, copyConstructor)
{
    Vector vec1(3);

    vec1(0) = a;
    vec1(1) = 2*a;
    vec1(2) = 3*a;

    Vector vec2(vec1);

    CHECK_EQUAL(3, vec2.size());

    CHECK_EQUAL(a, vec2(0));
    CHECK_EQUAL(2*a, vec2(1));
    CHECK_EQUAL(3*a, vec2(2));
}

TEST(Vector, selfAssignment)
{
    abcVec = abcVec;

    checkAbcVector(abcVec);
}

TEST(Vector, assignmentOperatorEqualSize)
{
    Vector vec(3);

    vec = abcVec;

    checkAbcVector(vec);
}

TEST(Vector, assignmentOperatorLargerSize)
{
    Vector vec(2);

    vec = abcVec;

    checkAbcVector(vec);
}

TEST(Vector, assignmentOperatorLesserSize)
{
    Vector vec(4);

    vec = abcVec;

    checkAbcVector(vec);
}

TEST(Vector, simpleAddition)
{
    Vector vec(3);
    Vector res;

    vec(0) = 1;
    vec(1) = 2;
    vec(2) = 3;

    res = vec + abcVec;

    CHECK_EQUAL(1 + a, res(0));
    CHECK_EQUAL(2 + b, res(1));
    CHECK_EQUAL(3 + c, res(2));
}

TEST(Vector, addVectorsOfDifferentSize)
{
    Vector vec(1);
    Vector res;

    vec(0) = 2*a*b;

    disableLog();
    /* This should simply return the unmodified lhs vector. */
    res = vec + abcVec;
    enableLog();

    CHECK(vec == res);
}

TEST(Vector, subtractVectorsOfDifferentSize)
{
    Vector vec(5);
    Vector res;

    vec(1) = a;
    vec(2) = 2;

    disableLog();
    /* This should simply return the unmodified lhs vector. */
    res = vec - abcVec;
    enableLog();

    CHECK(vec == res);
}

TEST(Vector, multiplyByScalar)
{
    abcVec = abcVec*a;

    CHECK_EQUAL(a*a, abcVec(0));
    CHECK_EQUAL(a*b, abcVec(1));
    CHECK_EQUAL(a*c, abcVec(2));

    abcVec = 2*abcVec;
    CHECK_EQUAL(2*a*a, abcVec(0));
    CHECK_EQUAL(2*a*b, abcVec(1));
    CHECK_EQUAL(2*a*c, abcVec(2));
}

TEST(Vector, simpleDotProduct)
{
    Vector vec(3);
    Var res;

    vec(0) = a;
    vec(1) = 2;
    vec(2) = a*b;

    res = abcVec.dotProduct(vec);

    CHECK_EQUAL(a*a + 2*b + a*b*c, res);
}

TEST(Vector, illegalDotProductDueToDimensions)
{
    Vector vec(1);
    Var res;

    vec(0) = a;

    disableLog();
    res = abcVec.dotProduct(vec);
    enableLog();

    CHECK(res.isZero());
}

TEST(Vector, unaryPlusOperator)
{
    Vector res;

    res = +abcVec;

    checkAbcVector(res);
}

TEST(Vector, unaryMinusOperator)
{
    Vector res;

    res = -abcVec;

    CHECK_EQUAL(3, res.size());

    CHECK_EQUAL(-a, res(0));
    CHECK_EQUAL(-b, res(1));
    CHECK_EQUAL(-c, res(2));
}

TEST(Vector, minusOperator)
{
    Vector vec(3);
    Vector res;

    vec(0) = 1;
    vec(1) = 2*a;
    vec(2) = c;

    res = abcVec - vec;

    CHECK_EQUAL(a - 1, res(0));
    CHECK_EQUAL(-2*a + b, res(1));
    CHECK_EQUAL(0, res(2));
}

TEST(Vector, compoundPlusOperator)
{
    Vector vec(3);

    vec(0) = 0;
    vec(1) = a;
    vec(2) = c;

    abcVec += vec;

    CHECK_EQUAL(a, abcVec(0));
    CHECK_EQUAL(a + b, abcVec(1));
    CHECK_EQUAL(2*c, abcVec(2));
}

TEST(Vector, compoundMinusOperator)
{
    Vector vec(3);

    vec(0) = -a;
    vec(1) = b;
    vec(2) = 0;

    abcVec -= vec;

    CHECK_EQUAL(2*a, abcVec(0));
    CHECK_EQUAL(0, abcVec(1));
    CHECK_EQUAL(c, abcVec(2));
}

TEST(Vector, printerOperator)
{
    const std::string expected("[ a ]\n[ b ]\n[ c ]");
    std::stringstream stream;

    stream << abcVec;

    CHECK_EQUAL(expected, stream.str());
}
