#ifndef TSYM_PRIMEFAC_H
#define TSYM_PRIMEFAC_H

#include <vector>
#include "int.h"
#include "number.h"
#include "numberfct.h"
#include "simpleprimepolicy.h"

namespace tsym {
    class Number;
}

namespace tsym {
    class PrimeFac {
        /* Utilty class for prime factorization of a rational, positive number. Upon construction,
         * the policy for computing prime numbers can be injected as a class template for integer
         * types. The prime numbers (numerator and denominator) are stored as vectors, where empty
         * vectors means, that the number is one. Multiplication and exponentiation with integer
         * exponents are possible. Extraction of prime numbers for a given rational exponent is
         * additionaly provided. In that case, the exponent may be changed, too.*/
      public:
        PrimeFac() = default;
        template <template <class> class PrimeFacPolicy = SimplePrimePolicy> explicit PrimeFac(const Number& n)
        {
            if (isDouble(n) || n < 0)
                return;
            else
                factorize<PrimeFacPolicy<Int>>(n);
        }

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
        template <class PrimeFacPolicy> void factorize(const Number& n)
        {
            PrimeFacPolicy::computeAndStore(n.numerator(), numPrimes);
            PrimeFacPolicy::computeAndStore(n.denominator(), denomPrimes);
        }

        void cancelPrimes(std::vector<Int>& p1, std::vector<Int>& p2);
        void merge(std::vector<Int>& target, std::vector<Int>&& source);
        void eraseDuplicates();

        std::vector<Int> numPrimes;
        std::vector<Int> denomPrimes;
    };
}

#endif
