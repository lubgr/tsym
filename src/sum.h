#ifndef TSYM_SUM_H
#define TSYM_SUM_H

#include <vector>
#include "base.h"

namespace tsym {
    class Sum : public Base {
        public:
            static BasePtr create(const BasePtr& s1, const BasePtr& s2);
            static BasePtr create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3);
            static BasePtr create(const BasePtr& s1, const BasePtr& s2, const BasePtr& s3,
                    const BasePtr& s4);
            static BasePtr create(const BasePtrCtr& summands);

            explicit Sum(const BasePtrCtr& summands, Base::CtorKey&&);
            Sum(const Sum&) = delete;
            Sum& operator = (const Sum&) = delete;
            Sum(Sum&&) = delete;
            Sum& operator = (Sum&&) = delete;
            ~Sum() = default;

            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;
            unsigned complexity() const;
            size_t hash() const;

            bool isSum() const;
            BasePtr expand() const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            BasePtr coeff(const BasePtr& variable, int exp) const;
            int degree(const BasePtr& variable) const;

        private:
            static BasePtr createSimplifiedSum(const BasePtrCtr& summands);
            Fraction toCommonDenom(const std::vector<Fraction>& operands) const;
            int sign() const;
            int signOfNumericParts() const;
            int signOfSymbolicParts() const;
            BasePtr coeffOverSummands(const BasePtr& variable, int exp) const;
    };
}

#endif
