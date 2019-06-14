
#include <vector>
#include "boostmatrixvector.h"
#include "firstnonzeropivot.h"
#include "leastcomplexitypivot.h"
#include "plu.h"
#include "stdvecwrapper.h"
#include "tsymtests.h"
#include "var.h"

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
    auto m = createBoostMatrix({{
                                  a,
                                  b,
                                },
      {c, d}});
    const auto copyOfM(m);
    const FirstNonZeroPivot<BoostMatrix> pivot(m, m.size1());

    pivot.applyTo(m);

    BOOST_CHECK_EQUAL(copyOfM, m);
}

BOOST_AUTO_TEST_CASE(numericPivotingDim3)
{
    const std::size_t dim = 3;
    const MatrixAdaptor expected{{3, 4, 2, 0, 2, 1, 9, 22, 7}, dim};
    const MatrixAdaptor orig{{0, 2, 1, 3, 4, 2, 9, 22, 7}, dim};
    const LeastComplexityPivot leastComplexityPivot(orig, dim);
    const FirstNonZeroPivot<MatrixAdaptor> firstNonZeroPivot(orig, dim);
    MatrixAdaptor m = orig;

    firstNonZeroPivot.applyTo(m);

    BOOST_TEST(expected.data == m.data, boost::test_tools::per_element{});

    m = orig;

    leastComplexityPivot.applyTo(m);

    BOOST_TEST(expected.data == m.data, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_CASE(pivotingDim2ApplyToRhs)
{
    std::initializer_list<Var> row1 = {0, b};
    std::initializer_list<Var> row2 = {c, a};
    auto m = createBoostMatrix({row1, row2});
    auto vec = createBoostVector({d, 2 * d});
    const FirstNonZeroPivot<BoostMatrix> pivot(m, m.size1());

    pivot.applyTo(m);
    pivot.applyTo(vec);

    BOOST_CHECK_EQUAL(createBoostMatrix({row2, row1}), m);
    BOOST_CHECK_EQUAL(createBoostVector({2 * d, d}), vec);
}

BOOST_AUTO_TEST_CASE(leastComplexityButWithZeroDim2)
{
    const std::size_t dim = 2;
    MatrixAdaptor m{{0, b, c, a}, dim};
    const MatrixAdaptor expected{{c, a, 0, b}, dim};
    const LeastComplexityPivot pivot(m, dim);

    pivot.applyTo(m);

    BOOST_TEST(expected.data == m.data, boost::test_tools::per_element{});
    BOOST_CHECK_EQUAL(1, pivot.nRowSwaps());
}

BOOST_AUTO_TEST_CASE(leastComplexityButWithZeroDim3)
{
    const std::size_t dim = 3;
    MatrixAdaptor m{{0, 1, a, b, 0, 2, a, Var(-1, 2), 0}, dim};
    const MatrixAdaptor expected{{a, Var(-1, 2), 0, 0, 1, a, b, 0, 2}, dim};
    const LeastComplexityPivot leastComplexityPivot(m, dim);

    leastComplexityPivot.applyTo(m);

    BOOST_TEST(expected.data == m.data, boost::test_tools::per_element{});
    BOOST_CHECK_EQUAL(2, leastComplexityPivot.nRowSwaps());
}

BOOST_AUTO_TEST_SUITE_END()
