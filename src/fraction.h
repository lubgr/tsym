#ifndef TSYM_FRACTION_H
#define TSYM_FRACTION_H

#include "baseptr.h"
#include "numeric.h"

namespace tsym {
    struct Fraction {
        /* Trivial entity that simplifies the handling of normalization and related operations. Its
         * main functionality is being the return value of Base-children's normal methods (thus, it
         * has nothing to do with numerical fractions per se). */
        BasePtr num;
        BasePtr denom = Numeric::one();
    };

    BasePtr eval(const Fraction& f); /* Divides numerator by denominator. */
    Fraction invert(const Fraction& f);
    Fraction cancel(const Fraction& f);
}

#endif
