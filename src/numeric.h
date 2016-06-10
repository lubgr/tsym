#ifndef TSYM_NUMERIC_H
#define TSYM_NUMERIC_H

#include "base.h"
#include "undefined.h"

namespace tsym {
    class Numeric : public Base {
        public:
            static BasePtr create(int value);
            static BasePtr create(double value);
            static BasePtr create(const Int& value);
            static BasePtr create(int numerator, int denominator);
            static BasePtr create(const Int& numerator, const Int& denominator);
            static BasePtr create(const Number& number);

            /* Shortcuts for frequently used constant numbers. */
            static const BasePtr& zero();
            static const BasePtr& one();
            static const BasePtr& mOne();

            /* Implentations of pure virtual methods of Base. */
            bool isEqual(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;

            /* Overridden methods from Base. */
            bool isNumericallyEvaluable() const;
            bool isNumeric() const;
            bool isZero() const;
            bool isOne() const;
            bool isConst() const;
            BasePtr numericTerm() const;
            BasePtr nonNumericTerm() const;
            BasePtr constTerm() const;
            BasePtr nonConstTerm() const;
            BasePtr coeff(const BasePtr& variable, int exp) const;
            int degree(const BasePtr& variable) const;

        private:
            Numeric(const Number& number);
            Numeric(const Numeric& other);
            Numeric& operator = (Numeric const& other);
            ~Numeric();

            const Number number;
    };
}

#endif
