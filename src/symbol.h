#ifndef TSYM_SYMBOL_H
#define TSYM_SYMBOL_H

#include <string>
#include <string_view>
#include "base.h"
#include "name.h"

namespace tsym {
    class Symbol : public Base {
        /* Symbols are compared by their name and a their positive flag. */
      public:
        static BasePtr create(std::string_view name);
        static BasePtr create(const Name& name);
        static BasePtr createPositive(std::string_view name);
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
        std::optional<Number> numericEval() const override;
        Fraction normal(SymbolMap&) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        const Name& name() const override;

      private:
        static BasePtr create(const Name& name, bool positive);
        static BasePtr createNonEmptyName(const Name& name, bool positive);
        bool isEqualOtherSymbol(const Base& other) const;

        const Name symbolName;
        const bool positive;
        static unsigned tmpCounter;
        static constexpr std::string_view tmpSymbolNamePrefix = "tmp#";
    };
}

#endif
