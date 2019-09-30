
#include <array>
#include <vector>
#include "boostmatrixvector.h"
#include "functions.h"
#include "solve.h"
#include "tsymtests.h"

using namespace tsym;

struct PluFixture {
    const Var a = Var("a");
    const Var b = Var("b");
    const Var c = Var("c");
    const Var d = Var("d");
    const Var e = Var("e");
    const Var f = Var("f");
    const Var half = Var(1, 2);
    const Var sqrtTwo = tsym::sqrt(2);
};

BOOST_FIXTURE_TEST_SUITE(TestPlu, PluFixture)

BOOST_AUTO_TEST_CASE(solveWithSingularMatrix)
{
    const auto bCosA = tsym::pow(b, cos(a));
    auto A = createBoostMatrix({{12 * a, a * bCosA + a * c}, {12, bCosA + c}});
    auto rhs = createBoostVector({10, b});
    auto x = createBoostVector({0, 0});

    BOOST_CHECK_THROW(solve(A, rhs, x, x.size()), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim2a)
{
    auto A = createBoostMatrix({{0, 1}, {1, 0}});
    auto rhs = createBoostVector({a, b});
    auto x = createBoostVector({0, 0});
    auto expected = createBoostVector({b, a});

    solve(A, rhs, x, x.size());

    BOOST_CHECK_EQUAL(expected, x);
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim2b)
{
    BoostMatrix A(2, 2);
    BoostVector rhs(2);
    BoostVector x(2);

    A(0, 0) = a;
    A(0, 1) = b;
    A(1, 0) = c;
    A(1, 1) = d;

    rhs(0) = a + b;
    rhs(1) = c + d;

    solve(A, rhs, x, x.size());

    BOOST_CHECK_EQUAL(1, x(0));
    BOOST_CHECK_EQUAL(1, x(1));
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim3a)
{
    auto A = createBoostMatrix({{a, 17 * b / 29, 0}, {0, 1 / (a * b * c), tsym::pow(12, d)}, {1, 4 * a, 0}});
    auto rhs = createBoostVector({a * d + 17 * a * b / 116, b * tsym::pow(12, d) + 1 / (b * c * 4), d + a * a});
    auto x = createBoostVector({0, 0, 0});

    solve(A, rhs, x, x.size());

    BOOST_CHECK_EQUAL(3, x.size());
    BOOST_CHECK_EQUAL(d, x(0));
    BOOST_CHECK_EQUAL(a / 4, x(1));
    BOOST_CHECK_EQUAL(b, x(2));
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim4SkipField)
{
    const Var untouched = tsym::pow(a + d, b);
    auto A = createBoostMatrix({{a, b, 17 * b / 29, 0}, {d, 3 * d - a, 4, 0},
      {0, a - b, 1 / (a * b * c), tsym::pow(12, d)}, {1, 5 * a * a, 4 * a, 0}});
    auto rhs = createBoostVector({a * d + 17 * a * b / 116, d, b * tsym::pow(12, d) + 1 / (b * c * 4), d + a * a});
    auto x = createBoostVector({0, untouched, 0, 0});
    std::vector<bool> skipField{false, true, false, false};

    solve(A, rhs, x, skipField, x.size());

    BOOST_CHECK_EQUAL(4, x.size());
    BOOST_CHECK_EQUAL(d, x(0));
    BOOST_CHECK_EQUAL(untouched, x(1));
    BOOST_CHECK_EQUAL(a / 4, x(2));
    BOOST_CHECK_EQUAL(b, x(3));
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim3b, *label("expensive"))
{
    const BoostSizeType dim(3);
    BoostMatrix A(dim, dim);
    BoostVector rhs(dim);
    BoostVector x(dim);

    A(0, 0) = a;
    A(0, 1) = Var(1, 2);
    A(0, 2) = b;
    A(1, 0) = 0;
    A(1, 1) = c + d;
    A(1, 2) = tsym::pow(a, 2 * d);
    A(2, 0) = 1;
    A(2, 1) = 5;
    A(2, 2) = b;

    rhs(0) = 3 * b / 10 + b * f * f + e * a * a * f;
    rhs(1) = 3 * (d + c) / 5 * b + tsym::pow(a, 2 * d) * f * f;
    rhs(1) = Var(3, 5) * b * (d + c) + tsym::pow(a, 2 * d) * f * f;
    rhs(2) = 3 * b + b * f * f + e * a * f;

    solve(A, rhs, x, dim);

    BOOST_CHECK_EQUAL(a * e * f, x(0));
    BOOST_CHECK_EQUAL(3 * b / 5, x(1));
    BOOST_CHECK_EQUAL(f * f, x(2));
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim4a)
{
    BoostMatrix A(4, 4);
    BoostVector rhs(4);
    BoostVector x(4);

    A(0, 0) = a;
    A(0, 1) = 1;
    A(1, 1) = b;
    A(1, 0) = tsym::pow(a, 3);
    A(1, 3) = 2;
    A(2, 2) = c;
    A(3, 0) = a;
    A(3, 2) = b;

    rhs(0) = 1;
    rhs(1) = 2;
    rhs(2) = 3;
    rhs(3) = 4;

    solve(A, rhs, x, rhs.size());

    BOOST_CHECK_EQUAL((4 * c - 3 * b) / (a * c), x(0));
    BOOST_CHECK_EQUAL(-3 + 3 * b / c, x(1));
    BOOST_CHECK_EQUAL(3 / c, x(2));
    BOOST_CHECK_EQUAL(1 - 2 * a * a + 3 * b / 2 + 3 * a * a * b / (2 * c) - 3 * b * b / (2 * c), x(3));
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim4b)
{
    const Var sqrtTwo = tsym::sqrt(2);
    BoostMatrix A(4, 4);
    BoostVector rhs(4);
    BoostVector x(4);

    A(0, 0) = sqrtTwo * b / (4 * a) + b / a;
    A(0, 1) = 0;
    A(0, 2) = -sqrtTwo * b / (4 * a);
    A(0, 3) = sqrtTwo * b / (4 * a);

    A(1, 0) = 0;
    A(1, 1) = 1;
    A(1, 2) = 0;
    A(1, 3) = 0;

    A(2, 0) = -sqrtTwo * b / (4 * a);
    A(2, 1) = 0;
    A(2, 2) = sqrtTwo * b / (4 * a) + b / a;
    A(2, 3) = -sqrtTwo * b / (4 * a);

    A(3, 0) = sqrtTwo * b / (4 * a);
    A(3, 1) = 0;
    A(3, 2) = -sqrtTwo * b / (4 * a);
    A(3, 3) = sqrtTwo * b / (4 * a) + b / a;

    rhs(0) = 0;
    rhs(1) = 0;
    rhs(2) = 1;
    rhs(3) = 0;

    solve(A, rhs, x, rhs.size(), Algo::Gauss);

    BOOST_CHECK_EQUAL(a / (3 * b + 2 * sqrtTwo * b), x(0));
    BOOST_CHECK_EQUAL(0, x(1));
    BOOST_CHECK_EQUAL((2 * a + 2 * sqrtTwo * a) / (3 * b + 2 * sqrtTwo * b), x(2));
    BOOST_CHECK_EQUAL(a / (3 * b + 2 * sqrtTwo * b), x(3));
}

BOOST_AUTO_TEST_CASE(solveDependentLinearSystemDim4, *label("expensive"))
{
    const BoostSizeType dim(4);
    BoostMatrix A(dim, dim);
    BoostVector rhs(dim);
    BoostVector x(dim);

    A(0, 0) = a;
    A(0, 1) = b;
    A(1, 0) = 1;
    A(1, 1) = c;
    A(1, 2) = 2;
    A(1, 3) = 1;
    A(2, 2) = b * b;
    A(2, 3) = 1;
    A(3, 0) = a;
    A(3, 1) = a * c;
    A(3, 2) = 2 * a;
    A(3, 3) = a;

    rhs(0) = 9;
    rhs(1) = a;
    rhs(2) = b * b;
    rhs(3) = a * a;

    BOOST_CHECK_THROW(solve(A, rhs, x, x.size()), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim3WithStdVector)
{
    std::vector<std::vector<tsym::Var>> A;
    std::vector<tsym::Var> x(3);
    std::vector<tsym::Var> rhs;

    A.push_back({1, 0, 3});
    A.push_back({b, 0, 4});
    A.push_back({10, 1, -a});

    rhs.push_back(a + 3 * c);
    rhs.push_back(a * b + 4 * c);
    rhs.push_back(10 * a + b - a * c);

    solve(A, rhs, x, x.size());

    BOOST_CHECK_EQUAL(a, x[0]);
    BOOST_CHECK_EQUAL(b, x[1]);
    BOOST_CHECK_EQUAL(c, x[2]);
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim3WithStdArray)
{
    std::array<std::array<tsym::Var, 3>, 3> A;
    std::array<tsym::Var, 3> x;
    std::array<tsym::Var, 3> rhs;

    A[0] = {{1, -1, a}};
    A[1] = {{b, 0, 4}};
    A[2] = {{10, 1, -a}};

    rhs[0] = a - b + a * c;
    rhs[1] = a * b + 4 * c;
    rhs[2] = 10 * a + b - a * c;

    solve(A, rhs, x, x.size());

    BOOST_CHECK_EQUAL(a, x[0]);
    BOOST_CHECK_EQUAL(b, x[1]);
    BOOST_CHECK_EQUAL(c, x[2]);
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim3WithMixedTypes)
{
    tsym::Var A[3][3];
    std::vector<Var> rhs(3);
    std::vector<Var> x(3);

    A[0][0] = 0;
    A[0][1] = 4;
    A[0][2] = 2;
    A[1][0] = c * c;
    A[1][1] = c;
    A[1][2] = 1 / c;
    A[2][0] = 1;
    A[2][1] = -1;
    A[2][2] = 1;

    rhs[0] = 8 * a + 6 * b;
    rhs[1] = 2 * a * c + 3 * b / c;
    rhs[2] = -2 * a + 3 * b;

    solve(A, rhs, x, std::vector<Var>::size_type{3});

    BOOST_CHECK_EQUAL(0, x[0]);
    BOOST_CHECK_EQUAL(2 * a, x[1]);
    BOOST_CHECK_EQUAL(3 * b, x[2]);
}

BOOST_AUTO_TEST_CASE(solveZeroDimension)
{
    auto A = createBoostMatrix({{}});
    auto rhs = createBoostVector({});
    auto x = createBoostVector({});

    solve(A, rhs, x, std::vector<Var>::size_type{0});
}

BOOST_AUTO_TEST_CASE(solveWithPlainCArrayAndSimplePivoting)
{
    Var A[3][3];
    Var b[3];
    Var x[3];

    A[0][0] = 0;
    A[0][1] = 2;
    A[0][2] = 1;
    A[1][0] = 3;
    A[1][1] = 4;
    A[1][2] = 2;
    A[2][0] = 9;
    A[2][1] = 22;
    A[2][2] = 7;

    b[0] = 7;
    b[1] = 17;
    b[2] = 74;

    solve(A, b, x, 3);

    BOOST_CHECK_EQUAL(1, x[0]);
    BOOST_CHECK_EQUAL(2, x[1]);
    BOOST_CHECK_EQUAL(3, x[2]);
}

BOOST_AUTO_TEST_CASE(solveLinearSystemDim3PivotingByCycling)
{
    BoostMatrix A(3, 3);
    BoostVector rhs(3);
    BoostVector x(3);

    A(0, 0) = a;
    A(0, 1) = 2;
    A(1, 1) = 1;
    A(2, 2) = 3;

    rhs(0) = 4;
    rhs(1) = 2;
    rhs(2) = 9;

    solve(A, rhs, x, x.size());

    BOOST_CHECK_EQUAL(0, x(0));
    BOOST_CHECK_EQUAL(2, x(1));
    BOOST_CHECK_EQUAL(3, x(2));
}

BOOST_AUTO_TEST_CASE(solveLinearNumericSystemDim10)
{
    const size_t dim = 10;
    std::vector<std::vector<Var>> A(dim);
    std::vector<Var> rhs(dim);
    std::vector<Var> expected(dim);
    std::vector<Var> x(dim);

    A[0] = std::vector<Var>{Var(985, 1944), 0, Var(-49, 108), Var(-3, 8), 0, 0, 0, 0, 0, 0, Var(21, 5)};
    A[1] = std::vector<Var>{0, Var(13, 18), 0, 0, Var(-1, 2), 0, 0, 0, 0, 0, 0};
    A[2] = std::vector<Var>{Var(-49, 108), 0, Var(43, 18), Var(3, 4), 0, 0, 0, 0, 0, 0, Var(3, 4)};
    A[3] =
      std::vector<Var>{Var(-3, 8), 0, Var(3, 4), Var(567, 1000), 0, Var(-12, 25), Var(-24, 125), 0, 0, 0, Var(123, 32)};
    A[4] = std::vector<Var>{0, Var(-1, 2), 0, 0, Var(9, 10), 0, 0, Var(-2, 5), 0, 0, 0};
    A[5] = std::vector<Var>{0, 0, 0, Var(-12, 25), 0, Var(6, 5), Var(12, 25), 0, 0, 0, Var(-15, 16)};
    A[6] = std::vector<Var>{0, 0, 0, Var(-24, 125), 0, Var(12, 25),
      Var(24, 125) + Var(1, 9) / sqrtTwo + Var(4, 243) * sqrtTwo, Var(-1, 9) / sqrtTwo + Var(4, 243) * sqrtTwo,
      Var(2, 27) * sqrtTwo, Var(2, 27) * sqrtTwo, Var(-27, 32)};
    A[7] = std::vector<Var>{0, 0, 0, 0, Var(-2, 5), 0, Var(-1, 9) / sqrtTwo + Var(4, 243) * sqrtTwo,
      Var(2, 5) + Var(1, 9) / sqrtTwo + Var(4, 243) * sqrtTwo, Var(2, 27) * sqrtTwo, Var(2, 27) * sqrtTwo, 0};
    A[8] = std::vector<Var>{
      0, 0, 0, 0, 0, 0, Var(2, 27) * sqrtTwo, Var(2, 27) * sqrtTwo, Var(4, 9) * sqrtTwo, Var(2, 9) * sqrtTwo, 0};
    A[9] = std::vector<Var>{
      0, 0, 0, 0, 0, 0, Var(2, 27) * sqrtTwo, Var(2, 27) * sqrtTwo, Var(2, 9) * sqrtTwo, Var(4, 9) * sqrtTwo, 0};

    rhs[0] = Var(21, 5);
    rhs[1] = 0;
    rhs[2] = Var(3, 4);
    rhs[3] = Var(123, 32);
    rhs[4] = 0;
    rhs[5] = Var(-15, 16);
    rhs[6] = Var(-27, 32);
    rhs[7] = 0;
    rhs[8] = 0;
    rhs[9] = 0;

    solve(A, rhs, x, x.size(), Algo::GaussLCPivot);

    expected[0] = Var(378351, 1280);
    expected[1] = Var(-135, 64);
    expected[2] = Var(-135027, 1280);
    expected[3] = Var(132049, 256);
    expected[4] = Var(-195, 64);
    expected[5] = Var(132629, 640) + Var(27, 8) / sqrtTwo;
    expected[6] = Var(-135, 32) - Var(135, 16) / sqrtTwo;
    expected[7] = Var(-135, 32);
    expected[8] = Var(15, 16) + Var(15, 16) / sqrtTwo;
    expected[9] = Var(15, 16) + Var(15, 16) / sqrtTwo;

    BOOST_TEST(expected == x, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_CASE(solveLinearSymbolicSystemDim10)
{
    const size_t dim = 10;
    BoostMatrix A(dim, dim);
    BoostVector rhs(dim);
    BoostVector x(dim);
    BoostVector expected(dim);

    A(0, 0) = Var(985, 1944) * a * b;
    A(0, 1) = 0;
    A(0, 2) = Var(-49, 108) * a * b;
    A(0, 3) = Var(-3, 8) * a * b;
    A(0, 4) = 0;
    A(0, 5) = 0;
    A(0, 6) = 0;
    A(0, 7) = 0;
    A(0, 8) = 0;
    A(0, 9) = 0;

    A(1, 0) = 0;
    A(1, 1) = Var(13, 18) * a * b;
    A(1, 2) = 0;
    A(1, 3) = 0;
    A(1, 4) = Var(-1, 2) * c * a;
    A(1, 5) = 0;
    A(1, 6) = 0;
    A(1, 7) = 0;
    A(1, 8) = 0;
    A(1, 9) = 0;

    A(2, 0) = Var(-49, 108) * a * b;
    A(2, 1) = 0;
    A(2, 2) = Var(43, 18) * a * b;
    A(2, 3) = Var(3, 4) * a * b;
    A(2, 4) = 0;
    A(2, 5) = 0;
    A(2, 6) = 0;
    A(2, 7) = 0;
    A(2, 8) = 0;
    A(2, 9) = 0;

    A(3, 0) = Var(-3, 8) * a * b;
    A(3, 1) = 0;
    A(3, 2) = Var(3, 4) * a * b;
    A(3, 3) = Var(1143, 1000) * a * b;
    A(3, 4) = 0;
    A(3, 5) = Var(-24, 25) * a * b;
    A(3, 6) = Var(-96, 125) * a * b;
    A(3, 7) = 0;
    A(3, 8) = Var(-24, 25) * a * b;
    A(3, 9) = 0;

    A(4, 0) = 0;
    A(4, 1) = Var(-1, 2) * c * a;
    A(4, 2) = 0;
    A(4, 3) = 0;
    A(4, 4) = Var(9, 10) * c * a;
    A(4, 5) = 0;
    A(4, 6) = 0;
    A(4, 7) = Var(-2, 5) * c * a;
    A(4, 8) = 0;
    A(4, 9) = 0;

    A(5, 0) = 0;
    A(5, 1) = 0;
    A(5, 2) = 0;
    A(5, 3) = Var(-24, 25) * a * b;
    A(5, 4) = 0;
    A(5, 5) = Var(8, 5) * a * b;
    A(5, 6) = Var(24, 25) * a * b;
    A(5, 7) = 0;
    A(5, 8) = Var(4, 5) * a * b;
    A(5, 9) = 0;

    A(6, 0) = 0;
    A(6, 1) = 0;
    A(6, 2) = 0;
    A(6, 3) = Var(-96, 125) * a * b;
    A(6, 4) = 0;
    A(6, 5) = Var(24, 25) * a * b;
    A(6, 6) = Var(1, 9) * c * a / sqrtTwo + Var(96, 125) * a * b + Var(1, 243) * sqrtTwo * a * b;
    A(6, 7) = Var(-1, 9) * c * a / sqrtTwo + Var(1, 243) * sqrtTwo * a * b;
    A(6, 8) = Var(24, 25) * a * b;
    A(6, 9) = Var(1, 27) * sqrtTwo * a * b;

    A(7, 0) = 0;
    A(7, 1) = 0;
    A(7, 2) = 0;
    A(7, 3) = 0;
    A(7, 4) = Var(-2, 5) * c * a;
    A(7, 5) = 0;
    A(7, 6) = Var(-1, 9) * c * a / sqrtTwo + Var(1, 243) * sqrtTwo * a * b;
    A(7, 7) = Var(2, 5) * c * a + Var(1, 9) * c * a / sqrtTwo + Var(1, 243) * sqrtTwo * a * b;
    A(7, 8) = 0;
    A(7, 9) = Var(1, 27) * sqrtTwo * a * b;

    A(8, 0) = 0;
    A(8, 1) = 0;
    A(8, 2) = 0;
    A(8, 3) = Var(-24, 25) * a * b;
    A(8, 4) = 0;
    A(8, 5) = Var(4, 5) * a * b;
    A(8, 6) = Var(24, 25) * a * b;
    A(8, 7) = 0;
    A(8, 8) = Var(8, 5) * a * b;
    A(8, 9) = 0;

    A(9, 0) = 0;
    A(9, 1) = 0;
    A(9, 2) = 0;
    A(9, 3) = 0;
    A(9, 4) = 0;
    A(9, 5) = 0;
    A(9, 6) = Var(1, 27) * sqrtTwo * a * b;
    A(9, 7) = Var(1, 27) * sqrtTwo * a * b;
    A(9, 8) = 0;
    A(9, 9) = Var(1, 3) * sqrtTwo * a * b;

    rhs(0) = Var(33, 10);
    rhs(1) = 0;
    rhs(2) = Var(3, 4);
    rhs(3) = Var(12, 5);
    rhs(4) = 0;
    rhs(5) = Var(-5, 8);
    rhs(6) = Var(3, 2);
    rhs(7) = 0;
    rhs(8) = Var(5, 8);
    rhs(9) = 0;

    solve(A, rhs, x, x.size(), Algo::GaussLCPivot);

    expected(0) = 34263 / (160 * a * b);
    expected(1) = -27 / (-13 * a * b + 9 * a * c);
    expected(2) = -1215 / (16 * a * b);
    expected(3) = 59587 / (160 * a * b);
    expected(4) = -39 * b / (-13 * a * b * c + 9 * a * c * c);
    expected(5) =
      (28080 * b * b + 56160 * sqrtTwo * b * b - 1551937 * b * c - 38880 * sqrtTwo * b * c + 1066941 * c * c)
      / (-10400 * a * b * b * c + 7200 * a * b * c * c);
    expected(6) = (-351 * b - 702 * sqrtTwo * b + 135 * c + 486 * sqrtTwo * c) / (-52 * a * b * c + 36 * a * c * c);
    expected(7) = (-351 * b + 135 * c) / (-52 * a * b * c + 36 * a * c * c);
    expected(8) =
      (28080 * b * b + 56160 * sqrtTwo * b * b - 1568187 * b * c - 38880 * sqrtTwo * b * c + 1078191 * c * c)
      / (-10400 * a * b * b * c + 7200 * a * b * c * c);
    expected(9) = (39 * b + 39 * sqrtTwo * b - 15 * c - 27 * sqrtTwo * c) / (-26 * a * b * c + 18 * a * c * c);

    BOOST_CHECK_EQUAL(expected, x);
}

BOOST_AUTO_TEST_CASE(detDim0)
{
    const BoostSizeType dim(0);
    BoostMatrix A(dim, dim);
    const Var det = determinant(A, dim);

    BOOST_CHECK_EQUAL(1, det);
}

BOOST_AUTO_TEST_CASE(numericDetDim2)
{
    auto A = createBoostMatrix({{0, 1}, {1, 0}});
    const Var det = determinant(A, A.size1());

    BOOST_CHECK_EQUAL(-1, det);
}

BOOST_AUTO_TEST_CASE(symbolDetDim2)
{
    const Var expected(a * d - b * c);
    const BoostSizeType dim(2);
    BoostMatrix A(dim, dim);

    A(0, 0) = a;
    A(0, 1) = b;
    A(1, 0) = c;
    A(1, 1) = d;

    const Var det = determinant(A, dim);

    BOOST_CHECK_EQUAL(expected, det);
}

BOOST_AUTO_TEST_CASE(numericDetDim20)
{
    const double expected(352255267.354137);
    const size_t dim = 20;
    Var A[dim][dim] = {{0, -3, 0, 0, Var(2, 3), 3, 0, 2, 0.163763761215668, 1, 0.972645326126339, Var(-1, 9), -2,
                         Var(-2, 9), 0.518333759061494, -3, 2, 2, 0.784473042052998, 2},
      {-0.817845830240927, 0, 0, 0.871111379270801, -0.587976833818062, 0, -0.730432211807773, -0.436438037403665, 0, 0,
        Var(-1, 3), 0, 0, 0.336413789908801, 0, 1, 0, 0, -1, Var(-1, 3)},
      {0, 0, 0, -3, -2, 0, 0.432018803567675, 0.415954558314915, 0.334921106638503, 0, -0.441064077093597, -1, 0,
        -0.788960954477927, 0, -1, 2, 3, 0, 0.782883339525534},
      {-0.701953171249646, Var(-2, 9), 0, 0.233319224614176, 0.421511139950083, -0.236214381078049, 2,
        0.935533452453758, -1, -1, 3, -3, Var(1, 3), 0.431540855728871, -1, 0, -0.304962232988921, Var(2, 9), 2,
        Var(-1, 3)},
      {0.409536719769605, 0.169569519475026, Var(1, 3), 0, 0, 0.487494740627881, Var(1, 3), 0, 1, -3, 0, 1, 0, -3,
        0.197019152177339, Var(2, 9), 0, -0.217695943889126, -0.323049213293008, -2},
      {-3, 2, 0.564340432881273, -0.726041746735617, 0.661481848233678, 0.251710811672905, 0.291456760775844, 3,
        0.743946440292804, -1, 0.893180217983624, -3, 3, 0, -0.517223090263141, Var(-1, 6), Var(-1, 3),
        -0.564726256451931, -1, 3},
      {-0.362061416633498, 2, 0.878408900602927, Var(-1, 9), 0.285686875667206, -2, -2, 0, Var(-1, 6), 2, -1, 3,
        0.464463147669131, 0, Var(1, 9), Var(1, 3), 0, 0, -3, -1},
      {-0.879212124759158, -0.814199905230186, 2, Var(2, 9), -0.198250188424975, 0.501922631262054, 0, -2, Var(2, 9),
        -0.643714327219942, Var(-2, 3), -3, 2, Var(1, 9), Var(1, 6), -1, 0, Var(-1, 6), -0.891693022830127,
        -0.795647000362407},
      {0.400446050091482, -2, Var(-1, 3), -0.179156659692230, -1, -0.102493130926656, 0, 1, 0, -2, -0.602360060388995,
        0, 0, -1, -3, 0.472224469456821, -0.311738610520558, Var(1, 9), Var(-2, 9), -2},
      {-2, -0.619736720716315, 0, -0.514268868053285, 1, -0.576144179562380, 2, 2, Var(1, 9), 0.851523046057734, -2,
        Var(-1, 3), Var(1, 3), -0.661189346178467, -1, 2, -2, -3, 0, 0.141647243131056},
      {-3, -2, 0.756071627569365, 2, -3, 0, -0.286235664684729, 0.330354602667583, -0.404611840570539,
        -0.178961233748883, 0, 0, -3, 0, Var(-1, 3), -1, -3, 1, 2, -0.206800764892073},
      {-0.780178283220871, 2, -1, 0, 0, -0.614305973735434, 0.334233970643127, 0.351167397856154, 0.896517487896082,
        Var(1, 3), Var(-2, 9), 0, 0.556766076465591, 0, -0.255970680754231, 0.493476754055555, -2, -1, 0,
        -0.266899323261365},
      {Var(-1, 3), 0, Var(-1, 3), -0.249453521690999, -3, 0.480133238056094, -0.712493294431367, 0.684729493715378,
        Var(1, 9), 0.634799018780988, Var(-1, 3), 0, 0, Var(-2, 3), 1, 0.759958926984356, 1, -0.177062062068694,
        -0.738120757753549, -3},
      {Var(-1, 9), Var(2, 9), 0, Var(-1, 3), -2, Var(-1, 6), 0, 0, -0.991033486014383, Var(-1, 3), 2, 0, 0, 0, 0,
        -0.523520922745934, 1, 0.841536360677068, -0.570936096606758, 0},
      {-0.914577623243884, 0.643215642845554, Var(2, 3), Var(-2, 3), -1, Var(-1, 2), -0.448076097792330,
        -0.472765107276961, -1, 1, 1, -0.298450588426544, 0, -0.636200520881444, 0, 1, 0, 0, 0, -1},
      {0.560116920154630, 0.182250758566945, 0.790114920815374, 0, 0, 0, -1, 2, -0.751359753172712, -0.350507602358450,
        0.445573509241984, Var(-1, 3), 0.438068401333650, 0, 0.949396463448075, 0, 0.428948693635781,
        -0.432297272426761, -0.544789129383373, 0},
      {-1, -0.918781660997322, -0.469630186237979, -0.501190892717313, -0.506809526188111, 0, -0.745707881530680, 0, 3,
        Var(-1, 2), 0.118891897894698, -0.671535749003615, 0.420302665526368, 0.579728657232557, -0.971242483607050,
        0.957223256031235, 0, Var(-2, 3), -0.726977244558762, 0.729950389364666},
      {Var(1, 6), 0, 0, -1, 0.182417591159116, 0, Var(-2, 9), Var(-1, 6), 0, Var(2, 3), 0, -3, -2, -2,
        0.695995893453037, 3, -1, 0, -0.302923538999540, -0.193937987755568},
      {0, -0.877908243205685, 0, Var(-1, 3), 0, -1, -3, 0.305292463545492, 0.298548866432640, -3, 1, 0, Var(-1, 6),
        -0.390714584652508, -0.964800524467479, Var(-1, 6), 2, -0.891977924554555, 0, 0.318824163394123},
      {Var(2, 9), Var(2, 3), 0.593093099557646, 0, 0.612785416052094, 0, Var(2, 3), -1, 0, 0.854807696107082,
        0.631006623400826, 0, 2, 0, -1, -0.290114424749472, Var(1, 2), Var(-1, 9), -0.420632521291579, 0}};

    const Var det = determinant(A, dim);

    /* The tolerance shouldn't be too high here, because the resulting determinant is large. */
    BOOST_CHECK_CLOSE(expected, static_cast<double>(det), 1.e-10);
}

BOOST_AUTO_TEST_CASE(detDim3)
{
    auto A = createBoostMatrix({{0, 1, a}, {b, 0, 2}, {a, Var(-1, 2), 0}});
    const Var expected(-a * b / 2 + 2 * a);
    const Var det = determinant(A, A.size1());

    BOOST_CHECK_EQUAL(expected, det);
}

BOOST_AUTO_TEST_CASE(detDim3ThroughLamdba)
{
    std::array<std::array<Var, 3>, 3> A;
    const Var expected(a * b * d + 2 * c);

    A[0] = {{a, 1, 0}};
    A[1] = {{-2, 0, -b}};
    A[2] = {{0, d, c}};

    const Var det = determinant([&A](auto i, auto j) { return A.at(i).at(j); }, std::size_t{3});

    BOOST_CHECK_EQUAL(expected, det);
}

BOOST_AUTO_TEST_CASE(detDim4)
{
    auto A = createBoostMatrix({{0, 1, a, 3}, {b, 0, 2, 0}, {a, Var(-1, 2), 0, 2}, {0, b, 3, 0}});
    const Var expected(-6 * a * b - 2 * a * b * b + 21 * b / 2);
    const Var det = determinant(A, BoostSizeType{4});

    BOOST_CHECK_EQUAL(expected, det);
}

BOOST_AUTO_TEST_CASE(detDim7SkipField)
{
    boost::numeric::ublas::vector<bool> skipField(7, false);

    skipField(1) = skipField(2) = skipField(6) = true;

    auto A = createBoostMatrix({{0, b * a, a + b, 1, a, 3, 14}, {9, 8, 7, b + c, 5, a, 2},
      {a, 0, 0, 23 * d, c, d * c, a}, {b, 100, -100, 0, 2, 0, 2 * a / 3 + b}, {a, a, -3 * b, Var(-1, 2), 0, 2, 123},
      {0, f, f * f * f, b, 3, 0, -f}, {d, -10, b, d * c, 0, 72, 0}});
    const Var expected(-6 * a * b - 2 * a * b * b + 21 * b / 2);
    const Var det = determinant(A, skipField, BoostSizeType{7});

    BOOST_CHECK_EQUAL(expected, det);
}

BOOST_AUTO_TEST_CASE(inverseDim2)
{
    auto A = createBoostMatrix({{a, b}, {c, d}});
    const Var det = a * d - b * c;
    auto expected = createBoostMatrix({{d / det, -b / det}, {-c / det, 1 / (d - b * c / a)}});

    invert(A, A.size1());

    BOOST_CHECK_EQUAL(expected, A);
}

BOOST_AUTO_TEST_CASE(inverseDim2SpecifyAccess)
{
    const Var det = a * d - b * c;
    const std::vector<Var> expected{d / det, -b / det, -c / det, 1 / (d - b * c / a)};
    std::vector<Var> A{a, b, c, d};
    auto adaptor = [&A](auto i, auto j) -> Var& { return A[i * 2 + j]; };

    invert(adaptor, 2u);

    BOOST_TEST(expected == A, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_CASE(numericInverseDim3)
{
    auto A = createBoostMatrix({{1, 2, 3}, {2, 1, 0}, {3, -1, -4}});
    auto expected =
      createBoostMatrix({{Var(4, 3), Var(-5, 3), 1}, {Var(-8, 3), Var(13, 3), -2}, {Var(5, 3), Var(-7, 3), 1}});

    invert(A, A.size1());

    BOOST_CHECK_EQUAL(expected, A);
}

BOOST_AUTO_TEST_CASE(inverseDim3, *label("expensive"))
{
    const unsigned dim = 3;
    std::vector<std::vector<Var>> A(dim);
    std::vector<std::vector<Var>> expected(dim);

    A[0] = {a, 0, 2 * b};
    A[1] = {10, 0, c * d};
    A[2] = {a * a, b, 1};

    invert(A, dim, Algo::Gauss);

    expected[0] = {c * d / (-20 * b + a * c * d), -2 * b / (-20 * b + a * c * d), 0};
    expected[1] = {(10 - a * a * c * d) / (-20 * b * b + a * b * c * d),
      (-a + 2 * a * a * b) / (-20 * b * b + a * b * c * d), 1 / b};
    expected[2] = {-10 / (-20 * b + a * c * d), 1 / (-20 * b / a + c * d), 0};

    for (unsigned i = 0; i < dim; ++i)
        for (unsigned j = 0; j < dim; ++j) {
            BOOST_CHECK_EQUAL(expected[i][j], A[i][j]);
        }
}

BOOST_AUTO_TEST_CASE(inverseDim4SkipField, *label("expensive"))
{
    const unsigned dim = 4;
    const std::vector<int> skip{0, 0, 1, 0};
    std::vector<std::vector<Var>> A(dim);
    std::vector<std::vector<Var>> expected(dim);

    A[0] = {a, 0, c * c, 2 * b};
    A[1] = {10, 0, d + 4 * a / 3, c * d};
    A[2] = {2500, a * b * c, 3 * d * a, -10};
    A[3] = {a * a, b, -b * a, 1};

    invert(A, skip, dim, Algo::Gauss);

    expected[0] = {c * d / (-20 * b + a * c * d), -2 * b / (-20 * b + a * c * d), c * c, 0};
    expected[1] = {(10 - a * a * c * d) / (-20 * b * b + a * b * c * d),
      (-a + 2 * a * a * b) / (-20 * b * b + a * b * c * d), d + 4 * a / 3, 1 / b};
    expected[2] = {2500, a * b * c, 3 * d * a, -10},
    expected[3] = {-10 / (-20 * b + a * c * d), 1 / (-20 * b / a + c * d), -b * a, 0};

    for (unsigned i = 0; i < dim; ++i)
        for (unsigned j = 0; j < dim; ++j) {
            BOOST_CHECK_EQUAL(expected[i][j], A[i][j]);
        }
}

BOOST_AUTO_TEST_CASE(trivialInverseDim5)
{
    auto identity =
      createBoostMatrix({{1, 0, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 0, 1}});
    auto copy(identity);

    invert(identity, identity.size1());

    BOOST_CHECK_EQUAL(copy, identity);
}

BOOST_AUTO_TEST_CASE(inverseDim15, *label("expensive"))
{
    const size_t size = 15;
    Var A[size][size] = {{9.29395026851320, 8.09033121585509, 5.14433525830085, 8.95249050693807, 6.96258322137449,
                           4.11264527154185, 7.30290414594927, 9.37082878476097, 5.92939990091632, 9.92026448526447,
                           6.16902185735627, 9.03313816781665, 5.45734909494898, 8.98852263879611, 9.77177677229026},
      {4.83644760156884, 0, 4.84000869746979, 5.88337051798656, 4.70968745838288, 3.66214064215514, 6.36230717624036,
        3.50961417618395, 7.10581176984010, 5.04166029537815, 2.98501351263885, 0.554722563774794, 2.96550468652435,
        7.40414657646940, 9.71714502100726},
      {5.20057329793591, 6.01009766751540, 1.71316070661009, 4.82084058527944, 4.54114692959545, 0.945070546326013,
        8.91574866473931, 8.01108136699260, 8.45182575466427, 1.78514008364464, 3.78800832130653, 4.67115837166208,
        7.18669413858628, 2.27993849257348, 7.97237640672426},
      {0.561025730849580, 4.51258891898577, 2.27847708794378, 1.12754362691773, 7.43879850513026, 6.47197296769185,
        2.80893836509667, 7.69038720614297, 2.04368840164795, 3.55508378637137, 5.92318484981771, 3.51922348365048,
        8.91269422792576, 9.63573495306973, 9.95285640971918},
      {8.73682388684816, 6.07077570539229, 6.42159414907996, 7.70457151194356, 0.791111649907894, 7.99822805126661,
        2.06406697074826, 6.40934069966954, 1.72747736324253, 4.77243954227107, 1.88394196392722, 9.62850689915738,
        2.16581789479282, 1.96178221999787, 8.27788948946133},
      {7.68262445640236, 3.13318335485998, 4.59279740818428, 4.64149016258606, 1.24191514655683, 0, 3.26797377447073,
        7.52562359561980, 4.25482965002206, 1.73525641474494, 1.08178228974695, 6.46199240855777, 3.25572719485427,
        7.95316616228373, 7.85931353605145},
      {3.36916771032943, 5.64049126807565, 5.27680747044406, 0.157335735959609, 7.33816419152185, 3.59600014656244,
        1.69509580117098, 9.05781704931730, 8.19611131415660, 6.16728114589075, 1.57032243078534, 3.87307680870668,
        5.85325587728887, 2.36108850957649, 0.866762230017446},
      {0.574976392846100, 4.39075980793887, 5.02228193757148, 3.26566832850721, 1.98750778536600, 9.57659564280478,
        1.97988985325903, 2.58444491333057, 0.490094630292568, 2.90518205537825, 3.50600599354464, 9.40279398297339,
        0.225279774198123, 0.530598418119101, 1.33504330100030},
      {7.58495385275344, 1.50183448509619, 8.28332029075993, 0.863869830881749, 7.58580284521211, 9.44408695202429,
        3.34824082948345, 0.257018679991457, 5.84379212414533, 9.02498533309962, 1.25768463892998, 5.27143939279265,
        5.27786438004969, 7.25272212365802, 2.46073443141611},
      {4.16895493641656, 3.00199996861486, 7.91700724224825, 7.18400990478930, 6.44781518887440, 2.51328471705331,
        2.87583808065467, 3.77306225586734, 9.83678364562634, 5.79081110656823, 3.29582643985839, 2.63564573715060,
        3.74440312734826, 9.08235768559694, 3.58456781835626},
      {8.13571414582448, 6.84640455616605, 6.44899816211425, 0.583563821392530, 8.41172404540720, 5.51338933536693,
        9.31172604196418, 0.544475486585051, 2.30694413209765, 9.10470870651367, 2.94407160163276, 9.06301513849804,
        1.12007007757400, 5.38577885594940, 3.65809938637912},
      {4.97242785079997, 5.53975417080203, 1.74212763455302, 5.81236816633928, 9.68242261547356, 6.98632607706483,
        2.75833038887730, 8.88036703392089, 4.42380072714734, 5.07822833212463, 8.92375572496908, 0, 3.56695961888082,
        3.34810704987267, 2.14664808823191},
      {2.00326171653685, 7.32420746258326, 3.68482906687982, 8.21143222879577, 5.85899463495970, 3.95873024081868,
        3.81417205321199, 3.30299998094557, 1.09148291019075, 6.58063949671691, 3.58086483894916, 6.38884330539743,
        2.90791512921258, 2.92807219263060, 2.95328177453511},
      {6.80611836310833, 2.95080184762064, 3.34492866089684, 5.61981213289071, 5.01263459878027, 1.41941696922046,
        5.34452855396564, 2.73851703980014, 5.39863210114139, 5.50395385215616, 6.35107686373044, 5.24791722993784,
        1.73031360106109, 0, 6.88234593102001},
      {8.36781127281422, 1.01339079242435, 1.46643566196019, 9.05988332172014, 8.32515502504598, 8.79583647716310,
        2.57238204217801, 4.42643857206381, 4.74812400429605, 3.54101452266103, 3.75907522288290, 2.24228789070800,
        7.49434585191406, 6.74102530165140, 0}};
    const Var expected[size][size] = {
      {-0.00288071968262256, -0.0693153332677301, 0.0156789565669110, -0.0147982395566584, 0.0662556503668025,
        0.0113197561148176, -0.0426257920665564, -0.0711917632459387, 0.0287645810834458, 0.0156899167444463,
        0.0322919799031649, 0.0398784253040378, -0.0638782530483221, 0.0111075282240292, 0.0137741796082851},
      {0.0846594711158889, -0.140026142463294, 0.0383415539370544, 0.104265391460832, 0.263398342194149,
        -0.323118657136549, -0.0127609578156974, -0.114470602697389, -0.230425606063105, 0.281984439504022,
        0.189416714278449, -0.0634571054151137, -0.220021678701846, -0.0926593496556599, 0.0406982098806788},
      {-0.311647644431915, 0.00591490928785393, 0.166825315100842, -0.192197892813483, -0.277835736591644,
        0.578981255003894, -0.204738712691484, 0.0372067783659400, 0.617422076904644, -0.288588693079498,
        -0.321633679440864, 0.349205289799700, 0.526894043040405, -0.0241024009504714, -0.326891021374311},
      {-0.0330261215362486, 0.0447677144064443, -0.00814551157954093, -0.0351778564479346, -0.0112528689515597,
        0.0360339447210369, -0.00995769684855987, 0.00116530088851475, -0.00776634120465172, -0.0155336720570947,
        -0.0418554969749489, -0.00558011356249475, 0.111131181969656, 0.00457296612564604, 0.0292872566730555},
      {-0.149417930545767, 0.0895103545143849, -0.192294303921549, 0.144335364258974, 0.116149571462953,
        -0.123257187085581, 0.222970514629118, -0.0608324020023523, -0.391347831205904, 0.100024265837763,
        0.251151619196449, -0.177539222869334, -0.0884700606720979, 0.132236160584511, 0.210189183026545},
      {0.0652113320342236, 0.0321029939865715, -0.0165044616348271, 0.0460609167812167, 0.129287574749807,
        -0.205405869331669, 0.0306323229008375, 0.0282558019254022, -0.136901664980730, 0.0828766118715665,
        0.0876136791582145, -0.0563408269755789, -0.177718690636502, -0.0626372419246596, 0.0801004968540340},
      {-0.0707210860308481, 0.0826937350948717, 0.163637737542852, -0.165473000923057, -0.240340584949317,
        0.292453169293325, -0.133559541994671, 0.0818874113711627, 0.331906311019159, -0.250139661877045,
        -0.156468806178950, 0.186433764703030, 0.271312358242801, -0.0903526195011965, -0.155621281415128},
      {-0.0391053509439229, 0.0973770553908203, 0.00257446821720567, -0.103108747759235, -0.157687882421549,
        0.256351911818072, 0.0366315439887857, 0.0700968201460340, 0.149434326087054, -0.218536561254015,
        -0.132980423241944, 0.126253762680451, 0.169568339295747, -0.0381941770419289, -0.0796576295069572},
      {0.176453191297798, -0.0278116895362268, -0.0688781377267835, 0.118759542607137, 0.225051913806130,
        -0.433545230252534, 0.145665203089780, -0.00785012970951196, -0.413766765270447, 0.321587429406820,
        0.214530705924181, -0.239550842742885, -0.423341277647616, 0.0271523624843426, 0.187764621473400},
      {0.173479373035539, 0.0329140317411679, -0.0203657038203875, -0.0651633277253249, -0.0743266215282311,
        -0.0277427862728045, 0.00836302241028621, 0.00494043350461448, 0.137689276053252, -0.104369566100085,
        -0.109142120985255, 0.0118721968561130, 0.0539234413375382, -0.0195901560295130, -0.0660333057418640},
      {0.00953685121258915, -0.120458404985550, 0.114759535524944, -0.0732938257541118, -0.182969768742052,
        0.206258663231367, -0.208930418105406, 0.0705424764167445, 0.334892097653176, -0.0861852122156680,
        -0.202841057410046, 0.219609315335593, 0.122064195861557, 0.0388413146280774, -0.181154937369544},
      {0.0586075775454709, -0.0124227993588339, -0.104973533480439, 0.0738625315801758, 0.0186719761605958,
        -0.0858652391043916, 0.120188366855191, 0.0736682545013683, -0.222910416327149, 0.0624541097887187,
        0.102525973065975, -0.189132643081199, -0.133485071075621, 0.104228117824138, 0.139087065960051},
      {-0.0857736961872437, -0.0896063520442892, 0.188206955154682, -0.0855890115246864, -0.191081343419576,
        0.283599797237363, -0.165707112248890, -0.0101724979989318, 0.470539088001210, -0.210756393605008,
        -0.279015914682410, 0.161923020051683, 0.321713771552500, -0.0158519595215275, -0.168209215532831},
      {0.110269999182653, -0.0290676837557765, -0.0347897323362694, 0.0463156545164633, 0.0256195089840706,
        -0.0813514129766720, -0.0219120015719012, 0.00730455595077183, -0.104386259984983, 0.118979106326161,
        0.0756278692702257, -0.0467755358998044, -0.136808806048570, -0.0703240221074870, 0.0486641404959649},
      {0.00176683331752029, 0.0530310381347688, -0.0938712387835647, 0.138757391326374, 0.189444688908547,
        -0.214346878750652, 0.0975557216032190, -0.0690616550846699, -0.270239299567082, 0.123891329811036,
        0.148933843661104, -0.152214341949777, -0.169178936494543, 0.0670248276011444, 0.0957528399161877}};
    const double TOL = 1.e-8;

    invert(A, size);

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            BOOST_CHECK_CLOSE(static_cast<double>(expected[i][j]), static_cast<double>(A[i][j]), TOL);
}

BOOST_AUTO_TEST_CASE(illegalInverseSingular)
{
    BoostMatrix A(2, 2);

    A(0, 0) = 2 * a;
    A(0, 1) = -a * a;
    A(1, 1) = a;
    A(1, 0) = -2;

    BOOST_CHECK_THROW(invert(A, A.size1()), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
