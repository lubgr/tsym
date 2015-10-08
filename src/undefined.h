#ifndef TSYM_UNDEFINED_H
#define TSYM_UNDEFINED_H

#include "base.h"
#include "baseptr.h"

namespace tsym {
    class Undefined : public Base {
        public:
            static BasePtr create();

            /* Returns always false: */
            bool isEqual(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap&) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;

            /* Returns always false: */
            bool isDifferent(const BasePtr& other) const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;

            bool isUndefined() const;

        private:
            Undefined();
            Undefined(const Undefined& other);
            Undefined& operator = (Undefined const& other);
            ~Undefined();
    };
}

#endif
