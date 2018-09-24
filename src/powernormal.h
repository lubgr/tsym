#ifndef TSYM_POWERNORMAL_H
#define TSYM_POWERNORMAL_H

#include "fraction.h"
#include "numeric.h"

namespace tsym {
    class SymbolMap;
}

namespace tsym {
    class PowerNormal {
        /* Utility class for the normalization of Power objects. Input are base and exponent as well
         * as a non-const SymbolMap, output is a Fraction containing the rational form of the power
         * (in most cases, this includes a replacement of the Power by temporary symbols, handled in
         * the SymbolMap). */
      public:
        /* Base and exponent parameter aren't required to outlive the class instance: */
        PowerNormal(const Base& base, const Base& exp, SymbolMap& map);
        /* The data members are const-qualified, hence the explicit assignment deletion: */
        const PowerNormal& operator=(const PowerNormal&) = delete;
        const PowerNormal& operator=(PowerNormal&&) = delete;
        PowerNormal(const PowerNormal&) = default;
        PowerNormal(PowerNormal&&) = default;
        ~PowerNormal() = default;

        Fraction normal() const;

      private:
        bool isBaseOrExpUndefined() const;
        bool isRationalExpInteger() const;
        Fraction normalIntegerExp() const;
        Fraction normalNonIntegerExp() const;
        Fraction normalNumEvalExp() const;
        Fraction normalNumEvalPosExp() const;
        BasePtr evaluatePow(const BasePtr& base, const BasePtr& exp) const;
        Fraction normalNumEvalNegExp() const;
        Fraction normalNonNumEvalExp() const;

        const BasePtr& one{Numeric::one()};
        SymbolMap& map;
        const Fraction rationalBase;
        const BasePtr rationalExp;
    };
}

#endif
