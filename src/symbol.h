#ifndef TSYM_SYMBOL_H
#define TSYM_SYMBOL_H

#include <string>
#include "base.h"
#include "baseptr.h"

namespace tsym {
    class Symbol : public Base {
        public:
            static BasePtr create(const std::string& name);
            static BasePtr create(const Name& name);
            static BasePtr createTmpSymbol();

            /* Implentations of pure virtual methods of Base. */
            bool isEqual(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap&) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;

            /* Overridden methods from Base. */
            bool isSymbol() const;
            const Name& name() const;

        private:
            explicit Symbol(const Name& name);
            Symbol(const Symbol& other);
            Symbol& operator = (const Symbol& other);
            ~Symbol();

            static Name getTmpName();

            const Name symbolName;
    };
}

#endif
