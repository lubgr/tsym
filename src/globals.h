#ifndef TSYM_GLOBALS_H
#define TSYM_GLOBALS_H

#include <string>
#include "plu.h"
#include "var.h"

namespace tsym {
    /* Central functions and constants, that they are allowed to pollute the global tsym namespace,
     * i.e., common mathematical functions and constants, the interface for parsing expressions and
     * functions for the solution of linear systems of equations and the like. */
    Var sqrt(const Var& base);
    Var pow(const Var& base, const Var& exp);

    Var log(const Var& arg);

    Var sin(const Var& arg);
    Var cos(const Var& arg);
    Var tan(const Var& arg);
    Var asin(const Var& arg);
    Var acos(const Var& arg);
    Var atan(const Var& arg);
    Var atan2(const Var& y, const Var& x);

    const Var& pi();
    const Var& euler();

    template<class Matrix, class Vector, typename SizeType, class MatrixAccess = plu::detail::DefaultAccess<Matrix>,
        class VectorAccess = plu::detail::DefaultAccess<Vector>> void solve(
                Matrix& A, Vector& b, Vector& x, SizeType dim, MatrixAccess&& mAccess = MatrixAccess{}, VectorAccess&& vAccess = VectorAccess{})
    {
        plu::detail::solve(A, std::forward<MatrixAccess>(mAccess), b, x, std::forward<VectorAccess>(vAccess), dim);
    }

    template<class Matrix, typename SizeType, class MatrixAccess = plu::detail::DefaultAccess<Matrix>> Var determinant(
            Matrix& A, SizeType dim, MatrixAccess&& access = MatrixAccess{})
    {
        return plu::detail::determinant(A, std::forward<MatrixAccess>(access), dim);
    }

    template<class Matrix, typename SizeType, class MatrixAccess = plu::detail::DefaultAccess<Matrix>> void invert(
            Matrix& A, SizeType dim, MatrixAccess&& access = MatrixAccess{})
    {
        plu::detail::invert(A, std::forward<MatrixAccess>(access), dim);
    }

    Var parse(const std::string& str, bool *success = nullptr);
}

#endif
