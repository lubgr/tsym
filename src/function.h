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
            virtual Number numericEval() const = 0;
            virtual Fraction normal(SymbolMap& map) const = 0;
            virtual BasePtr diffWrtSymbol(const BasePtr& symbol) const = 0;
            virtual BasePtr subst(const BasePtr& from, const BasePtr& to) const = 0;
            virtual bool isPositive() const = 0;
            virtual bool isNegative() const = 0;
            virtual unsigned complexity() const = 0;

            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            std::string typeStr() const;
            size_t hash() const;

            bool isConst() const;
            bool isFunction() const;
            BasePtr constTerm() const;
            BasePtr nonConstTerm() const;
            const Name& name() const;

        protected:
            Function(const BasePtrCtr& args, const std::string& name);
            Function(const Function& other) = delete;
            Function& operator = (const Function& other) = delete;
            virtual ~Function() = default;

        private:
            const Name functionName;
    };
}

#endif
