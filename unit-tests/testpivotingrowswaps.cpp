
#include <vector>
#include "boostmatrixvector.h"
#include "directsolve.h"
#include "stdvecwrapper.h"
#include "tsymtests.h"
#include "var.h"

using namespace tsym;

struct PivotingRowSwapsFixture {
    Var a{"a"};
    Var b{"b"};
    Var c{"c"};
    Var d{"d"};
};

using OptVector = std::optional<VectorAdaptor<>>;

BOOST_FIXTURE_TEST_SUITE(TestPivotingRowSwaps, PivotingRowSwapsFixture)

BOOST_AUTO_TEST_CASE(nothingToDo)
{
    SquareMatrixAdaptor<> m{{a, b, c, d}, 2};
    OptVector rhs;
    const unsigned rowSwaps = eliminateGauss(m, rhs, &firstNonZeroPivot);

    BOOST_CHECK_EQUAL(0, rowSwaps);
}

BOOST_AUTO_TEST_CASE(numericPivotingDim3)
{
    const std::size_t dim = 3;
    const SquareMatrixAdaptor<> orig{{0, 2, 1, 3, 4, 2, 9, 22, 7}, dim};
    OptVector rhs;

    auto m = orig;

    unsigned rowSwaps = eliminateGauss(m, rhs, &firstNonZeroPivot);

    BOOST_CHECK_EQUAL(1, rowSwaps);

    m = orig;

    rowSwaps = eliminateGauss(m, rhs, &leastComplexityPivot);

    BOOST_CHECK_EQUAL(1, rowSwaps);
}

BOOST_AUTO_TEST_CASE(zeroLastRowDim3)
{
    const SquareMatrixAdaptor<> orig{{0, 1, a, b, 0, 2, a, Var(-1, 2), 0}, 3};
    const OptVector rhsOrig{{{1, 2, 0}}};
    VectorAdaptor<> x{{0, 0, 0}};

    auto m = orig;
    auto rhs = rhsOrig;
    unsigned rowSwaps = eliminateGauss(m, rhs, &firstNonZeroPivot);

    BOOST_CHECK_EQUAL(1, rowSwaps);

    m = orig;
    rhs = rhsOrig;
    rowSwaps = eliminateGauss(m, rhs, &leastComplexityPivot);

    BOOST_CHECK_EQUAL(1, rowSwaps);
}

BOOST_AUTO_TEST_SUITE_END()
