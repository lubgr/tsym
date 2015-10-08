
#include "solver.h"
#include "logging.h"

tsym::Solver::Solver() :
    A(NULL),
    b(NULL)
{}

tsym::Solver::Solver(const Matrix& A, const Vector& rhs) :
    A(&A),
    b(&rhs)
{}

void tsym::Solver::set(const Matrix& A)
{
    this->A = &A;
}

void tsym::Solver::set(const Vector& rhs)
{
    b = &rhs;
}

bool tsym::Solver::hasSolution() const
{
    if (hasRequirements())
        return !A->det().isZero();
    else
        return false;
}

bool tsym::Solver::hasRequirements() const
{
    if (A == NULL || b == NULL)
        logging::error() << "Coefficient matrix or right hand side not set!";
    else if (!A->isSquare())
        logging::error() << "Matrix (" << A->rowSize() << ", " << A->colSize() << ") isn't square!";
    else if (b->size() != A->rowSize())
        logging::error() << "Matrix dim. " << A->rowSize() << " must match vec. size " << b->size();
    else if (b->size() == 0)
        logging::error() << "Matrix and right and side with zero dimension!";
    else
        return true;

    return false;
}

const tsym::Vector& tsym::Solver::solve()
{
    if (hasRequirements())
        x = A->solve(*b);

    return x;
}
