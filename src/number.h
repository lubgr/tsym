#ifndef TSYM_NUMBER_H
#define TSYM_NUMBER_H

#include "int.h"

namespace tsym {
    class Number {
        /* Independent wrapper class around integers, fractions and floating point numbers. This
         * class is needed inside of the classes derived from the Base class, more specifically,
         * it's needed independently of the base class. Simplification of fractions is always
         * performed automatically. Floating point numbers are automatically converted to fractions
         * up to a certain (not very high) accuracy. */
        public:
            Number() = default;
            Number(int value);
            Number(double value);
            Number(int numerator, int denominator);
            explicit Number(Int value);
            Number(Int numerator, Int denominator);
            static Number createUndefined();

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
            bool isUndefined() const;

            /* Returns the numerator of a fraction or the value of an integer. */
            const Int& numerator() const;
            /* Returns the denominator in case of a fraction, one otherwise. */
            const Int& denominator() const;
            double toDouble() const;
            Number abs() const;
            /* Returns 0 in case of a zero number, otherwise -1 or 1: */
            int sign() const;

        private:
            void setAndSimplify(Int&& num, Int&& denom, double dValue);
            void set(Int&& num, Int&& denom, double dValue);
            void setUndefined();
            void simplify();
            void tryDoubleToFraction();
            void cancel();
            bool isThisOrOtherUndefined(const Number& other) const;
            bool isThisOrOtherDouble(const Number& other) const;
            void addRational(const Number& other);
            Number flipSign() const;
            void timesRational(const Number& other);
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

            Int num { 0 };
            Int denom { 1 };
            double dValue { 0.0 };
            bool undefined = false;
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
