#ifndef TSYM_CONSTANT_H
#define TSYM_CONSTANT_H

#include "base.h"
#include "baseptr.h"
#include "number.h"

namespace tsym {
    class Constant : public Base {
        /* So far, only Pi and e are needed. This is, however, crucial for resolving trigonometric
         * functions exactly and returning exact results from the natural logarithm function. */
        public:
            static BasePtr createPi();
            static BasePtr createE();

            /* Implentations of pure virtual methods of Base. */
            bool isEqual(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;

            /* Overridden methods from Base. */
            bool isNumericallyEvaluable() const;
            bool isConstant() const;
            const Name& name() const;

        private:
            enum Type { PI, E };
            Constant(Type type, const Name& name);
            Constant(const Constant& other);
            Constant& operator = (const Constant& other);
            ~Constant();

            std::string getName(Type type) const;

            const Type type;
            const Name constantName;
    };
}

#endif
