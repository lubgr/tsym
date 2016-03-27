#ifndef TSYM_FUNCTION_H
#define TSYM_FUNCTION_H

#include "base.h"

namespace tsym {
    class Function : public Base {
        /* Underlying abstract base class for functions to be further specified by inheriting
         * classes. For those, a constructor is provided, otherwise, there are getter to access the
         * name and the argument of the function. */
        public:
            /* These pure virtual methods of Base must be implemented by concrete functions. */
            virtual Number numericEval() const = 0;
            virtual Fraction normal(SymbolMap& map) const = 0;
            virtual BasePtr diffWrtSymbol(const BasePtr& symbol) const = 0;
            virtual BasePtr subst(const BasePtr& from, const BasePtr& to) const = 0;

            /* Implentations of pure virtual methods of Base. */
            bool isEqual(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            std::string typeStr() const;

            /* Overridden methods from Base. */
            bool isConst() const;
            bool isFunction() const;
            BasePtr constTerm() const;
            BasePtr nonConstTerm() const;
            const Name& name() const;

        protected:
            Function(const BasePtrList& args, const std::string& name);
            Function(const Function& other);
            Function& operator = (const Function& other);
            virtual ~Function();

        private:
            const Name functionName;
    };
}

#endif
