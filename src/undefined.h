#ifndef TSYM_UNDEFINED_H
#define TSYM_UNDEFINED_H

#include "base.h"

namespace tsym {
    class Undefined : public Base {
        public:
            static BasePtr create();

            /* Returns always false: */
            bool isEqual(const BasePtr& other) const;
            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap&) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;
            unsigned complexity() const;
            size_t hash() const;

            /* Returns always true: */
            bool isDifferent(const BasePtr& other) const;
            bool has(const BasePtr& other) const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            int degree(const BasePtr& variable) const;

            bool isUndefined() const;

        private:
            Undefined();
            Undefined(const Undefined& other) = delete;
            Undefined& operator = (Undefined const& other) = delete;
            ~Undefined();
    };
}

#endif
