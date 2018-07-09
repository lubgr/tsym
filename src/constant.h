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
        ~Constant() override = default;

        bool isEqualDifferentBase(const Base& other) const override;
        bool sameType(const Base& other) const override;
        Number numericEval() const override;
        Fraction normal(SymbolMap& map) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        std::string typeStr() const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        bool isNumericallyEvaluable() const override;
        bool isConstant() const override;
        const Name& name() const override;

      private:
        static BasePtr create(Type type, Name&& name);

        const Type type;
        const Name constantName;
    };
}

#endif
