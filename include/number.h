#ifndef TSYM_NUMBER_H
#define TSYM_NUMBER_H

#include "int.h"
#include "version.h"

namespace tsym {
    class Number {
        /* Independent wrapper class around integers, fractions and floating point numbers. This
         * class is needed inside of the classes derived from the Base class, more specifically,
         * it's needed independently of the base class. Simplification of fractions is always
         * performed automatically. Floating point numbers are automatically converted to fractions
         * up to a certain (not very high) accuracy.
         *
         * Conveniece shortcut functions for numeric evaluation of powers can be found as static
         * methods of this class (Pow/Sqrt).
         *
         * Note that in contrast to many other computer algebra systems, no arbitrary precision
         * numbers are involved, which means that integer overflows may occur, e.g. when a power of
         * high numbers is requested. All operations for rational numbers are checked for an
         * overflow - if it happens, the representation of the number is automatically converted to
         * double and a warning is printed out. Double overflow isn't checked. */
        public:
            Number();
            Number(int value);
            Number(double value);
            Number(int numerator, int denominator);
            explicit Number(const Int& value);
            Number(const Int& numerator, const Int& denominator);
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
            /* Returns -1 or 1 (for 0, it return 1): */
            int sign() const;

            static Number Sqrt(const Number& base);
            static Number Pow(const Number& base, const Number& exp);

        private:
            void setAndSimplify(int num, int denom, double dValue);
            void setAndSimplify(const Int& num, const Int& denom, double dValue);
            void set(const Int& num, const Int& denom, double dValue);
            void setUndefined();
            void simplify();
            void tryDoubleToFraction();
            void cancel();
            Int gcd(const Int& a, const Int& b) const;
            bool isThisOrOtherUndefined(const Number& other) const;
            bool isThisOrOtherDouble(const Number& other) const;
            void addRational(const Number& other);
            Int lcm(const Int& a, const Int& b) const;
            void printOverflowWarning() const;
            Number flipSign() const;
            void timesRational(const Number& other);
            bool processTrivialPowers(const Number& exponent, Number& result) const;
            Number computeMinusOneToThe(const Number& exponent) const;
            bool processNegBase(const Number& exponent, Number& result) const;
            bool processIrrationalPowers(const Number& exponent, Number& result) const;
            bool processRationalPowers(const Number& exponent, Number& result) const;
            bool computeNumPower(const Int& numExponent, Number& result) const;
            void computeDenomPower(const Int& denomExponent, Number& result) const;
            Int tryGetBase(const Int& n, const Int& denomExponent) const;
            bool areBothRational(const Number& other) const;
            bool equalViaDouble(const Number& rhs) const;

            Int num;
            Int denom;
            double dValue;
            bool undefined;
            static const double TOL;
            static const double ZERO_TOL;
            #ifdef TSYM_DEBUG_STRINGS
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

#endif
