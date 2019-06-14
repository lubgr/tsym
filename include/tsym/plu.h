#ifndef TSYM_PLU_H
#define TSYM_PLU_H

#include <iterator>
#include <vector>
#include "traits.h"
#include "var.h"

namespace tsym {
    namespace detail {
        template <class ValueType, class Vector, class SizeType> auto toStdVec(const Vector& src, SizeType dim)
        {
            std::vector<ValueType> result;

            result.reserve(static_cast<std::size_t>(dim));

            for (SizeType i = 0; i < dim; ++i)
                if constexpr (hasConstUnaryCallOp<Vector, SizeType>)
                    result.push_back(src(i));
                else if (hasConstUnaryArrayOp<Vector, SizeType>)
                    result.push_back(src[i]);

            return result;
        }

        template <class ValueType, class Vector, class SizeType>
        auto toStdVec(const Vector& src, SizeType nRows, SizeType nColumns)
        {
            std::vector<ValueType> result;

            result.reserve(static_cast<std::size_t>(nRows * nColumns));

            for (SizeType i = 0; i < nRows; ++i)
                for (SizeType j = 0; j < nColumns; ++j)
                    if constexpr (hasConstBinaryCallOp<Vector, SizeType>)
                        result.push_back(src(i, j));
                    else if (hasConstBinaryArrayOp<Vector, SizeType>)
                        result.push_back(src[i][j]);

            return result;
        }

        template <class ValueType, class Vector, class SizeType>
        void fromStdVec(std::vector<ValueType>&& src, Vector& dest, SizeType dim)
        {
            auto value = std::move_iterator(src.begin());

            for (SizeType i = 0; i < dim; ++i)
                if constexpr (hasUnaryCallOp<Vector, SizeType>)
                    dest(i) = *value++;
                else if (hasUnaryArrayOp<Vector, SizeType>)
                    dest[i] = *value++;
        }

        template <class ValueType, class Matrix, class SizeType>
        void fromStdVec(std::vector<ValueType>&& src, Matrix& dest, SizeType nRows, SizeType nColumns)
        {
            auto value = std::move_iterator(src.begin());

            for (SizeType i = 0; i < nRows; ++i)
                for (SizeType j = 0; j < nColumns; ++j)
                    if constexpr (hasBinaryCallOp<Matrix, SizeType>)
                        dest(i, j) = *value++;
                    else if (hasBinaryArrayOp<Matrix, SizeType>)
                        dest[i][j] = *value++;
        }

        void invert(std::vector<Var>& A, std::size_t dim);
        Var determinant(std::vector<Var>&& A, std::size_t dim);
        std::vector<Var> solve(std::vector<Var>&& A, std::vector<Var>&& b, std::size_t dim);
    }

    template <class Matrix, class RhsVector, class SolutionVector, typename SizeType>
    void solve(const Matrix& A, const RhsVector& b, SolutionVector& x, SizeType dim)
    {
        std::vector<Var> coeff = detail::toStdVec<Var>(A, dim, dim);
        std::vector<Var> rhs = detail::toStdVec<Var>(b, dim);

        std::vector<Var> result = detail::solve(std::move(coeff), std::move(rhs), static_cast<std::size_t>(dim));

        detail::fromStdVec(std::move(result), x, dim);
    }

    template <class Matrix, typename SizeType> Var determinant(const Matrix& A, SizeType dim)
    {
        std::vector<Var> vecA = detail::toStdVec<Var>(A, dim, dim);

        return detail::determinant(std::move(vecA), static_cast<std::size_t>(dim));
    }

    template <class Matrix, typename SizeType> void invert(Matrix& A, SizeType dim)
    {
        std::vector<Var> vecA = detail::toStdVec<Var>(A, dim, dim);

        detail::invert(vecA, static_cast<std::size_t>(dim));

        detail::fromStdVec(std::move(vecA), A, dim, dim);
    }
}

#endif
