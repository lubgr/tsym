#ifndef TSYM_PRODUCT_H
#define TSYM_PRODUCT_H

#include "base.h"
#include "numeric.h"

namespace tsym {
    class Product : public Base {
      public:
        static BasePtr create(const BasePtrList& factors);
        template <class... T> static BasePtr create(T&&... args)
        {
            return create({std::forward<T>(args)...});
        }

        template <class... T> static BasePtr minus(T&&... args)
        {
            return create({Numeric::mOne(), std::forward<T>(args)...});
        }

        explicit Product(const BasePtrList& factors, Base::CtorKey&&);
        Product(const Product&) = delete;
        Product& operator=(const Product&) = delete;
        Product(Product&&) = delete;
        Product& operator=(Product&&) = delete;
        ~Product() override = default;

        bool isEqualDifferentBase(const Base& other) const override;
        std::optional<Number> numericEval() const override;
        Fraction normal(SymbolMap& map) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        BasePtr numericTerm() const override;
        BasePtr nonNumericTerm() const override;
        BasePtr constTerm() const override;
        BasePtr nonConstTerm() const override;
        BasePtr expand() const override;
        BasePtr subst(const Base& from, const BasePtr& to) const override;
        BasePtr coeff(const Base& variable, int exp) const override;
        int degree(const Base& variable) const override;

      private:
        static BasePtr createSimplifiedProduct(const BasePtrList& factors);
        static bool needsExpansion(const BasePtrList& factors);
        int sign() const;
        Fraction normalAndSplitIntoFraction(SymbolMap& map) const;
        BasePtr coeffFactorMatch(const Base& variable, int exp) const;
    };
}

#endif
