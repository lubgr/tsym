#ifndef TSYM_NUMPOWERSIMPL_H
#define TSYM_NUMPOWERSIMPL_H

#include "number.h"
#include "options.h"
#include "primefac.h"

namespace tsym {
    class NumPowerSimpl {
        /* Class for the simplification of a numeric power or a product of a numeric power and a
         * numeric. Purpose is the definition of the simplest possible representation (mainly done
         * by minimizing the number of primes in the power expression) up to a certain upper bound
         * (that can be set for all instances of this class via a method, the default value is 1000)
         * of numbers due to high computational costs of prime factorization. Examples are:
         *
         * - simple resolvable power, sqrt(4) = 2
         * - extraction of base sign, 2*(-5)^(1/3) = (-2)*5^(1/3)
         * - prime factorization and extraction, sqrt(8) = 2*sqrt(2)
         * - the same for an expression with prefactor, 2/sqrt(2) = sqrt(2)
         * - prefer integer with negative exponent, sqrt(1/2) = 2^(-1/2)
         * - prefer fraction over exponent -1, 3^(-1) = 1/3
         * - invert fraction to make exponent positive, (2/3)^(-1/5) = (3/2)^(1/5)
         * - extract prefactor for exponents > 1, 5^(5/2) = 25*sqrt(5)
         * - final evaluation of double values, 5*sqrt(1.234567) = 5.555553527777408
         *
         * This class shall be independent of the Power and Product classes, because the
         * functionality of this class is necessary in both simplification of products and powers.
         * The components can be obtained via getter methods, where the representation of a simple
         * resulting number n (in case the power could be resolved to a number, e.g. 4^(1/2) = 2) is
         * 1*n^1. */
      public:
        NumPowerSimpl() = default;

        void setPower(const Number& base, const Number& exp);
        /* The prefactor is one when not specified: */
        void setPreFac(const Number& fac);
        /* Modify the default limit up to which prime factorization is performed for simplification: */
        void setMaxPrimeResolution(Int limit);

        bool isInputValid();
        const Number& getNewBase();
        const Number& getNewExp();
        const Number& getPreFactor();

      private:
        const Number& get(const Number& component);
        void computeAndSetFlag();
        void compute();
        void initFromOrig();
        void computeNonRational();
        void computeRational();
        void computeNegOrPosExp();
        void computeNegExp();
        void computePosExp();
        void shiftNegBase();
        void computePosExpPosBase();
        void shiftNegPreFac();
        void computeAllPos();
        bool areValuesSmallEnough() const;
        void cancel();
        void defNewBasePrimes();
        void defPreFacPrimesInPower();
        void cancelAndExtract();
        void collectPrimesInPower();
        void primesToComponents();
        void adjustExpGreaterThanOne();
        void adjustExpSignAndBase();
        void shiftPreFacSignBack();

        Number origBase{0};
        Number origExp{0};
        Number origPreFac{1};
        Number newBase{0};
        Number newExp{0};
        Number preFac{0};
        bool needsComputation = true;
        bool isPreFacNegative = false;
        PrimeFac nbPrimes;
        PrimeFac pfPrimes;
        Int maxPrimeLimit = options::getMaxPrimeResolution();
    };
}

#endif
