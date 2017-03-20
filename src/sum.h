#ifndef TSYM_SUM_H
#define TSYM_SUM_H

#include <vector>
#include "base.h"
#include "baseptrlist.h"

namespace tsym {
    class Sum : public Base {
        public:
            static BasePtr create(const BasePtr& s1, const BasePtr& s2);
            static BasePtr create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3);
            static BasePtr create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3,
                    const BasePtr& s4);
            static BasePtr create(const BasePtrList& summands);

            /* Implentations of pure virtual methods of Base. */
            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;

            /* Overridden methods from Base. */
            bool isSum() const;
            BasePtr expand() const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            BasePtr coeff(const BasePtr& variable, int exp) const;
            int degree(const BasePtr& variable) const;

        private:
            explicit Sum(const BasePtrList& summands);
            Sum(const Sum& other);
            Sum& operator = (const Sum& other);
            ~Sum();

            static BasePtr createSimplifiedSum(const BasePtrList& summands);
            Fraction toCommonDenom(const std::vector<Fraction>& operands) const;
            int sign() const;
            int signOfNumericParts() const;
            int signOfSymbolicParts() const;
            BasePtr coeffOverSummands(const BasePtr& variable, int exp) const;
    };
}

#endif
