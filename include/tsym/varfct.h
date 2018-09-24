#ifndef TSYM_VARFCT_H
#define TSYM_VARFCT_H

#include <optional>
#include <string>
#include <string_view>
#include <vector>
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

    Var subst(const Var& arg, const Var& from, const Var& to);
    Var expand(const Var& arg);
    Var normal(const Var& arg);
    /* Determines the simplest representation, currently by comparing the expanded with the
     * normalized one: */
    Var simplify(const Var& arg);
    /* The argument must be a Symbol: */
    Var diff(const Var& arg, const Var& symbol);
    bool has(const Var& arg, const Var& what);
    bool isPositive(const Var& arg);
    bool isNegative(const Var& arg);
    unsigned complexity(const Var& arg);
    Var numerator(const Var& arg);
    Var denominator(const Var& arg);
    /* Returns names for Symbols, Functions and Constants, an empty string otherwise: */
    std::string name(const Var& arg);
    std::vector<Var> operands(const Var& arg);
    std::vector<Var> collectSymbols(const Var& arg);

    std::optional<Var> parse(std::string_view str);
}

#endif
