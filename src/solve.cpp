
#include "solve.h"
#include <cassert>
#include "directsolve.h"
#include "functions.h"
#include "stdvecwrapper.h"

namespace tsym {
    namespace {
        Var detFromPlu(SquareMatrixAdaptor<>& A, unsigned nPivotSwaps)
        {
            const std::size_t dim = A.dim;
            Var det(nPivotSwaps % 2 == 0 ? 1 : -1);

            for (std::size_t i = 0; i < dim; ++i)
                det *= A(i, i);

            return simplify(det);
        }
    }

    PivotStrategy selectPivot(Algo choice)
    {
        return choice == Algo::Gauss ? &firstNonZeroPivot : &leastComplexityPivot;
    }
}

std::vector<tsym::Var> tsym::detail::solve(std::vector<Var>&& A, std::vector<Var>&& b, std::size_t dim, Algo choice)
{
    VectorAdaptor<> result{std::vector<Var>(dim)};

    if (choice == Algo::Gauss || choice == Algo::GaussLCPivot) {
        std::optional<VectorAdaptor<>> rhs{{std::move(b)}};
        SquareMatrixAdaptor<> coeff{std::move(A), dim};

        eliminateGauss(coeff, rhs, selectPivot(choice));
        computeSolution(coeff, *rhs, result);
    }

    return result.data;
}

tsym::Var tsym::detail::determinant(std::vector<Var>&& A, std::size_t dim, Algo choice)
{
    SquareMatrixAdaptor<> coeff{std::move(A), dim};
    std::optional<VectorAdaptor<>> fakeRhs;

    assert(choice == Algo::Gauss || choice == Algo::GaussLCPivot);

    const unsigned nRowSwaps = eliminateGauss(coeff, fakeRhs, selectPivot(choice));

    return detFromPlu(coeff, nRowSwaps);
}

void tsym::detail::invert(std::vector<Var>& A, std::size_t dim, Algo choice)
{
    const std::vector<Var> coeffOrig = std::move(A);
    SquareMatrixAdaptor<> inverseColumns{std::vector<Var>(dim * dim), dim};

    assert(choice == Algo::Gauss || choice == Algo::GaussLCPivot);

    for (std::size_t i = 0; i < dim; ++i) {
        /* The coefficient matrix should only be factorized once, so will be very inefficient. A
         * better solutions would allow for factorization of coefficient matrices along with a right
         * hand side matrix of arbitrary column size. */
        SquareMatrixAdaptor<> coeff{coeffOrig, dim};
        VectorAdaptor<> column{std::vector<Var>(dim)};
        std::optional<VectorAdaptor<>> optRhs{{std::vector<Var>(dim)}};
        VectorAdaptor<>& rhs = optRhs.value();

        for (std::size_t j = 0; j < dim; ++j)
            rhs(j) = i == j ? 1 : 0;

        eliminateGauss(coeff, optRhs, selectPivot(choice));

        computeSolution(coeff, rhs, column);

        for (std::size_t r = 0; r < dim; ++r)
            inverseColumns(r, i) = std::move(column(r));
    }

    A = std::move(inverseColumns.data);
}
