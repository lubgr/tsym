#ifndef TSYM_PLU_H
#define TSYM_PLU_H

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>
#include "traits.h"
#include "var.h"

namespace tsym {
    namespace detail {
        template <class SkipFieldVector, class SizeType> auto toSkipField(const SkipFieldVector& src, SizeType dim)
        {
            std::vector<bool> result;

            result.reserve(static_cast<std::size_t>(dim));

            for (SizeType i = 0; i < dim; ++i)
                if constexpr (hasConstUnaryCallOp<SkipFieldVector, SizeType>)
                    result.push_back(src(i));
                else if (hasConstUnaryArrayOp<SkipFieldVector, SizeType>)
                    result.push_back(src[i]);

            return result;
        }

        template <>
        inline auto toSkipField<std::vector<bool>, int>(const std::vector<bool>& src, [[maybe_unused]] int dim)
        {
            assert(static_cast<std::size_t>(dim) == src.size());

            return src;
        }

        template <>
        inline auto toSkipField<std::vector<bool>, std::size_t>(
          const std::vector<bool>& src, [[maybe_unused]] std::size_t dim)
        {
            assert(dim == src.size());

            return src;
        }

        template <class SizeType> auto defaultSkip(SizeType dim)
        {
            return std::vector<bool>(static_cast<std::size_t>(dim), false);
        }

        template <class ValueType, class Vector, class SizeType>
        auto toStdVec(const Vector& src, const std::vector<bool>& skip, SizeType dim)
        {
            std::vector<ValueType> result;

            result.reserve(static_cast<std::size_t>(dim));

            for (SizeType i = 0; i < dim; ++i) {
                if (skip[static_cast<std::size_t>(i)])
                    continue;

                if constexpr (hasConstUnaryCallOp<Vector, SizeType>)
                    result.push_back(src(i));
                else if (hasConstUnaryArrayOp<Vector, SizeType>)
                    result.push_back(src[i]);
            }

            return result;
        }

        template <class ValueType, class Vector, class SizeType>
        auto toStdVec(const Vector& src, const std::vector<bool>& skip, SizeType nRows, SizeType nColumns)
        {
            std::vector<ValueType> result;

            result.reserve(static_cast<std::size_t>(nRows * nColumns));

            for (SizeType i = 0; i < nRows; ++i) {
                if (skip[static_cast<std::size_t>(i)])
                    continue;

                for (SizeType j = 0; j < nColumns; ++j) {
                    if (skip[static_cast<std::size_t>(j)])
                        continue;

                    if constexpr (hasConstBinaryCallOp<Vector, SizeType>)
                        result.push_back(src(i, j));
                    else if (hasConstBinaryArrayOp<Vector, SizeType>)
                        result.push_back(src[i][j]);
                }
            }

            return result;
        }

        template <class ValueType, class Vector, class SizeType>
        void fromStdVec(std::vector<ValueType>&& src, Vector& dest, const std::vector<bool>& skip, SizeType dim)
        {
            auto value = std::move_iterator(src.begin());

            for (SizeType i = 0; i < dim; ++i) {
                if (skip[static_cast<std::size_t>(i)])
                    continue;
                if constexpr (hasUnaryCallOp<Vector, SizeType>)
                    dest(i) = *value++;
                else if (hasUnaryArrayOp<Vector, SizeType>)
                    dest[i] = *value++;
            }
        }

        template <class ValueType, class Matrix, class SizeType>
        void fromStdVec(
          std::vector<ValueType>&& src, Matrix& dest, const std::vector<bool>& skip, SizeType nRows, SizeType nColumns)
        {
            auto value = std::move_iterator(src.begin());

            for (SizeType i = 0; i < nRows; ++i) {
                if (skip[static_cast<std::size_t>(i)])
                    continue;

                for (SizeType j = 0; j < nColumns; ++j) {
                    if (skip[static_cast<std::size_t>(j)])
                        continue;

                    if constexpr (hasBinaryCallOp<Matrix, SizeType>)
                        dest(i, j) = *value++;
                    else if (hasBinaryArrayOp<Matrix, SizeType>)
                        dest[i][j] = *value++;
                }
            }
        }

        void invert(std::vector<Var>& A, std::size_t dim);
        Var determinant(std::vector<Var>&& A, std::size_t dim);
        std::vector<Var> solve(std::vector<Var>&& A, std::vector<Var>&& b, std::size_t dim);
    }

    template <class Matrix, class RhsVector, class SolutionVector, class SkipField, typename SizeType>
    void solve(const Matrix& A, const RhsVector& b, SolutionVector& x, const SkipField& sf, SizeType dim)
    {
        const std::vector<bool> skip = detail::toSkipField(sf, dim);
        std::vector<Var> coeff = detail::toStdVec<Var>(A, skip, dim, dim);
        std::vector<Var> rhs = detail::toStdVec<Var>(b, skip, dim);
        const std::size_t reducedDim = rhs.size();

        std::vector<Var> result = detail::solve(std::move(coeff), std::move(rhs), reducedDim);

        detail::fromStdVec(std::move(result), x, sf, dim);
    }

    template <class Matrix, class RhsVector, class SolutionVector, typename SizeType>
    void solve(const Matrix& A, const RhsVector& b, SolutionVector& x, SizeType dim)
    {
        solve(A, b, x, detail::defaultSkip(dim), dim);
    }

    template <class Matrix, class SkipField, typename SizeType>
    Var determinant(const Matrix& A, const SkipField& sf, SizeType dim)
    {
        const std::vector<bool> skip = detail::toSkipField(sf, dim);
        const auto reducedDim = std::count(skip.cbegin(), skip.cend(), false);
        std::vector<Var> vecA = detail::toStdVec<Var>(A, skip, dim, dim);

        return detail::determinant(std::move(vecA), static_cast<std::size_t>(reducedDim));
    }

    template <class Matrix, typename SizeType> Var determinant(const Matrix& A, SizeType dim)
    {
        return determinant(A, detail::defaultSkip(dim), dim);
    }

    template <class Matrix, class SkipField, typename SizeType>
    void invert(Matrix& A, const SkipField& sf, SizeType dim)
    {
        const std::vector<bool> skip = detail::toSkipField(sf, dim);
        const auto reducedDim = std::count(skip.cbegin(), skip.cend(), false);
        std::vector<Var> vecA = detail::toStdVec<Var>(A, skip, dim, dim);

        detail::invert(vecA, static_cast<std::size_t>(reducedDim));

        detail::fromStdVec(std::move(vecA), A, skip, dim, dim);
    }

    template <class Matrix, typename SizeType> void invert(Matrix& A, SizeType dim)
    {
        return invert(A, detail::defaultSkip(dim), dim);
    }
}

#endif
