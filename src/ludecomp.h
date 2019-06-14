#ifndef TSYM_LUDECOMP_H
#define TSYM_LUDECOMP_H

#include <cstddef>
#include <exception>
#include "functions.h"
#include "var.h"

namespace tsym {
    template <class MatrixView> class LUDecomposition {
      public:
        LUDecomposition(MatrixView& m, std::size_t dim)
            : matrix(m)
            , dim(dim)
        {}

        LUDecomposition(const LUDecomposition&) = delete;
        LUDecomposition(LUDecomposition&&) = delete;
        LUDecomposition& operator=(const LUDecomposition&) = delete;
        LUDecomposition& operator=(LUDecomposition&&) = delete;
        ~LUDecomposition() = default;

        void factorize()
        {
            for (std::size_t j = 0; j + 1 < dim; ++j)
                for (std::size_t i = j + 1; i < dim; ++i) {
                    matrix(i, j) /= matrix(j, j);
                    for (std::size_t k = j + 1; k < dim; ++k)
                        matrix(i, k) -= matrix(i, j) * matrix(j, k);
                }
        }

        template <class RhsVectorView, class SolutionVectorView>
        void computeSolution(RhsVectorView& rhs, SolutionVectorView& x)
        {
            for (std::size_t i = 0; i < dim; ++i)
                x(i) = 0;

            for (std::size_t i = 0; i < dim; ++i)
                for (std::size_t j = 0; j < i; ++j)
                    rhs(i) -= matrix(i, j) * rhs(j);

            for (std::size_t i = dim - 1; i + 1 > 0; --i) {
                for (std::size_t j = i + 1; j < dim; ++j)
                    x(i) -= matrix(i, j) * x(j);

                matrix(i, i) = simplify(matrix(i, i));

                if (matrix(i, i) == 0)
                    throw std::invalid_argument("Coefficient matrix is singular");

                x(i) = simplify((rhs(i) + x(i)) / matrix(i, i));
            }
        }

      private:
        MatrixView& matrix;
        const std::size_t dim;
    };
}

#endif
