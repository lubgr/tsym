
#include "directsolve.h"
#include <limits>
#include "functions.h"

std::size_t tsym::firstNonZeroPivot(const SquareMatrixAdaptor<>& coeff, std::size_t row)
{
    for (std::size_t i = row; i < coeff.dim; ++i)
        if (coeff(i, row) != 0)
            return i;

    throw std::invalid_argument("Coefficient matrix is singular");

    return 0;
}

std::size_t tsym::leastComplexityPivot(const SquareMatrixAdaptor<>& coeff, std::size_t row)
{
    auto leastComplexity = std::numeric_limits<unsigned>::max();
    std::size_t idx = row;

    for (std::size_t i = row; i < coeff.dim; ++i) {
        const Var& diag = coeff(i, row);
        const unsigned comp = complexity(diag);

        if (diag != 0 && comp < leastComplexity) {
            leastComplexity = comp;
            idx = i;
        }
    }

    return idx;
}

namespace tsym {
    namespace {
        void swapRows(SquareMatrixAdaptor<>& coeff, std::size_t from, std::size_t to)
        {
            for (std::size_t j = 0; j < coeff.dim; ++j)
                std::swap(coeff(from, j), coeff(to, j));
        }

        void swapScalar(VectorAdaptor<>& v, std::size_t from, std::size_t to)
        {
            std::swap(v(from), v(to));
        }
    }
}

unsigned tsym::eliminateGauss(SquareMatrixAdaptor<>& coeff, std::optional<VectorAdaptor<>>& rhs, PivotStrategy piv)
{
    const std::size_t dim = coeff.dim;
    unsigned rowSwaps = 0;

    for (std::size_t j = 0; j + 1 < dim; ++j) {
        const std::size_t pivIndex = piv(coeff, j);

        if (pivIndex != j) {
            ++rowSwaps;

            swapRows(coeff, j, pivIndex);

            if (rhs)
                swapScalar(*rhs, j, pivIndex);
        }

        for (std::size_t i = j + 1; i < dim; ++i) {
            coeff(i, j) = simplify(coeff(i, j) / coeff(j, j));

            for (std::size_t k = j + 1; k < dim; ++k)
                coeff(i, k) = simplify(coeff(i, k) - coeff(i, j) * coeff(j, k));
        }
    }

    return rowSwaps;
}

void tsym::computeSolution(SquareMatrixAdaptor<>& coeff, VectorAdaptor<>& rhs, VectorAdaptor<>& x)
{
    const std::size_t dim = coeff.dim;

    for (std::size_t i = 0; i < dim; ++i)
        x(i) = 0;

    for (std::size_t i = 0; i < dim; ++i)
        for (std::size_t j = 0; j < i; ++j)
            rhs(i) -= coeff(i, j) * rhs(j);

    for (std::size_t i = dim - 1; i + 1 > 0; --i) {
        for (std::size_t j = i + 1; j < dim; ++j)
            x(i) -= coeff(i, j) * x(j);

        coeff(i, i) = simplify(coeff(i, i));

        if (coeff(i, i) == 0)
            throw std::invalid_argument("Coefficient matrix is singular");

        x(i) = simplify((rhs(i) + x(i)) / coeff(i, i));
    }
}
