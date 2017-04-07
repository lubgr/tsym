#ifndef TSYM_LOGARITHM_H
#define TSYM_LOGARITHM_H

#include "function.h"

namespace tsym {
    class Logarithm : public Function {
        /* Natural Logarithm with respect to basis e. */
        public:
            static BasePtr create(const BasePtr& arg);

            /* Implentations of pure virtual methods of Base. */
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            bool isPositive() const;
            bool isNegative() const;
            unsigned int complexity() const;


        private:
            Logarithm(const BasePtr& arg);
            Logarithm(const Logarithm& other);
            Logarithm& operator = (const Logarithm& other);
            ~Logarithm();

            static bool isInvalidArg(const BasePtr& arg);
            static BasePtr createNumerically(const BasePtr& arg);
            static BasePtr createFromConstant(const BasePtr& arg);
            static BasePtr createFromPower(const BasePtr& arg);

            Number checkedNumericEval() const;
            bool checkSign(bool (Base::*method)() const) const;

            const BasePtr& arg;
    };
}

#endif
