#ifndef TSYM_UNDEFINED_H
#define TSYM_UNDEFINED_H

#include "base.h"

namespace tsym {
    class Undefined : public Base {
        public:
            /* There is only one instance, because no state is needed: */
            static const BasePtr& create();

            explicit Undefined(Base::CtorKey&&);
            Undefined(const Undefined&) = delete;
            Undefined& operator = (const Undefined&) = delete;
            Undefined(Undefined&&) = delete;
            Undefined& operator = (Undefined&&) = delete;
            ~Undefined() = default;

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
    };
}

#endif
