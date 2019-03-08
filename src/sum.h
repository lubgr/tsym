#ifndef TSYM_SUM_H
#define TSYM_SUM_H

#include <vector>
#include "base.h"

namespace tsym {
    class Sum : public Base {
      public:
        static BasePtr create(const BasePtrList& summands);
        template <class... T> static BasePtr create(T&&... args)
        {
            return create({std::forward<T>(args)...});
        }

        explicit Sum(const BasePtrList& summands, Base::CtorKey&&);
        Sum(const Sum&) = delete;
        Sum& operator=(const Sum&) = delete;
        Sum(Sum&&) = delete;
        Sum& operator=(Sum&&) = delete;
        ~Sum() override = default;

        bool isEqualDifferentBase(const Base& other) const override;
        std::optional<Number> numericEval() const override;
        Fraction normal(SymbolMap& map) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        BasePtr expand() const override;
        BasePtr subst(const Base& from, const BasePtr& to) const override;
        BasePtr coeff(const Base& variable, int exp) const override;
        int degree(const Base& variable) const override;

      private:
        static BasePtr createSimplifiedSum(const BasePtrList& summands);
        Fraction toCommonDenom(const std::vector<Fraction>& operands) const;
        int sign() const;
        int signOfNumericParts() const;
        int signOfSymbolicParts() const;
        BasePtr coeffOverSummands(const Base& variable, int exp) const;
    };
}

#endif
