#ifndef TSYM_FRACTION_H
#define TSYM_FRACTION_H

#include "baseptr.h"

namespace tsym {
    class Fraction {
        /* Trivial class that simplifies the handling of normalization and related operations. Its
         * main functionality is being the return value of Base-children's normal methods (thus, it
         * has nothing to do with numerical fractions per se). For that purpose, a constructor with
         * numerator only is provided - the denominator is set to 1. */
        public:
            Fraction();
            explicit Fraction(const BasePtr& numerator);
            Fraction(const BasePtr& numerator, const BasePtr& denominator);

            BasePtr eval() const; /* Divides numerator by denominator. */
            Fraction invert() const;
            Fraction cancel() const;

            const BasePtr& num() const;
            const BasePtr& denom() const;

        private:
            Fraction cancelNonTrivial() const;

            BasePtr numerator;
            BasePtr denominator;
    };
}

#endif
