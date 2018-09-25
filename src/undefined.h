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
        bool isEqual(const Base& other) const override;
        bool isEqualDifferentBase(const Base& other) const override;
        bool sameType(const Base& other) const override;
        std::optional<Number> numericEval() const override;
        Fraction normal(SymbolMap&) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        std::string_view typeStr() const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        /* Returns always true: */
        bool isDifferent(const Base& other) const override;
        bool has(const Base& other) const override;
        BasePtr subst(const Base& from, const BasePtr& to) const override;
        int degree(const Base& variable) const override;

        bool isUndefined() const override;
    };
}

#endif
