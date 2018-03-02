#ifndef TSYM_PLU_H
#define TSYM_PLU_H

#include <functional>
#include <numeric>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <set>
#include <memory>
#include <type_traits>
#include <vector>
#include "var.h"

namespace tsym {
    namespace plu {
        namespace detail {
            template<typename SizeType> auto toUnsigned(SizeType n)
            {
                return static_cast<typename std::make_unsigned<SizeType>::type>(n);
            }

            template<class Container> class DefaultAccess {
                public:
                    template<typename SizeType> Var& operator () (Container& matrix, SizeType i, SizeType j)
                    {
                        return access(matrix, i, j, 0);
                    }

                    template<typename SizeType> Var& operator () (Container& vec, SizeType i)
                    {
                        return access(vec, i, 0);
                    }

                private:
                    template<class Matrix, typename SizeType> auto access(Matrix& m, SizeType i, SizeType j, int) -> decltype(m(i, j))
                    {
                        return m(i, j);
                    }

                    template<class Matrix, typename SizeType> auto access(Matrix& m, SizeType i, SizeType j, long) -> decltype(m[i][j])
                    {
                        return m[i][j];
                    }

                    template<class Vector, typename SizeType> auto access(Vector& vec, SizeType i, int) -> decltype(vec(i))
                    {
                        return vec(i);
                    }

                    template<class Vector, typename SizeType> auto access(Vector& vec, SizeType i, long) -> decltype(vec[i])
                    {
                        return vec[i];
                    }
            };

            template<class Vector, typename SizeType> class VectorAccessProxy {
                public:
                    explicit VectorAccessProxy(Vector& vec) :
                        vec(vec)
                {}
                    template<class Callable> VectorAccessProxy(Vector& vec, Callable&& accessFct) :
                        vec(vec),
                        access(std::forward<Callable>(accessFct))
                {}

                    virtual ~VectorAccessProxy() = default;

                    Var& operator () (SizeType i)
                    {
                        return access(vec, i);
                    }

                    const Var& operator () (SizeType i) const
                    {
                        return access(vec, i);
                    }

                private:
                    Vector& vec;
                    const std::function<tsym::Var& (Vector& m, SizeType i)> access = DefaultAccess<Vector>{};
            };

            template<class Matrix, typename SizeType> class MatrixAccessProxy {
                public:
                    explicit MatrixAccessProxy(Matrix& matrix) :
                        m(matrix)
                {}
                    template<class Callable> MatrixAccessProxy(Matrix& matrix, Callable&& accessFct) :
                        m(matrix),
                        access(std::forward<Callable>(accessFct))
                {}

                    virtual ~MatrixAccessProxy() = default;

                    Var& operator () (SizeType i, SizeType j)
                    {
                        return access(m, i, j);
                    }

                    const Var& operator () (SizeType i, SizeType j) const
                    {
                        return access(m, i, j);
                    }

                private:
                    Matrix& m;
                    const std::function<tsym::Var& (Matrix& m, SizeType i, SizeType j)> access = DefaultAccess<Matrix>{};
            };

            template<typename SizeType> class VectorWrapper : public VectorAccessProxy<std::unique_ptr<Var[]>, SizeType> {
                public:
                    explicit VectorWrapper(SizeType dim) :
                        VectorAccessProxy<std::unique_ptr<Var[]>, SizeType>(rep,
                                [dim](decltype(rep)& m, SizeType i) -> Var& { return m.get()[i]; }),
                        rep(new Var[dim])
                {}

                private:
                    std::unique_ptr<Var[]> rep;
            };

            template<typename SizeType> class SquareMatrixWrapper : public MatrixAccessProxy<std::unique_ptr<Var[]>, SizeType> {
                public:
                    explicit SquareMatrixWrapper(SizeType dim) :
                        MatrixAccessProxy<decltype(rep), SizeType>(rep,
                                [dim](decltype(rep)& m, SizeType i, SizeType j) -> Var& { return m.get()[i*dim + j]; }),
                        rep(new Var[dim*dim]),
                        dim(dim)
                {}

                    VectorAccessProxy<std::unique_ptr<Var[]>, SizeType> columnProxy(SizeType col)
                    {
                        const SizeType localDim{dim};
                        auto accessFct = [col, localDim](decltype(rep)& vec, SizeType i) -> Var& { return vec[col + i*localDim]; };

                        return VectorAccessProxy<std::unique_ptr<Var[]>, SizeType>(rep, std::move(accessFct));
                    }

                private:
                    std::unique_ptr<Var[]> rep;
                    const SizeType dim;
            };

            template<class MatrixProxy, typename SizeType> class LeastComplexityPivot {
                public:
                    LeastComplexityPivot(const MatrixProxy& m, SizeType dim) :
                        pivotIndices(toUnsigned(dim)),
                        dim(dim)
                {
                    collectAndSort(m, pivotIndices);
                    selectPivots(pivotIndices);
                }

                    unsigned nRowSwaps() const
                    {
                        return swapCount(pivotIndices);
                    }

                    void applyTo(MatrixProxy& m) const
                    {
                        SquareMatrixWrapper<SizeType> origMatrix(dim);

                        copyMatrix(m, origMatrix);

                        for (SizeType i = 0; i < dim; ++i)
                            for (SizeType j = 0; j < dim; ++j)
                                m(pivotIndices[toUnsigned(i)].front(), j) = origMatrix(i, j);
                    }

                    template<class RhsVectorProxy> void applyTo(RhsVectorProxy& rhs) const
                    {
                        VectorWrapper<SizeType> origRhs(dim);

                        copyVector(rhs, origRhs);

                        for (SizeType i = 0; i < dim; ++i)
                            rhs(pivotIndices[toUnsigned(i)].front()) = origRhs(i);
                    }

                private:
                    void collectAndSort(const MatrixProxy& matrix, std::vector<std::vector<SizeType>>& indices) const
                    {
                        for (SizeType i = 0; i < dim; ++i)
                            collectAndSort(matrix, i, indices[toUnsigned(i)]);
                    }

                    void collectAndSort(const MatrixProxy& matrix, SizeType i, std::vector<SizeType>& lineIndices) const
                    {
                        auto sortByComplexity = [i, &matrix] (SizeType pivotA, SizeType pivotB) {
                            return matrix(i, pivotA).complexity() < matrix(i, pivotB).complexity();
                        };

                        for (SizeType j = 0; j < dim; ++j)
                            if (!matrix(i, j).isZero())
                                lineIndices.push_back(j);

                        assert(lineIndices.size() > 0);

                        std::sort(begin(lineIndices), end(lineIndices), sortByComplexity);
                    }

                    void selectPivots(std::vector<std::vector<SizeType>>& indices) const
                        /* Decides how lines are reordered based on pivot, i.e., divisor complexity. As conflicts might
                         * appear, permutation and recursion continue until a pivot distribution is found. This is a
                         * trial-and-error logic, and there is no guarantee that the selected reordering yields the lowest
                         * pivot complexity. However, as long as the coefficient matrix is not singular, an order is
                         * provided that ensures no division by zero will happen during LU decomposition. */
                    {
                        std::vector<SizeType> selectedPivots(toUnsigned(dim));
                        std::set<SizeType> targetLineIndices;

                        for (SizeType i = 0; i < dim; ++i)
                            for (const auto& pivotIndex : indices[toUnsigned(i)])
                                if (targetLineIndices.count(pivotIndex) == 0) {
                                    targetLineIndices.insert(pivotIndex);
                                    selectedPivots[toUnsigned(i)] = pivotIndex;
                                    break;
                                }

                        if (isLineMissing(targetLineIndices)) {
                            std::rotate(begin(indices), begin(indices) + 1, end(indices));
                            selectPivots(indices);
                            std::rotate(rbegin(indices), rbegin(indices) + 1, rend(indices));
                        } else
                            for (SizeType i = 0; i < dim; ++i)
                                indices[toUnsigned(i)] = { selectedPivots[toUnsigned(i)] };
                    }

                    bool isLineMissing(std::set<SizeType>& pivots) const
                    {
                        for (SizeType i = 0; i < dim; ++i)
                            if (pivots.find(i) == cend(pivots))
                                return true;

                        return false;
                    }

                    void copyMatrix(const MatrixProxy& from, SquareMatrixWrapper<SizeType>& to) const
                    {
                        for (SizeType i = 0; i < dim; ++i)
                            for (SizeType j = 0; j < dim; ++j)
                                to(i, j) = from(i, j);
                    }

                    template<class RhsVectorProxy> void copyVector(const RhsVectorProxy& from, VectorWrapper<SizeType>& to) const
                    {
                        for (SizeType i = 0; i < dim; ++i)
                            to(i) = from(i);
                    }

                    unsigned swapCount(std::vector<std::vector<SizeType>> indices) const
                        /* Uses the selected indices as a sparse representation of a perturbed unit matrix and swaps rows
                         * until the original distribution of diagonal elements is retained. */
                    {
                        unsigned count = 0;

                        for (SizeType i = 0; i < dim; ++i) {
                            if (indices[toUnsigned(i)][0] == i)
                                continue;

                            for (SizeType n = i + 1; n < dim; ++n)
                                if (i == indices[toUnsigned(n)][0]) {
                                    std::swap(indices[toUnsigned(n)], indices[toUnsigned(i)]);
                                    ++count;
                                    break;
                                }
                        }

                        return count;
                    }

                private:
                    std::vector<std::vector<SizeType>> pivotIndices;
                    const SizeType dim;
            };

            template<class MatrixProxy, typename SizeType> class FirstNonZeroPivot {
                public:
                    FirstNonZeroPivot(const MatrixProxy& m, SizeType dim) :
                        newRowIndices(toUnsigned(dim)),
                        dim(dim)
                {
                    for (SizeType j = 0; j + 1 < dim; ++j) {
                        if (!m(j, j).isZero()) {
                            newRowIndices[toUnsigned(j)] = j;
                            continue;
                        }

                        for (SizeType i = j + 1; i < dim; ++i)
                            if (!m(i, j).isZero()) {
                                newRowIndices[toUnsigned(j)] = i;

                                ++swapCount;

                                break;
                            }
                    }
                }

                    unsigned nRowSwaps() const
                    {
                        return swapCount;
                    }

                    void applyTo(MatrixProxy& m) const
                    {
                        auto newIndices(newRowIndices);

                        for (SizeType i = 0; i + 1 < dim; ++i) {
                            auto targetRow = newRowIndices[toUnsigned(i)];

                            if (i != targetRow) {
                                swapRows(m, i, targetRow);

                                /* Make sure the rows are swapped only once: */
                                newIndices[toUnsigned(targetRow)] = targetRow;
                            }
                        }
                    }

                    template<class RhsVectorProxy> void applyTo(RhsVectorProxy& rhs) const
                    {
                        auto newIndices(newRowIndices);

                        for (SizeType i = 0; i + 1 < dim; ++i) {
                            auto targetRow = newRowIndices[toUnsigned(i)];

                            if (i != targetRow) {
                                std::swap(rhs(i), rhs(targetRow));

                                /* See above. */
                                newIndices[toUnsigned(targetRow)] = targetRow;
                            }
                        }
                    }

                private:
                    void swapRows(MatrixProxy& m, SizeType index1, SizeType index2) const
                    {
                        for (SizeType j = 0; j < dim; ++j)
                            std::swap(m(index1, j), m(index2, j));
                    }

                    std::vector<SizeType> newRowIndices;
                    unsigned swapCount = 0;
                    const SizeType dim;
            };

            template<class MatrixProxy, typename SizeType> class LUDecomposition {
                public:
                    LUDecomposition(MatrixProxy& m, SizeType dim) :
                        matrix(m),
                        dim(dim)
                {}

                    void factorize()
                    {
                        for (SizeType j = 0; j + 1 < dim; ++j)
                            for (SizeType i = j + 1; i < dim; ++i) {
                                matrix(i, j) /= matrix(j, j);
                                for (SizeType k = j + 1; k < dim; ++k)
                                    matrix(i, k) -= matrix(i, j)*matrix(j, k);
                            }
                    }

                    template<class VectorProxyRhs, class VectorProxySolution> void computeSolution(VectorProxyRhs&& rhs, VectorProxySolution&& x)
                    {
                        for (SizeType i = 0; i < dim; ++i)
                            x(i) = 0;

                        for (SizeType i = 0; i < dim; ++i)
                            for (SizeType j = 0; j < i; ++j)
                                rhs(i) -= matrix(i, j)*rhs(j);

                        for (SizeType i = dim - 1; i + 1 > 0; --i) {
                            for (SizeType j = i + 1; j < dim; ++j)
                                x(i) -= matrix(i, j)*x(j);

                            matrix(i, i) = matrix(i, i).simplify();

                            if (matrix(i, i).isZero())
                                throw std::invalid_argument("Coefficient matrix is singular");

                            x(i) = ((rhs(i) + x(i))/matrix(i, i)).simplify();
                        }
                    }

                private:
                    MatrixProxy matrix;
                    const SizeType dim;
            };

            template<class MatrixProxy, class VectorProxy, typename SizeType> void solveViaProxies(MatrixProxy& A, VectorProxy& b, VectorProxy& x, SizeType dim)
            {
                const LeastComplexityPivot<MatrixProxy, SizeType> pivoting(A, dim);
                LUDecomposition<MatrixProxy, SizeType> luDecomp(A, dim);

                pivoting.applyTo(A);
                pivoting.applyTo(b);
                luDecomp.factorize();

                luDecomp.computeSolution(b, x);
            }

            template<class Matrix, class MatrixAccess, class Vector, class VectorAccess, typename SizeType> void solve(Matrix& A, MatrixAccess&& mAccess, Vector& b, Vector& x, VectorAccess&& vAccess, SizeType dim)
            {
                MatrixAccessProxy<Matrix, SizeType> aProxy(A, std::forward<MatrixAccess>(mAccess));
                VectorAccessProxy<Vector, SizeType> bProxy(b, std::forward<VectorAccess>(vAccess));
                decltype(bProxy) xProxy(x, vAccess);

                solveViaProxies(aProxy, bProxy, xProxy, dim);
            }

            template<class MatrixProxy, typename SizeType> Var detFromPlu(MatrixProxy& A, SizeType dim, unsigned nPivotSwaps)
            {
                Var det(nPivotSwaps % 2 == 0 ? 1 : -1);

                for (SizeType i = 0; i < dim; ++i)
                    det *= A(i, i);

                return det.simplify();
            }

            template<class MatrixProxy, typename SizeType> Var detViaProxy(MatrixProxy& matrix, SizeType dim)
            {
                const LeastComplexityPivot<MatrixProxy, SizeType> pivoting(matrix, dim);
                LUDecomposition<MatrixProxy, SizeType> luDecomp(matrix, dim);

                pivoting.applyTo(matrix);
                luDecomp.factorize();

                unsigned nSwaps = pivoting.nRowSwaps();

                return detFromPlu(matrix, dim, nSwaps);
            }

            template<class Matrix, class MatrixAccess, typename SizeType> Var determinant(Matrix& matrix, MatrixAccess&& access, SizeType dim)
            {
                plu::detail::MatrixAccessProxy<Matrix, SizeType> mProxy(matrix, std::forward<MatrixAccess>(access));

                return detViaProxy(mProxy, dim);
            }

            template<class MatrixProxy, typename SizeType> void invertViaProxy(MatrixProxy& matrix, SizeType dim)
            {
                const LeastComplexityPivot<MatrixProxy, SizeType> pivots(matrix, dim);
                LUDecomposition<MatrixProxy, SizeType> luDecomp(matrix, dim);
                SquareMatrixWrapper<SizeType> inverseColumns(dim);
                VectorWrapper<SizeType> rhsProxy(dim);

                pivots.applyTo(matrix);
                luDecomp.factorize();

                for (SizeType i = 0; i < dim; ++i) {

                    for (SizeType j = 0; j < dim; ++j)
                        rhsProxy(j) = i == j ? 1 : 0;

                    pivots.applyTo(rhsProxy);
                    luDecomp.computeSolution(rhsProxy, inverseColumns.columnProxy(i));
                }

                for (SizeType i = 0; i < dim; ++i)
                    for (SizeType j = 0; j < dim; ++j)
                        matrix(i, j) = inverseColumns(i, j);
            }

            template<class Matrix, class MatrixAccess, typename SizeType> void invert(Matrix& matrix, MatrixAccess&& access, SizeType dim)
            {
                plu::detail::MatrixAccessProxy<Matrix, SizeType> mProxy(matrix, std::forward<MatrixAccess>(access));

                plu::detail::invertViaProxy(mProxy, dim);
            }
        }
    }
}

#endif
