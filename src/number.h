#ifndef TSYM_NUMBER_H
#define TSYM_NUMBER_H

#include <boost/operators.hpp>
#include <boost/rational.hpp>
#include <optional>
#include <variant>
#include "int.h"

namespace tsym {
    class Number : private boost::totally_ordered<Number, boost::arithmetic<Number>> {
        /* Independent wrapper class around (Boost) rational numbers and floating point numbers.
         * This class is needed independently of the base class. Floating point numbers are
         * automatically converted to fractions up to a certain (not very high) accuracy. */
      public:
        Number() = default;
        Number(int value); // NOLINT
        Number(double value); // NOLINT
        Number(int numerator, int denominator);
        explicit Number(const Int& value);
        Number(const Int& numerator, const Int& denominator);

        Number& operator+=(const Number& rhs);
        Number& operator-=(const Number& rhs);
        Number& operator*=(const Number& rhs);
        Number& operator/=(const Number& rhs);

        const Number& operator+() const;
        Number operator-() const;

        Number toThe(const Number& exponent) const;

        bool isRational() const;
        bool isDouble() const;
        /* Returns the numerator of a fraction or the value of an integer. Zero if the number is a
         * double: */
        Int numerator() const;
        /* Returns the denominator in case of a fraction, one otherwise. */
        Int denominator() const;
        double toDouble() const;

      private:
        using Rational = boost::rational<Int>;
        using Rep = std::variant<Rational, double>;

        template <class Operation> struct Operate {
            template <class T> Rep operator()(const T& lhs, const T& rhs)
            {
                return Operation{}(lhs, rhs);
            }

            Rep operator()(const Rational& lhs, double rhs)
            {
                return Operation{}(boost::rational_cast<double>(lhs), rhs);
            }

            Rep operator()(double lhs, const Rational& rhs)
            {
                return Operation{}(lhs, boost::rational_cast<double>(rhs));
            }
        };

        void setDebugString();
        void tryDoubleToFraction();
        double getDouble() const;

        std::optional<Number> computeTrivialPower(const Number& exponent) const;
        Number computeMinusOneToThe(const Number& exponent) const;
        std::optional<Number> computeNegBasePower(const Number& exponent) const;
        std::optional<Number> computeIrrationalPower(const Number& exponent) const;
        Number computeRationalPower(const Number& exponent) const;
        Number computeNumPower(const Int& numExponent) const;
        void computeDenomPower(const Int& denomExponent, Number& result) const;

        Rational getRational() const;

        Rep rep{Rational{0}};

#ifdef TSYM_WITH_DEBUG_STRINGS
        /* A member to be leveraged for pretty printing in a debugger. */
        std::string prettyStr{"0"};
#endif
    };

    bool operator==(const Number& lhs, const Number& rhs);
    bool operator<(const Number& lhs, const Number& rhs);

    std::ostream& operator<<(std::ostream& stream, const Number& rhs);
}

namespace std {
    template <> struct hash<tsym::Number> {
        size_t operator()(const tsym::Number& n) const;
    };
}

#endif
