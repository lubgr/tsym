
#include "plu.h"
#include "var.h"
#include "boostmatrixvector.h"
#include "tsymtests.h"

using namespace tsym;

struct PivotingFixture {
    Var a{"a"};
    Var b{"b"};
    Var c{"c"};
    Var d{"d"};
};

BOOST_FIXTURE_TEST_SUITE(TestPivoting, PivotingFixture)

BOOST_AUTO_TEST_CASE(nothingToDo)
{
    auto m = createBoostMatrix({{ a, b, }, { c, d }});
    auto mProxy = BoostMatrixProxy(m);
    const auto copyOfM(m);
    const plu::detail::FirstNonZeroPivot<BoostMatrixProxy, BoostSizeType> pivot(mProxy, m.size1());

    pivot.applyTo(mProxy);

    BOOST_CHECK_EQUAL(copyOfM, m);
}

BOOST_AUTO_TEST_CASE(numericPivotingDim3)
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

    BOOST_CHECK_EQUAL(expected, m);

    m = createBoostMatrix({ row1, row2, row3 });

    leastComplexityPivot.applyTo(mProxy);

    BOOST_CHECK_EQUAL(expected, m);
}

BOOST_AUTO_TEST_CASE(pivotingDim2ApplyToRhs)
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

    BOOST_CHECK_EQUAL(createBoostMatrix({ row2, row1 }), m);
    BOOST_CHECK_EQUAL(createBoostVector({ 2*d, d }), vec);
}

BOOST_AUTO_TEST_CASE(leastComplexityButWithZeroDim2)
{
    std::initializer_list<Var> row1 = { 0, b };
    std::initializer_list<Var> row2 = { c, a };
    auto m = createBoostMatrix({ row1, row2 });
    auto mProxy = BoostMatrixProxy(m);
    const plu::detail::LeastComplexityPivot<BoostMatrixProxy, BoostSizeType> pivot(mProxy, m.size1());

    pivot.applyTo(mProxy);

    BOOST_CHECK_EQUAL(createBoostMatrix({ row2, row1 }), m);
    BOOST_CHECK_EQUAL(1, pivot.nRowSwaps());
}

BOOST_AUTO_TEST_CASE(leastComplexityButWithZeroDim3)
{
    std::initializer_list<Var> row1 = { 0, 1, a };
    std::initializer_list<Var> row2 = { b, 0, 2 };
    std::initializer_list<Var> row3 = { a, Var(-1, 2), 0 };
    const auto expected = createBoostMatrix({ row3, row1, row2 });
    auto m = createBoostMatrix({ row1, row2, row3 });
    auto mProxy = BoostMatrixProxy(m);
    const plu::detail::LeastComplexityPivot<BoostMatrixProxy, BoostSizeType> leastComplexityPivot(mProxy, m.size1());

    leastComplexityPivot.applyTo(mProxy);

    BOOST_CHECK_EQUAL(expected, m);
    BOOST_CHECK_EQUAL(2, leastComplexityPivot.nRowSwaps());
}

BOOST_AUTO_TEST_SUITE_END()
