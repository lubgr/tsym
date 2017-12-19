#ifndef TSYM_POWER_H
#define TSYM_POWER_H

#include "base.h"

namespace tsym {
    class Power : public Base {
        /* Class representing a power via BasePtr members to base and exponent. As sum and product,
         * the power exists, once created, in a simplified form and can't be changed. For details of
         * the simplification, see the PowerSimpl class, which follows Cohen's Computer Algebra and
         * Symbolic Computation [2003] in some parts, but implements a special treatment of numeric
         * powers, e.g. (3/4)^(-1/2) = (4/3)^(1/2). */
        public:
            static BasePtr create(const BasePtr& base, const BasePtr& exponent);
            static BasePtr oneOver(const BasePtr& base);
            static BasePtr sqrt(const BasePtr& base);

            /* Implentations of pure virtual methods of Base. */
            bool isEqualDifferentBase(const BasePtr& other) const;
            bool sameType(const BasePtr& other) const;
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            std::string typeStr() const;
            bool isPositive() const;
            bool isNegative() const;
            unsigned complexity() const;
            size_t hash() const;

            /* Overridden methods from Base. */
            bool isPower() const;
            bool isNumericPower() const;
            BasePtr expand() const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            BasePtr coeff(const BasePtr& variable, int exp) const;
            int degree(const BasePtr& variable) const;
            BasePtr base() const;
            BasePtr exp() const;

        private:
            Power(const BasePtr& base, const BasePtr& exponent);
            Power(const Power& other) = delete;
            Power& operator = (Power const& other) = delete;
            ~Power() = default;

            static BasePtr createNotUndefined(const BasePtr& base, const BasePtr& exponent);
            static BasePtr createNonTrivial(const BasePtr& base, const BasePtr& exponent);
            bool isInteger(const BasePtr& ptr) const;
            bool isExponentRationalNumeric() const;
            BasePtr expandIntegerExponent() const;
            BasePtr expandSumBaseIntExp() const;

            const BasePtr& baseRef;
            const BasePtr& expRef;
    };
}

#endif
