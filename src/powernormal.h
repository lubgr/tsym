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
        explicit PowerNormal(SymbolMap& map);

        /* Base and exponent parameter aren't required to outlive the class instance: */
        void setBase(const Base& base);
        void setExponent(const Base& exp);

        Fraction normal();

      private:
        bool isBaseOrExpUndefined() const;
        bool isRationalExpInteger() const;
        Fraction normalIntegerExp() const;
        Fraction normalNonIntegerExp();
        Fraction normalNumEvalExp();
        Fraction normalNumEvalPosExp();
        BasePtr evaluatePow(const BasePtr& base, const BasePtr& exp);
        Fraction normalNumEvalNegExp();
        Fraction normalNonNumEvalExp();

        const BasePtr& one{Numeric::one()};
        BasePtr rationalExp{};
        SymbolMap& map;
        Fraction rationalBase;
    };
}

#endif
