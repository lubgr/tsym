
#include "abc.h"
#include "globals.h"
#include "matrix.h"
#include "vector.h"
#include "solver.h"
#include "logging.h"
#include "cpputest.h"

using namespace tsym;

TEST_GROUP(Solver)
{
    Solver solver;
    Var a;
    Var b;
    Var c;
    Var d;

    void setup()
    {
        a = Var("a");
        b = Var("b");
        c = Var("c");
        d = Var("d");

        solver = Solver();
    }
};

TEST(Solver, defaultSolution)
{
    Vector x;

    disableLog();
    x = solver.solve();
    enableLog();

    CHECK_EQUAL(0, x.size());
}

TEST(Solver, rhsNotSet)
{
    const Matrix A(4, 4);
    Vector x;

    solver.set(A);

    disableLog();

    CHECK(!solver.hasSolution());
    x = solver.solve();

    enableLog();

    CHECK_EQUAL(0, x.size());
}

TEST(Solver, matrixNotSet)
{
    const Vector rhs(10);
    Vector x;

    solver.set(rhs);

    disableLog();

    CHECK(!solver.hasSolution());
    x = solver.solve();

    enableLog();

    CHECK_EQUAL(0, x.size());
}

TEST(Solver, nonSquareMatrix)
{
    const Matrix A(2, 3);
    const Vector rhs(2);

    solver.set(A);
    solver.set(rhs);

    disableLog();
    CHECK(!solver.hasSolution());
    enableLog();
}

TEST(Solver, nonMatchingDimensions)
{
    const Matrix A(3, 3);
    const Vector rhs(4);

    solver.set(A);
    solver.set(rhs);

    disableLog();
    CHECK(!solver.hasSolution());
    enableLog();
}

TEST(Solver, zeroDimension)
{
    const Matrix A;
    const Vector rhs;

    solver.set(A);
    solver.set(rhs);

    disableLog();
    CHECK(!solver.hasSolution());
    enableLog();
}

TEST(Solver, singularMatrix)
{
    const Var bCosA = pow(b, cos(a));
    Matrix A(2, 2);
    Vector rhs(2);

    A(0, 0) = 12*a;
    A(0, 1) = a*bCosA + a*c;
    A(1, 0) = 12;
    A(1, 1) = bCosA + c;

    rhs(0) = 10;
    rhs(1) = b;

    solver = Solver(A, rhs);

    CHECK(!solver.hasSolution());
}

TEST(Solver, linearSystemDim2)
{
    Matrix A(3, 3);
    Vector rhs(3);
    Vector x;

    A(0, 0) = pow(a, 2*sin(b));
    A(0, 1) = 17*b/29;
    A(1, 1) = 1/(a*b*c);
    A(1, 2) = pow(12, d);
    A(2, 0) = 1;
    A(2, 1) = 4*a;

    rhs(0) = pow(a, 2*sin(b))*d + 17*a*b/116;
    rhs(1) = pow(b, cos(b))*pow(12, d) + 1/(4*b*c);
    rhs(2) = d + a*a;

    solver.set(A);
    solver.set(rhs);

    CHECK(solver.hasSolution());

    x = solver.solve();

    CHECK_EQUAL(d, x(0));
    CHECK_EQUAL(a/4, x(1));
    CHECK_EQUAL(pow(b, cos(b)), x(2));
}

TEST(Solver, linearSystemDim3)
{
    Matrix A(3, 3);
    Vector rhs(3);
    Vector x;

    A(0, 0) = a;
    A(0, 1) = 17*b/29;
    A(1, 1) = 1/(a*b*c);
    A(1, 2) = pow(12, d);
    A(2, 0) = 1;
    A(2, 1) = 4*a;

    rhs(0) = a*d + 17*a*b/116;
    rhs(1) = b*pow(12, d) + 1/(b*c*4);
    rhs(2) = d + a*a;

    solver.set(A);
    solver.set(rhs);

    CHECK(solver.hasSolution());

    x = solver.solve();

    CHECK_EQUAL(d, x(0));
    CHECK_EQUAL(a/4, x(1));
    CHECK_EQUAL(b, x(2));
}
