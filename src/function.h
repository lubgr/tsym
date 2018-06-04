#ifndef TSYM_FUNCTION_H
#define TSYM_FUNCTION_H

#include "base.h"
#include "name.h"

namespace tsym {
    class Function : public Base {
        /* Underlying abstract base class for functions to be further specified by inheriting
         * classes. For those, a constructor is provided, otherwise, there are getter to access the
         * name and the argument of the function. */
      public:
        Number numericEval() const override = 0;
        Fraction normal(SymbolMap& map) const override = 0;
        BasePtr diffWrtSymbol(const BasePtr& symbol) const override = 0;
        BasePtr subst(const BasePtr& from, const BasePtr& to) const override = 0;
        bool isPositive() const override = 0;
        bool isNegative() const override = 0;
        unsigned complexity() const override = 0;

        bool isEqualDifferentBase(const BasePtr& other) const override;
        bool sameType(const BasePtr& other) const override;
        std::string typeStr() const override;
        size_t hash() const override;

        bool isConst() const override;
        bool isFunction() const override;
        BasePtr constTerm() const override;
        BasePtr nonConstTerm() const override;
        const Name& name() const override;

      protected:
        Function(const BasePtrList& args, const std::string& name);
        Function(const Function&) = delete;
        Function& operator=(const Function&) = delete;
        Function(Function&&) = delete;
        Function& operator=(Function&&) = delete;
        ~Function() override = default;

      private:
        const Name functionName;
    };
}

#endif
