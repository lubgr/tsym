#ifndef TSYM_NUMBER_H
#define TSYM_NUMBER_H

#include <boost/operators.hpp>
#include <boost/rational.hpp>
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

        /* Returns the numerator of a fraction or the value of an integer. */
        Int numerator() const;
        /* Returns the denominator in case of a fraction, one otherwise. */
        Int denominator() const;
        double toDouble() const;

      private:
        void setDebugString();
        void tryDoubleToFraction();
        bool isThisOrOtherDouble(const Number& other) const;
        bool processTrivialPowers(const Number& exponent, Number& result) const;
        Number computeMinusOneToThe(const Number& exponent) const;
        bool processNegBase(const Number& exponent, Number& result) const;
        bool processIrrationalPowers(const Number& exponent, Number& result) const;
        void processRationalPowers(const Number& exponent, Number& result) const;
        void computeNumPower(const Int& numExponent, Number& result) const;
        void computeDenomPower(const Int& denomExponent, Number& result) const;
        Int tryGetBase(const Int& n, const Int& denomExponent) const;

        using Rational = boost::rational<Int>;
        Rational rational{0};
        double dValue{0.0};
        static constexpr double ZERO_TOL = std::numeric_limits<double>::epsilon();
        static constexpr double TOL = 100.0 * ZERO_TOL;

#ifdef TSYM_WITH_DEBUG_STRINGS
        /* A member to be leveraged for pretty printing in a debugger. */
        std::string prettyStr;
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
