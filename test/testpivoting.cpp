
#include "plu.h"
#include "var.h"
#include "boostmatrixvector.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Pivoting)
{
    Var a{"a"};
    Var b{"b"};
    Var c{"c"};
    Var d{"d"};
};

TEST(Pivoting, nothingToDo)
{
    auto m = createBoostMatrix({{ a, b, }, { c, d }});
    auto mProxy = BoostMatrixProxy(m);
    const auto copyOfM(m);
    const plu::detail::FirstNonZeroPivot<BoostMatrixProxy, BoostSizeType> pivot(mProxy, m.size1());

    pivot.applyTo(mProxy);

    CHECK_EQUAL(copyOfM, m);
}

TEST(Pivoting, numericPivotingDim3)
{
    std::initializer_list<Var> row1 = { 0, 2, 1 };
    std::initializer_list<Var> row2 = { 3, 4, 2 };
    std::initializer_list<Var> row3 = { 9, 22, 7 };
    const auto expected = createBoostMatrix({ row2, row1, row3 });
    auto m = createBoostMatrix({ row1, row2, row3 });
    auto mProxy = BoostMatrixProxy(m);
    const plu::detail::LeastComplexityPivot<BoostMatrixProxy, BoostSizeType> leastComplexityPivot(mProxy, m.size1());
    const plu::detail::FirstNonZeroPivot<BoostMatrixProxy, BoostSizeType> firstNonZeroPivot(mProxy, m.size1());

    firstNonZeroPivot.applyTo(mProxy);

    CHECK_EQUAL(expected, m);

    m = createBoostMatrix({ row1, row2, row3 });

    leastComplexityPivot.applyTo(mProxy);

    CHECK_EQUAL(expected, m);
}

TEST(Pivoting, pivotingDim2ApplyToRhs)
{
    std::initializer_list<Var> row1 = { 0, b };
    std::initializer_list<Var> row2 = { c, a };
    auto m = createBoostMatrix({ row1, row2 });
    auto mProxy = BoostMatrixProxy(m);
    auto vec = createBoostVector({ d, 2*d });
    auto vecProxy = BoostVectorProxy(vec);
    const plu::detail::FirstNonZeroPivot<BoostMatrixProxy, BoostSizeType> pivot(mProxy, m.size1());

    pivot.applyTo(mProxy);
    pivot.applyTo(vecProxy);

    CHECK_EQUAL(createBoostMatrix({ row2, row1 }), m);
    CHECK_EQUAL(createBoostVector({ 2*d, d }), vec);
}

TEST(Pivoting, leastComplexityButWithZeroDim2)
{
    std::initializer_list<Var> row1 = { 0, b };
    std::initializer_list<Var> row2 = { c, a };
    auto m = createBoostMatrix({ row1, row2 });
    auto mProxy = BoostMatrixProxy(m);
    const plu::detail::LeastComplexityPivot<BoostMatrixProxy, BoostSizeType> pivot(mProxy, m.size1());

    pivot.applyTo(mProxy);

    CHECK_EQUAL(createBoostMatrix({ row2, row1 }), m);
    CHECK_EQUAL(1, pivot.nRowSwaps());
}

TEST(Pivoting, leastComplexityButWithZeroDim3)
{
    std::initializer_list<Var> row1 = { 0, 1, a };
    std::initializer_list<Var> row2 = { b, 0, 2 };
    std::initializer_list<Var> row3 = { a, Var(-1, 2), 0 };
    const auto expected = createBoostMatrix({ row3, row1, row2 });
    auto m = createBoostMatrix({ row1, row2, row3 });
    auto mProxy = BoostMatrixProxy(m);
    const plu::detail::LeastComplexityPivot<BoostMatrixProxy, BoostSizeType> leastComplexityPivot(mProxy, m.size1());

    leastComplexityPivot.applyTo(mProxy);

    CHECK_EQUAL(expected, m);
    CHECK_EQUAL(2, leastComplexityPivot.nRowSwaps());
}
