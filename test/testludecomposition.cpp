
#include "plu.h"
#include "var.h"
#include "boostmatrixvector.h"
#include "tsymtests.h"

using namespace tsym;

struct LUDecompositionFixture {
    const Var a{"a"};
    const Var b{"b"};
    const Var c{"c"};
    const Var d{"d"};
};

BOOST_FIXTURE_TEST_SUITE(TestLUDecomposition, LUDecompositionFixture)

BOOST_AUTO_TEST_CASE(dimTwoWithDefaults)
{
    const auto expected = createBoostMatrix({{ a, b }, { c/a, -b*c/a + d }});
    auto m = createBoostMatrix({{ a, b, }, { c, d }});
    auto mProxy = BoostMatrixProxy(m);
    plu::detail::LUDecomposition<BoostMatrixProxy, BoostSizeType> lu(mProxy, 2);

    lu.factorize();

    BOOST_CHECK_EQUAL(expected, m);
}

BOOST_AUTO_TEST_CASE(dimTwoWithLambdaAccess)
{
    const auto expected = createBoostMatrix({{ a, b }, { c/a, -b*c/a + d }});
    auto accessMatrix = [](BoostMatrix& m, BoostSizeType i, BoostSizeType j) -> Var& { return m(i, j); };
    auto m = createBoostMatrix({{ a, b, }, { c, d }});
    auto mProxy = BoostMatrixProxy(m, accessMatrix);
    plu::detail::LUDecomposition<BoostMatrixProxy, BoostSizeType> lu(mProxy, 2);

    lu.factorize();

    BOOST_CHECK_EQUAL(expected, m);
}

BOOST_AUTO_TEST_CASE(applyToRightHandSide)
{
    auto m = createBoostMatrix({{ a, b }, { c/a, -b*c/a + d }});
    auto mProxy = BoostMatrixProxy(m);
    auto rhs = createBoostVector({ a - 2*b, c - 2*d });
    auto rhsProxy = BoostVectorProxy(rhs);
    auto x = BoostVector(2);
    auto xProxy = BoostVectorProxy(x);
    plu::detail::LUDecomposition<BoostMatrixProxy, BoostSizeType> lu(mProxy, 2);
    
    lu.computeSolution(rhsProxy, xProxy);

    BOOST_CHECK_EQUAL(1, x(0));
    BOOST_CHECK_EQUAL(-2, x(1));
}

BOOST_AUTO_TEST_CASE(dimThreeDecompAndApplyToRhs)
{
    BoostSizeType dim = 3;
    BoostMatrix m(dim, dim);
    auto mProxy = BoostMatrixProxy(m);
    BoostVector rhs(dim);
    auto rhsProxy = BoostVectorProxy(rhs);
    BoostVector x(dim);
    auto xProxy = BoostVectorProxy(x);
    plu::detail::LUDecomposition<BoostMatrixProxy, BoostSizeType> lu(mProxy, dim);

    m(0, 0) = 3;
    m(0, 1) = 4;
    m(0, 2) = 2;
    m(1, 0) = 0;
    m(1, 1) = 2;
    m(1, 2) = 1;
    m(2, 0) = 9;
    m(2, 1) = 22;
    m(2, 2) = 7;

    rhs(0) = 17;
    rhs(1) = 7;
    rhs(2) = 74;

    lu.factorize();
    lu.computeSolution(rhsProxy, xProxy);

    BOOST_CHECK_EQUAL(1, x(0));
    BOOST_CHECK_EQUAL(2, x(1));
    BOOST_CHECK_EQUAL(3, x(2));
}

BOOST_AUTO_TEST_SUITE_END()
