#ifndef TSYM_NUMERIC_H
#define TSYM_NUMERIC_H

#include "base.h"
#include "logging.h"
#include "number.h"
#include "undefined.h"

namespace tsym {
    class Numeric : public Base {
      public:
        static BasePtr create(Number number);
        template <class T> static BasePtr create(T&& value)
        {
            return create(Number(std::forward<T>(value)));
        }
        template <class S, class T> static BasePtr create(S&& num, T&& denom)
        {
            if (denom == 0) {
                TSYM_ERROR("Attempt to create a Numeric with zero denominator, result is Undefined");
                return Undefined::create();
            }

            return create(Number(std::forward<S>(num), std::forward<T>(denom)));
        }

        Numeric(Number&& number, Base::CtorKey&&);
        Numeric(const Numeric&) = delete;
        Numeric& operator=(const Numeric&) = delete;
        Numeric(Numeric&&) = delete;
        Numeric& operator=(Numeric&&) = delete;
        ~Numeric() override = default;

        /* Shortcuts for frequently used constant numbers. */
        static const BasePtr& zero();
        static const BasePtr& one();
        static const BasePtr& two();
        static const BasePtr& three();
        static const BasePtr& four();
        static const BasePtr& half();
        static const BasePtr& third();
        static const BasePtr& fourth();
        static const BasePtr& mOne();

        bool isEqualDifferentBase(const Base& other) const override;
        bool sameType(const Base& other) const override;
        std::optional<Number> numericEval() const override;
        Fraction normal(SymbolMap& map) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        bool isConst() const override;
        BasePtr numericTerm() const override;
        BasePtr nonNumericTerm() const override;
        BasePtr constTerm() const override;
        BasePtr nonConstTerm() const override;
        BasePtr coeff(const Base& variable, int exp) const override;
        int degree(const Base& variable) const override;

      private:
        const Number number;
    };
}

#endif
