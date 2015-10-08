#ifndef TSYM_INT_H
#define TSYM_INT_H

#include <iostream>

namespace tsym {
    class Int {
        public:
            Int();
            Int(int n);
            Int(long n);
            ~Int();

            Int& operator += (const Int& rhs);
            Int& operator -= (const Int& rhs);
            Int& operator *= (const Int& rhs);
            Int& operator /= (const Int& rhs);
            Int& operator %= (const Int& rhs);

            Int& operator ++ ();
            Int& operator -- ();
            Int operator ++ (int);
            Int operator -- (int);
            const Int& operator + () const;
            Int operator - () const;

            /* Returns 0, if the exponent < 0. */
            Int toThe(const Int& exp) const;
            Int abs() const;

            bool equal(const Int& rhs) const;
            bool lessThan(const Int& rhs) const;
            int sign() const;
            bool hasOverflowed() const;
            bool fitsIntoInt() const;
            bool fitsIntoLong() const;
            int toInt() const;
            long toLong() const;
            double toDouble() const;

            void print(std::ostream& stream) const;

            static Int max();
            static Int min();

        private:
            bool warnOverflow(const char * operationName) const;
            bool warnOverflow(const char * operationName, const Int& operand) const;
            Int createOverflowed() const;
            Int nonTrivialPower(const Int& exp) const;

            static const int maxInt;
            static const int minInt;
            static const long maxLong;
            static const long minLong;
            bool overflow;
            long rep;
    };

    bool operator == (const Int& lhs, const Int& rhs);
    bool operator != (const Int& lhs, const Int& rhs);
    bool operator < (const Int& lhs, const Int& rhs);
    bool operator <= (const Int& lhs, const Int& rhs);
    bool operator > (const Int& lhs, const Int& rhs);
    bool operator >= (const Int& lhs, const Int& rhs);

    Int operator + (Int lhs, const Int& rhs);
    Int operator - (Int lhs, const Int& rhs);
    Int operator * (Int lhs, const Int& rhs);
    Int operator / (Int lhs, const Int& rhs);
    Int operator % (Int lhs, const Int& rhs);

    std::ostream& operator << (std::ostream& stream, const Int& rhs);
}

#endif
