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
        Undefined& operator=(const Undefined&) = delete;
        Undefined(Undefined&&) = delete;
        Undefined& operator=(Undefined&&) = delete;
        ~Undefined() override = default;

        /* Returns always false: */
        bool isEqual(const BasePtr& other) const override;
        bool isEqualDifferentBase(const BasePtr& other) const override;
        bool sameType(const BasePtr& other) const override;
        Number numericEval() const override;
        Fraction normal(SymbolMap&) const override;
        BasePtr diffWrtSymbol(const BasePtr& symbol) const override;
        std::string typeStr() const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        /* Returns always true: */
        bool isDifferent(const BasePtr& other) const override;
        bool has(const BasePtr& other) const override;
        BasePtr subst(const BasePtr& from, const BasePtr& to) const override;
        int degree(const BasePtr& variable) const override;

        bool isUndefined() const override;
    };
}

#endif
