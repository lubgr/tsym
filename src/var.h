#ifndef TSYM_VAR_H
#define TSYM_VAR_H

#include <vector>
#include <string>

namespace tsym { class BasePtr; }

namespace tsym {
    class Var {
        /* Wrapper around numbers, symbols, functions, constants and expressions with one of those,
         * or undefined. There are implicit constructors for integer and double numerics, symbols
         * (possibly > 0 by given flag) must be explicitly constructed with a symbol name. Besides
         * the few global free functions, every interaction with symbolic expressions is either
         * possible by comparison or algebraic operators or by member functions (differentiation,
         * normalization, expansion, requesting the type etc.). Numbers can be converted to plain
         * double or, if they fit, into integers (numerator and/or denominator). */
        public:
            enum class Sign { POSITIVE = 1 };
            enum class Type { SYMBOL, INT, FRACTION, DOUBLE, CONSTANT, UNDEFINED, FUNCTION, SUM,
                PRODUCT, POWER };

            Var();
            Var(int value);
            Var(double value);
            Var(int numerator, int denominator);
            /* The next two constructors create symbols or (possibly big) integers. For symbols,
             * alphanumeric characters are valid, though the symbol name must not start with a
             * digit. It can contain subscripts though: if it's only one character, a_1 is
             * sufficient, for longer subscripts, use a_{10}. */
            explicit Var(const char *str);
            explicit Var(const char *str, Sign sign);
            /* To be used only internally: */
            explicit Var(const BasePtr& ptr);
            Var(const Var& other);
            Var& operator = (const Var& rhs);
            ~Var();

            Var& operator += (const Var& rhs);
            Var& operator -= (const Var& rhs);
            Var& operator *= (const Var& rhs);
            Var& operator /= (const Var& rhs);

            const Var& operator + () const;
            Var operator - () const;

            Var toThe(const Var& exponent) const;

            Var subst(const Var& from, const Var& to) const;
            Var expand() const;
            Var normal() const;
            /* The argument must be a Symbol: */
            Var diff(const Var& symbol) const;
            bool equal(const Var& other) const;
            bool has(const Var& other) const;
            bool isZero() const;
            bool isPositive() const;
            bool isNegative() const;
            Type type() const;
            Var numerator() const;
            Var denominator() const;
            bool fitsIntoInt() const;
            int toInt() const;
            double toDouble() const;
            /* Returns names for Symbols, Functions and Constants, an empty string otherwise: */
            const std::string& name() const;
            std::vector<Var> operands() const;
            std::vector<Var> collectSymbols() const;
            const BasePtr& getBasePtr() const;

        private:
            Type numericType() const;
            bool isInteger() const;
            std::pair<Var, Var> normalToFraction() const;
            void collectSymbols(const BasePtr& ptr, std::vector<Var>& symbols) const;
            void insertSymbolIfNotPresent(const BasePtr& symbol, std::vector<Var>& symbols) const;

            BasePtr *rep;
    };

    bool operator == (const Var& lhs, const Var& rhs);
    bool operator != (const Var& lhs, const Var& rhs);

    Var operator + (Var lhs, const Var& rhs);
    Var operator - (Var lhs, const Var& rhs);
    Var operator * (Var lhs, const Var& rhs);
    Var operator / (Var lhs, const Var& rhs);

    std::ostream& operator << (std::ostream& stream, const Var& rhs);
    std::ostream& operator << (std::ostream& stream, const Var::Type& rhs);
}

#endif
