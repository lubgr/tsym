#ifndef TSYM_FUNCTIONS_H
#define TSYM_FUNCTIONS_H

#include <string>

namespace tsym {
    class Var;
    class Vector;
    class Matrix;
}

namespace tsym {
    /* Functions and constants central, that they are allowed to pollute the global tsym namespace,
     * i.e., common mathematical functions and constants, the interface for parsing expressions and
     * a shortcut function for the solution of linear systems of equations. */
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

    bool solve(const Matrix& A, const Vector& b, Vector& x);

    Var parse(const std::string& str, bool *success = nullptr);
}

#endif
