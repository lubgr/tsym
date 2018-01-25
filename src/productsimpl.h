#ifndef TSYM_PRODUCTSIMPL_H
#define TSYM_PRODUCTSIMPL_H

#include "int.h"
#include "baseptr.h"
#include "baseptrctr.h"
#include "trigonometric.h"

namespace tsym { class Number; }

namespace tsym {
    /* Automatic simplification of a product of a number of factors according to Cohen, Computer
     * Algebra and Symbolic Computation [2003], chapter 3.2.
     *
     * It varies in the handling of constant powers, i.e. powers, where both the base as well as the
     * exponent are numerics. One implication is that 2*sqrt(2) won't be evaluated to 2^(3/2) but
     * rather stays being a product, see the NumPowerSimpl class for details, e.g. numeric powers
     * with equal exponents are transformed according to sqrt(3)*sqrt(6) = sqrt(18) = 3*sqrt(2).
     * Another feature is the contraction of numeric powers with an equal denominator of the
     * exponent, e.g. 2^(1/3)*3^(2/3) = 18^(1/3). To avoid unexpected high integers, this
     * contraction is carried out only if the new base (numerator or denominator) is less than the
     * limit for the prime factorization, see again the NumPowerSimpl class.
     *
     * An additional feature is the automatic simplification of appropiate products of trigonometric
     * functions: sin(a)/cos(a) = tan(a) and cos(a)/sin(a) = 1/tan(a). This has to be done in
     * advance to Cohen's algorithm to ensure its proper functionality. */
    class ProductSimpl {
        public:
            BasePtrCtr simplify(const BasePtrCtr& factors);

        private:
            void prepare(BasePtrCtr& fac);
            void extractProducts(BasePtrCtr& u);
            void contractTrigonometrics(BasePtrCtr& u);
            void contract(BasePtrCtr& u,
                    bool (ProductSimpl::*check)(const BasePtr& f1, const BasePtr& f2),
                    BasePtrCtr (ProductSimpl::*simpl)(const BasePtr& f1, const BasePtr& f2));
            bool areContractableTrigFctPowers(const BasePtr& f1, const BasePtr& f2);
            bool isContractableTrigFctPower(const BasePtr& pow);
            BasePtrCtr contractTrigFctPowers(const BasePtr& f1, const BasePtr& f2);
            BasePtr trigSymbReplacement(Trigonometric::Type type, const BasePtr& arg);
            BasePtr trigFunctionPowerReplacement(const BasePtr& pow, const BasePtr& sin,
                    const BasePtr& cos);

            BasePtrCtr simplTwoFactors(const BasePtrCtr& u);
            BasePtrCtr simplTwoFactors(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplTwoFactorsWithProduct(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr merge(const BasePtrCtr& l1, const BasePtrCtr& l2);
            BasePtrCtr mergeNonEmpty(const BasePtrCtr& p, const BasePtrCtr& q);
            BasePtrCtr simplTwoFactorsWithoutProduct(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplTwoConst(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplTwoNumerics(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplNumAndConst(const BasePtr& numeric, const BasePtr& constant);
            BasePtrCtr simplNumAndNumPow(const BasePtr& numeric, const BasePtr& numPow);
            BasePtrCtr simplNumAndNumPow(const Number& preFactor, const Number& base,
                    const Number& exp);
            bool haveEqualBases(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplTwoEqualBases(const BasePtr& f1, const BasePtr& f2);
            bool areNumPowersWithEqualExp(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplTwoEqualExp(const BasePtr& f1, const BasePtr& f2);
            bool areNumPowersWithZeroSumExp(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplTwoZeroSumExp(const BasePtr& f1, const BasePtr& f2);
            bool areNumPowersWithEqualExpDenom(const BasePtr& f1, const BasePtr& f2);
            BasePtrCtr simplTwoEqualExpDenom(const BasePtr& f1, const BasePtr& f2);
            Int evalNumExpNumerator(const BasePtr& numPow);
            Int evalExpNumerator(const Number& base, const Int& exp);
            Int evalDenomExpNumerator(const BasePtr& numPow);

            BasePtrCtr simplNFactors(BasePtrCtr u);
            void prepareConst(BasePtrCtr& u);
            void contractNumerics(BasePtrCtr& u);
            void contractConst(BasePtrCtr& u);
            bool areTwoContractableConst(const BasePtr& f1, const BasePtr& f2);
            void contractTwoConst(BasePtrCtr::iterator& it1, BasePtrCtr::iterator& it2,
                    BasePtrCtr& u);
            BasePtrCtr simplPreparedFactors(const BasePtrCtr& u);
            BasePtrCtr simplNPreparedFactors(const BasePtrCtr& u);
    };
}

#endif
