#ifndef TSYM_LOGARITHM_H
#define TSYM_LOGARITHM_H

#include "function.h"

namespace tsym {
    class Logarithm : public Function {
        /* Natural Logarithm with respect to basis e. */
        public:
            static BasePtr create(const BasePtr& arg);

            Logarithm(const BasePtr& arg, Base::CtorKey&&);
            Logarithm(const Logarithm&) = delete;
            Logarithm& operator = (const Logarithm&) = delete;
            Logarithm(Logarithm&&) = delete;
            Logarithm& operator = (Logarithm&&) = delete;
            ~Logarithm() = default;

            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            bool isPositive() const;
            bool isNegative() const;
            unsigned complexity() const;

        private:
            static bool isInvalidArg(const BasePtr& arg);
            static BasePtr createInstance(const BasePtr& arg);
            static BasePtr createNumerically(const BasePtr& arg);
            static BasePtr createFromConstant(const BasePtr& arg);
            static BasePtr createFromPower(const BasePtr& arg);

            Number checkedNumericEval() const;
            bool checkSign(bool (Base::*method)() const) const;

            const BasePtr& arg;
    };
}

#endif
