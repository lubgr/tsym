#ifndef TSYM_INT_H
#define TSYM_INT_H

#include <iostream>
#include <cstddef>
#include "gmp.h"

namespace tsym {
    class Int {
        /* Simple wrapper around libgmp integer functions and memory management. Standard integer
         * operators are provided. The C++ interface of gmp (libgmpxx) could have been used instead,
         * but some of the API restrictions are circumvented here (probably sacrificing
         * performance), and operators as well as method names are more consistent with the Number
         * and Var classes. */
        public:
            Int();
            Int(int n);
            Int(long n);
            /* Truncates the argument: */
            explicit Int(double n);
            /* No error checking is performed, illegal strings result in an error log message, and
             * the number is set to zero: */
            explicit Int(const char *str);
            Int(const Int& other);
            const Int& operator = (const Int& rhs);
            ~Int();

            Int& operator += (const Int& rhs);
            Int& operator -= (const Int& rhs);
            Int& operator *= (const Int& rhs);
            /* Logs an error when rhs is zero, but forwads it to the gmp division: */
            Int& operator /= (const Int& rhs);
            /* The remainder has the same sign as the left and side: */
            Int& operator %= (const Int& rhs);

            Int& operator ++ ();
            Int& operator -- ();
            Int operator ++ (int);
            Int operator -- (int);
            const Int& operator + () const;
            Int operator - () const;

            /* Returns 0 if the exponent < 0: */
            Int toThe(const Int& exp) const;
            Int abs() const;

            bool equal(const Int& rhs) const;
            bool lessThan(const Int& rhs) const;
            int sign() const;
            bool fitsIntoInt() const;
            bool fitsIntoLong() const;
            int toInt() const;
            long toLong() const;
            double toDouble() const;

            void print(std::ostream& stream) const;

        private:
            Int nonTrivialPower(const Int& exp) const;

            mpz_t handle;
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
