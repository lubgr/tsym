#ifndef TSYM_NUMBER_H
#define TSYM_NUMBER_H

#include "int.h"
#include <boost/rational.hpp>

namespace tsym {
    class Number {
        /* Independent wrapper class around (Boost) rational numbers and floating point numbers.
         * This class is needed independently of the base class. Floating point numbers are
         * automatically converted to fractions up to a certain (not very high) accuracy. */
        public:
            Number() = default;
            Number(int value);
            Number(double value);
            Number(int numerator, int denominator);
            explicit Number(Int value);
            Number(Int numerator, Int denominator);

            Number& operator += (const Number& rhs);
            Number& operator -= (const Number& rhs);
            Number& operator *= (const Number& rhs);
            Number& operator /= (const Number& rhs);

            const Number& operator + () const;
            Number operator - () const;

            Number toThe(const Number& exponent) const;

            bool equal(const Number& other) const;
            bool lessThan(const Number& rhs) const;
            bool isZero() const;
            bool isOne() const;
            bool isInt() const;
            bool isFrac() const;
            bool isRational() const;
            bool isDouble() const;

            /* Returns the numerator of a fraction or the value of an integer. */
            Int numerator() const;
            /* Returns the denominator in case of a fraction, one otherwise. */
            Int denominator() const;
            double toDouble() const;
            Number abs() const;
            /* Returns 0 in case of a zero number, otherwise -1 or 1: */
            int sign() const;

        private:
            void setDebugString();
            void tryDoubleToFraction();
            bool isThisOrOtherDouble(const Number& other) const;
            Number flipSign() const;
            bool processTrivialPowers(const Number& exponent, Number& result) const;
            Number computeMinusOneToThe(const Number& exponent) const;
            bool processNegBase(const Number& exponent, Number& result) const;
            bool processIrrationalPowers(const Number& exponent, Number& result) const;
            void processRationalPowers(const Number& exponent, Number& result) const;
            void computeNumPower(const Int& numExponent, Number& result) const;
            void computeDenomPower(const Int& denomExponent, Number& result) const;
            Int tryGetBase(const Int& n, const Int& denomExponent) const;
            bool areBothRational(const Number& other) const;
            bool equalViaDouble(const Number& rhs) const;

            typedef boost::rational<Int> Rational;
            Rational rational{ 0 };
            double dValue{ 0.0 };
            static constexpr double ZERO_TOL = std::numeric_limits<double>::epsilon();
            static constexpr double TOL = 100.0*ZERO_TOL;

#ifdef TSYM_WITH_DEBUG_STRINGS
            /* A member to be accessed by a gdb pretty printing plugin. */
            std::string prettyStr;
#endif
    };

    bool operator == (const Number& lhs, const Number& rhs);
    bool operator < (const Number& lhs, const Number& rhs);
    bool operator != (const Number& lhs, const Number& rhs);
    bool operator <= (const Number& lhs, const Number& rhs);
    bool operator > (const Number& lhs, const Number& rhs);
    bool operator >= (const Number& lhs, const Number& rhs);

    Number operator + (Number lhs, const Number& rhs);
    Number operator - (Number lhs, const Number& rhs);
    Number operator * (Number lhs, const Number& rhs);
    Number operator / (Number lhs, const Number& rhs);

    std::ostream& operator << (std::ostream& stream, const Number& rhs);
}

namespace std
{
    template<> struct hash<tsym::Number>
    {
        size_t operator () (const tsym::Number& n) const;
    };
}

#endif
