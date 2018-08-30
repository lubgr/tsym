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

        Symbol(Name name, bool positive, Base::CtorKey&&);
        Symbol(unsigned tmpId, bool positive, Base::CtorKey&&);
        Symbol(const Symbol&) = delete;
        Symbol& operator=(const Symbol&) = delete;
        Symbol(Symbol&&) = delete;
        Symbol& operator=(Symbol&&) = delete;
        ~Symbol() override;

        bool isEqualDifferentBase(const Base& other) const override;
        bool sameType(const Base& other) const override;
        std::optional<Number> numericEval() const override;
        Fraction normal(SymbolMap&) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        std::string typeStr() const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        bool isSymbol() const override;
        const Name& name() const override;

      private:
        static BasePtr create(const Name& name, bool positive);
        static BasePtr createNonEmptyName(const Name& name, bool positive);
        bool isEqualOtherSymbol(const Base& other) const;

        const Name symbolName;
        const bool positive;
        static unsigned tmpCounter;
        static constexpr const char* tmpSymbolNamePrefix = "tmp#";
    };
}

#endif
