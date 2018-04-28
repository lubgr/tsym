#ifndef TSYM_CONSTANT_H
#define TSYM_CONSTANT_H

#include "base.h"
#include "name.h"

namespace tsym {
    class Constant : public Base {
        /* So far, only Pi and e are needed. This is, however, crucial for resolving trigonometric
         * functions exactly and returning exact results from the natural logarithm function. */
      public:
        static const BasePtr& createPi();
        static const BasePtr& createE();

      private:
        enum class Type { PI, E };

      public:
        Constant(Type type, Name&& name, Base::CtorKey&&);
        Constant(const Constant&) = delete;
        Constant& operator=(const Constant&) = delete;
        Constant(Constant&&) = delete;
        Constant& operator=(Constant&&) = delete;
        ~Constant() = default;

        bool isEqualDifferentBase(const BasePtr& other) const;
        bool sameType(const BasePtr& other) const;
        Number numericEval() const;
        Fraction normal(SymbolMap& map) const;
        BasePtr diffWrtSymbol(const BasePtr& symbol) const;
        std::string typeStr() const;
        bool isPositive() const;
        bool isNegative() const;
        unsigned complexity() const;
        size_t hash() const;

        bool isNumericallyEvaluable() const;
        bool isConstant() const;
        const Name& name() const;

      private:
        static BasePtr create(Type type, Name&& name);

        const Type type;
        const Name constantName;
    };
}

#endif
