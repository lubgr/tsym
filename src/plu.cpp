
#include "plu.h"
#include "leastcomplexitypivot.h"
#include "ludecomp.h"
#include "stdvecwrapper.h"

namespace tsym {
    namespace {
        void invertPlu(MatrixAdaptor& matrix, std::size_t dim)
        {
            const LeastComplexityPivot pivots(matrix, dim);
            LUDecomposition<MatrixAdaptor> luDecomp(matrix, dim);
            MatrixAdaptor inverseColumns{std::vector<Var>(dim * dim), dim};
            VectorAdaptor rhs{std::vector<Var>(dim)};

            pivots.applyTo(matrix);
            luDecomp.factorize();

            for (std::size_t i = 0; i < dim; ++i) {
                for (std::size_t j = 0; j < dim; ++j)
                    rhs(j) = i == j ? 1 : 0;

                auto columnProxy = [ col = i, &inverseColumns ](auto i) -> auto&
                {
                    return inverseColumns(i, col);
                };

                pivots.applyTo(rhs);
                luDecomp.computeSolution(rhs, columnProxy);
            }

            for (std::size_t i = 0; i < dim; ++i)
                for (std::size_t j = 0; j < dim; ++j)
                    matrix(i, j) = inverseColumns(i, j);
        }

        Var detFromPlu(MatrixAdaptor& A, std::size_t dim, unsigned nPivotSwaps)
        {
            Var det(nPivotSwaps % 2 == 0 ? 1 : -1);

            for (std::size_t i = 0; i < dim; ++i)
                det *= A(i, i);

            return simplify(det);
        }

        void solvePlu(MatrixAdaptor& A, VectorAdaptor& b, VectorAdaptor& x, std::size_t dim)
        {
            const LeastComplexityPivot pivoting(A, dim);
            LUDecomposition<MatrixAdaptor> luDecomp(A, dim);

            pivoting.applyTo(A);
            pivoting.applyTo(b);
            luDecomp.factorize();

            luDecomp.computeSolution(b, x);
        }
    }
}

void tsym::detail::invert(std::vector<Var>& A, std::size_t dim)
{
    MatrixAdaptor coeff{std::move(A), dim};

    invertPlu(coeff, dim);

    A = std::move(coeff.data);
}

std::vector<tsym::Var> tsym::detail::solve(std::vector<Var>&& A, std::vector<Var>&& b, std::size_t dim)
{
    MatrixAdaptor coeff{std::move(A), dim};
    VectorAdaptor rhs{std::move(b)};
    VectorAdaptor result{std::vector<Var>(dim)};

    solvePlu(coeff, rhs, result, dim);

    return result.data;

    return {Var("b"), Var("a")};
}

tsym::Var tsym::detail::determinant(std::vector<Var>&& A, std::size_t dim)
{
    MatrixAdaptor adaptedA{std::move(A), dim};
    const LeastComplexityPivot pivoting(adaptedA, dim);
    LUDecomposition<MatrixAdaptor> luDecomp(adaptedA, dim);

    pivoting.applyTo(adaptedA);
    luDecomp.factorize();

    return detFromPlu(adaptedA, dim, pivoting.nRowSwaps());
}
