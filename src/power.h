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

        Power(const BasePtr& base, const BasePtr& exponent, Base::CtorKey&&);
        Power(const Power&) = delete;
        Power& operator=(const Power&) = delete;
        Power(Power&&) = delete;
        Power& operator=(Power&&) = delete;
        ~Power() override = default;

        bool isEqualDifferentBase(const Base& other) const override;
        bool sameType(const Base& other) const override;
        std::optional<Number> numericEval() const override;
        Fraction normal(SymbolMap& map) const override;
        BasePtr diffWrtSymbol(const Base& symbol) const override;
        std::string typeStr() const override;
        bool isPositive() const override;
        bool isNegative() const override;
        unsigned complexity() const override;
        size_t hash() const override;

        bool isPower() const override;
        BasePtr expand() const override;
        BasePtr subst(const Base& from, const BasePtr& to) const override;
        BasePtr coeff(const Base& variable, int exp) const override;
        int degree(const Base& variable) const override;
        BasePtr base() const override;
        BasePtr exp() const override;

      private:
        static BasePtr createNotUndefined(const BasePtr& base, const BasePtr& exponent);
        static BasePtr createNonTrivial(const BasePtr& base, const BasePtr& exponent);
        bool isInteger(const Base& arg) const;
        BasePtr expandIntegerExponent() const;
        BasePtr expandSumBaseIntExp() const;

        const BasePtr& baseRef;
        const BasePtr& expRef;
    };
}

#endif
