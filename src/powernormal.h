#ifndef TSYM_POWERNORMAL_H
#define TSYM_POWERNORMAL_H

#include "symbolmap.h"
#include "fraction.h"
#include "numeric.h"

namespace tsym {
    class PowerNormal {
        /* Utility class for the normalization of Power objects. Input are base and exponent as well
         * as a non-const SymbolMap, output is a Fraction containing the rational form of the power
         * (in most cases, this includes a replacement of the Power by temporary symbols, handled in
         * the SymbolMap). */
        public:
            explicit PowerNormal(SymbolMap& map);

            void setBase(const BasePtr& base);
            void setExponent(const BasePtr& exp);

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

            const BasePtr& one { Numeric::one() };
            Fraction rationalBase {};
            BasePtr rationalExp {};
            SymbolMap& map;
    };
}

#endif
