#ifndef TSYM_CONSTANT_H
#define TSYM_CONSTANT_H

#include "base.h"

namespace tsym {
    class Constant : public Base {
        /* So far, only Pi and e are needed. This is, however, crucial for resolving trigonometric
         * functions exactly and returning exact results from the natural logarithm function. */
        public:
            static BasePtr createPi();
            static BasePtr createE();

            /* Implentations of pure virtual methods of Base. */
            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;
            size_t hash() const;

            /* Overridden methods from Base. */
            bool isNumericallyEvaluable() const;
            bool isConstant() const;
            const Name& name() const;

        private:
            enum class Type { PI, E };
            Constant(Type type, const Name& name);
            Constant(const Constant& other);
            Constant& operator = (const Constant& other);
            ~Constant();

            const Type type;
            const Name constantName;
    };
}

#endif
