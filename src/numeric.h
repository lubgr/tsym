#ifndef TSYM_NUMERIC_H
#define TSYM_NUMERIC_H

#include "base.h"
#include "number.h"

namespace tsym {
    class Numeric : public Base {
      public:
        static BasePtr create(int value);
        static BasePtr create(double value);
        static BasePtr create(const Int& value);
        static BasePtr create(int numerator, int denominator);
        static BasePtr create(const Int& numerator, const Int& denominator);
        static BasePtr create(const Number& number);

        explicit Numeric(const Number& number, Base::CtorKey&&);
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

        bool isNumericallyEvaluable() const override;
        bool isNumeric() const override;
        bool isZero() const override;
        bool isOne() const override;
        bool isConst() const override;
        BasePtr numericTerm() const override;
        BasePtr nonNumericTerm() const override;
        BasePtr constTerm() const override;
        BasePtr nonConstTerm() const override;
        BasePtr coeff(const BasePtr& variable, int exp) const override;
        int degree(const BasePtr& variable) const override;

      private:
        const Number number;
    };
}

#endif
