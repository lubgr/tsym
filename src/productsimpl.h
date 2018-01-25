#ifndef TSYM_PRODUCTSIMPL_H
#define TSYM_PRODUCTSIMPL_H

#include "int.h"
#include "baseptr.h"
#include "trigonometric.h"

namespace tsym {
    class Number;
    class BasePtrList;
}

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
            BasePtrList simplify(const BasePtrList& factors);

        private:
            void prepare(BasePtrList& fac);
            void extractProducts(BasePtrList& u);
            void contractTrigonometrics(BasePtrList& u);
            void contract(BasePtrList& u,
                    bool (ProductSimpl::*check)(const BasePtr& f1, const BasePtr& f2),
                    BasePtrList (ProductSimpl::*simpl)(const BasePtr& f1, const BasePtr& f2));
            bool areContractableTrigFctPowers(const BasePtr& f1, const BasePtr& f2);
            bool isContractableTrigFctPower(const BasePtr& pow);
            BasePtrList contractTrigFctPowers(const BasePtr& f1, const BasePtr& f2);
            BasePtr trigSymbReplacement(Trigonometric::Type type, const BasePtr& arg);
            BasePtr trigFunctionPowerReplacement(const BasePtr& pow, const BasePtr& sin,
                    const BasePtr& cos);

            BasePtrList simplTwoFactors(const BasePtrList& u);
            BasePtrList simplTwoFactors(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplTwoFactorsWithProduct(const BasePtr& f1, const BasePtr& f2);
            BasePtrList merge(const BasePtrList& l1, const BasePtrList& l2);
            BasePtrList mergeNonEmpty(const BasePtrList& p, const BasePtrList& q);
            BasePtrList simplTwoFactorsWithoutProduct(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplTwoConst(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplTwoNumerics(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplNumAndConst(const BasePtr& numeric, const BasePtr& constant);
            BasePtrList simplNumAndNumPow(const BasePtr& numeric, const BasePtr& numPow);
            BasePtrList simplNumAndNumPow(const Number& preFactor, const Number& base,
                    const Number& exp);
            bool haveEqualBases(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplTwoEqualBases(const BasePtr& f1, const BasePtr& f2);
            bool areNumPowersWithEqualExp(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplTwoEqualExp(const BasePtr& f1, const BasePtr& f2);
            bool areNumPowersWithZeroSumExp(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplTwoZeroSumExp(const BasePtr& f1, const BasePtr& f2);
            bool areNumPowersWithEqualExpDenom(const BasePtr& f1, const BasePtr& f2);
            BasePtrList simplTwoEqualExpDenom(const BasePtr& f1, const BasePtr& f2);
            Int evalNumExpNumerator(const BasePtr& numPow);
            Int evalExpNumerator(const Number& base, const Int& exp);
            Int evalDenomExpNumerator(const BasePtr& numPow);

            BasePtrList simplNFactors(BasePtrList u);
            void prepareConst(BasePtrList& u);
            void contractNumerics(BasePtrList& u);
            void contractConst(BasePtrList& u);
            bool areTwoContractableConst(const BasePtr& f1, const BasePtr& f2);
            void contractTwoConst(BasePtrList::iterator& it1, BasePtrList::iterator& it2,
                    BasePtrList& u);
            BasePtrList simplPreparedFactors(const BasePtrList& u);
            BasePtrList simplNPreparedFactors(const BasePtrList& u);
    };
}

#endif
