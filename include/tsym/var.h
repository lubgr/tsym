#ifndef TSYM_VAR_H
#define TSYM_VAR_H

#include <memory>
#include <string>

namespace tsym {
    class Base;
}

namespace tsym {
    class Var {
        /* Wrapper around numbers, symbols, functions, constants and expressions with one of those,
         * or undefined. There are implicit constructors for integer and double numerics, symbols
         * (possibly > 0 by given flag) must be explicitly constructed with a name. Every
         * interaction with symbolic expressions is via associated operators (addition, subtraction,
         * multiplication, division). As this class is not more than a wrapper, all other
         * functionality (differentiation, simplification etc.) is invoked by calling free
         * functions. Var objects holding numbers can be explicitely converted to plain double or,
         * if they fit, into integers. */
      public:
        enum class Sign : bool { POSITIVE = 1 };
        enum class Type { SYMBOL, INT, FRACTION, DOUBLE, CONSTANT, UNDEFINED, FUNCTION, SUM, PRODUCT, POWER };

        Var();
        Var(int value); // NOLINT
        Var(double value); // NOLINT
        Var(int numerator, int denominator);
        /* The next two constructors create symbols or (possibly big) integers. For symbols,
         * alphanumeric characters are valid, though the symbol name must not start with a
         * digit. It can contain subscripts: if it's only one character, a_1 is sufficient, for
         * longer subscripts, use a_{10}. */
        explicit Var(const std::string& str);
        explicit Var(const std::string& str, Sign sign);

        Var& operator+=(const Var& rhs);
        Var& operator-=(const Var& rhs);
        Var& operator*=(const Var& rhs);
        Var& operator/=(const Var& rhs);

        const Var& operator+() const;
        Var operator-() const;

        Type type() const;

        /* Conversions to primitive types throw a std::domain_error if the object isn't an
         * appropriate numeric type or std::overflow_error in case of a too large int: */
        explicit operator int() const;
        explicit operator double() const;

      public:
        /* To be used internally: */
        using BasePtr = std::shared_ptr<const Base>;
        explicit Var(const BasePtr& ptr);
        explicit Var(BasePtr&& ptr);
        const BasePtr& get() const;

      private:
        BasePtr rep;
    };

    bool operator==(const Var& lhs, const Var& rhs);
    bool operator!=(const Var& lhs, const Var& rhs);

    Var operator+(Var lhs, const Var& rhs);
    Var operator-(Var lhs, const Var& rhs);
    Var operator*(Var lhs, const Var& rhs);
    Var operator/(Var lhs, const Var& rhs);

    std::ostream& operator<<(std::ostream& stream, const Var& rhs);
    std::ostream& operator<<(std::ostream& stream, const Var::Type& rhs);
}

namespace std {
    template <> struct hash<tsym::Var> {
        size_t operator()(const tsym::Var& var) const;
    };
}

#endif
