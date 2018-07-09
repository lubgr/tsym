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
        Logarithm& operator=(const Logarithm&) = delete;
        Logarithm(Logarithm&&) = delete;
        Logarithm& operator=(Logarithm&&) = delete;
        ~Logarithm() override = default;

        Number numericEval() const override;
        Fraction normal(SymbolMap& map) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        BasePtr subst(const Base& from, const BasePtr& to) const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;

      private:
        static bool isInvalidArg(const Base& arg);
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
