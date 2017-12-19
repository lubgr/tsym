#ifndef TSYM_PRIMEFAC_H
#define TSYM_PRIMEFAC_H

#include <vector>
#include "number.h"

namespace tsym {
    class PrimeFac {
        /* Utilty class for prime factorization of a rational, positive number. The prime numbers
         * (numerator and denominator) are stored as vectors, where empty vectors means, that the
         * number is one. Multiplication and exponentiation with integer exponents are possible.
         * Extraction of prime numbers for a given rational exponent is additionaly provided. In
         * that case, the exponent may be changed, too.*/
        public:
            PrimeFac() = default;
            explicit PrimeFac(const Number& n);

            void set(const Number& n);

            /* Integer exponents only (restricition to rational numbers). */
            void toThe(const Int& exponent);
            void multiply(const Number& n);
            void multiply(const PrimeFac& other);
            /* Splits the number into a resolvable part for the given exponent, which is returned,
             * and the remaining part under the power, which is the altered object itself. A simple
             * example is (2*2*2*2)^(1/3) = 2*2^(1/3). */
            PrimeFac extract(const Number& exponent);
            /* Counts the occurrences of prime numbers, if they are all identical, duplicates are
             * dropped and the new exponent is returned. Example: (5*5)^(1/3) = 5^(2/3). */
            Number collectToNewExp(const Number& exponent);
            /* Returns the equal number of occurences of all primes or zero, if the occurence counts
             * differ (method is public mainly for unit testing). */
            int getEqualCount() const;

            const std::vector<Int>& getNumPrimes() const;
            const std::vector<Int>& getDenomPrimes() const;
            Number eval() const;

        private:
            void checkAndFactorize(const Number& n);
            void factorize(const Number& n);
            void defPrimes(Int n, std::vector<Int>& primes);
            void copyElementsNTimes(const Int& n, std::vector<Int>& primes);
            void cancelPrimes(std::vector<Int>& p1, std::vector<Int>& p2);
            void merge(std::vector<Int>& target, const std::vector<Int>& source);
            void extract(std::vector<Int>& source, std::vector<Int>& target, const Int& expDenom);
            int getEqualCount(const std::vector<Int>& primes) const;
            void deleteElements(int nToDelete);
            void deleteElements(int nToDelete, std::vector<Int>& primes);

            std::vector<Int> numPrimes;
            std::vector<Int> denomPrimes;
    };
}

#endif
