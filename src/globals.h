#ifndef TSYM_FUNCTIONS_H
#define TSYM_FUNCTIONS_H

#include "var.h"

namespace tsym {
    /* Functions and constants being so central, that they are allowed to pollute the global
     * namespace; namely trigonometric and power functions.
     *
     * The distinction between Sqrt/Pow functions for Var arguments/return values and their Number
     * counterparts is meaningful due to the different handling of Number-only powers, as they are
     * always numerically evaluated (in contrast to Var powers). Numeric power functions can be
     * found as static methods of the Number class. An example is Number::Sqrt(2) = 1.41421...,
     * while Sqrt(2) will deliver 2^(1/2) without numeric evaluation. The latter will be the desired
     * kind evaluation in most situations. */
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

    bool stringToVar(const std::string& toBeParsed, Var& result);

    extern const Var Pi;
    extern const Var e;
}

#endif
