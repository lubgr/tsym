#ifndef TSYM_PRODUCT_H
#define TSYM_PRODUCT_H

#include "base.h"

namespace tsym {
    class Product : public Base {
      public:
        static BasePtr create(const BasePtr& f1, const BasePtr& f2);
        static BasePtr create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3);
        static BasePtr create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3, const BasePtr& f4);
        static BasePtr minus(const BasePtr& f1);
        static BasePtr minus(const BasePtr& f1, const BasePtr& f2);
        static BasePtr minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3);
        static BasePtr minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3, const BasePtr& f4);
        static BasePtr create(const BasePtrList& factors);

        explicit Product(const BasePtrList& factors, Base::CtorKey&&);
        Product(const Product&) = delete;
        Product& operator=(const Product&) = delete;
        Product(Product&&) = delete;
        Product& operator=(Product&&) = delete;
        ~Product() override = default;

        bool isEqualDifferentBase(const BasePtr& other) const override;
        bool sameType(const BasePtr& other) const override;
        Number numericEval() const override;
        Fraction normal(SymbolMap& map) const override;
        BasePtr diffWrtSymbol(const BasePtr& symbol) const override;
        std::string typeStr() const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        bool isProduct() const override;
        BasePtr numericTerm() const override;
        BasePtr nonNumericTerm() const override;
        BasePtr constTerm() const override;
        BasePtr nonConstTerm() const override;
        BasePtr expand() const override;
        BasePtr subst(const BasePtr& from, const BasePtr& to) const override;
        BasePtr coeff(const BasePtr& variable, int exp) const override;
        int degree(const BasePtr& variable) const override;

      private:
        static BasePtr createSimplifiedProduct(const BasePtrList& factors);
        static bool needsExpansion(const BasePtrList& factors);
        int sign() const;
        Fraction normalAndSplitIntoFraction(SymbolMap& map) const;
        BasePtr coeffFactorMatch(const BasePtr& variable, int exp) const;
    };
}

#endif
