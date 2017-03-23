#ifndef TSYM_SYMBOL_H
#define TSYM_SYMBOL_H

#include <string>
#include "base.h"

namespace tsym {
    class Symbol : public Base {
        /* Symbols are compared by their name and a their positive flag. */
        public:
            static BasePtr create(const std::string& name);
            static BasePtr create(const Name& name);
            static BasePtr createPositive(const std::string& name);
            static BasePtr createPositive(const Name& name);
            /* Sets 'count' to the temporary symbol counter if not a nullptr: */
            static BasePtr createTmpSymbol(unsigned *count, bool positive = false);

            /* Implentations of pure virtual methods of Base. */
            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap&) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;
            size_t hash() const;

            /* Overridden methods from Base. */
            bool isSymbol() const;
            const Name& name() const;

        private:
            Symbol(const Name& name, bool positive);
            Symbol(unsigned tmpId, bool positive);
            Symbol(const Symbol& other);
            Symbol& operator = (const Symbol& other);
            ~Symbol();

            static BasePtr create(const Name& name, bool positive);
            static BasePtr createNonEmptyName(const Name& name, bool positive);
            bool isEqualOtherSymbol(const BasePtr& other) const;

            const Name symbolName;
            const bool positive;
            static unsigned tmpCounter;
    };
}

#endif
