#ifndef TSYM_PRODUCT_H
#define TSYM_PRODUCT_H

#include "base.h"

namespace tsym {
    class Product : public Base {
        public:
            static BasePtr create(const BasePtr& f1, const BasePtr& f2);
            static BasePtr create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3);
            static BasePtr create(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3,
                    const BasePtr& f4);
            static BasePtr minus(const BasePtr& f1);
            static BasePtr minus(const BasePtr& f1, const BasePtr& f2);
            static BasePtr minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3);
            static BasePtr minus(const BasePtr& f1, const BasePtr& f2, const BasePtr& f3,
                    const BasePtr& f4);
            static BasePtr create(const BasePtrCtr& factors);

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

            bool isProduct() const;
            BasePtr numericTerm() const;
            BasePtr nonNumericTerm() const;
            BasePtr constTerm() const;
            BasePtr nonConstTerm() const;
            BasePtr expand() const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            BasePtr coeff(const BasePtr& variable, int exp) const;
            int degree(const BasePtr& variable) const;

        private:
            explicit Product(const BasePtrCtr& factors);
            Product(const Product& other) = delete;
            Product& operator = (Product const& other) = delete;
            ~Product() = default;

            static BasePtr createSimplifiedProduct(const BasePtrCtr& factors);
            static bool needsExpansion(const BasePtrCtr& factors);
            int sign() const;
            Fraction normalAndSplitIntoFraction(SymbolMap& map) const;
            BasePtr coeffFactorMatch(const BasePtr& variable, int exp) const;
    };
}

#endif
