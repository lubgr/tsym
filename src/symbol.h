#ifndef TSYM_SYMBOL_H
#define TSYM_SYMBOL_H

#include <string>
#include "base.h"
#include "name.h"

namespace tsym {
    class Symbol : public Base {
        /* Symbols are compared by their name and a their positive flag. */
        public:
            static BasePtr create(const std::string& name);
            static BasePtr create(const Name& name);
            static BasePtr createPositive(const std::string& name);
            static BasePtr createPositive(const Name& name);
            static BasePtr createTmpSymbol(bool positive = false);

            Symbol(const Name& name, bool positive, Base::CtorKey&&);
            Symbol(unsigned tmpId, bool positive, Base::CtorKey&&);
            Symbol(const Symbol&) = delete;
            Symbol& operator = (const Symbol&) = delete;
            Symbol(Symbol&&) = delete;
            Symbol& operator = (Symbol&&) = delete;
            ~Symbol();

            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap&) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;
            unsigned complexity() const;
            size_t hash() const;

            bool isSymbol() const;
            const Name& name() const;

        private:
            static BasePtr create(const Name& name, bool positive);
            static BasePtr createNonEmptyName(const Name& name, bool positive);
            bool isEqualOtherSymbol(const BasePtr& other) const;

            const Name symbolName;
            const bool positive;
            static unsigned tmpCounter;
            static constexpr const char *tmpSymbolNamePrefix = "tmp#";
    };
}

#endif
