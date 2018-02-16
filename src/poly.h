#ifndef TSYM_POLY_H
#define TSYM_POLY_H

#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym { class Gcd; }

namespace tsym {
    /* Functions for multivariate polynomial terms with rational number coefficients, symbolic
     * variables and positive integer exponents. All algorithms implemented here are described in
     * Cohen [2003]. */
    namespace poly {
        /* Division u/v, where the divisor v is non-zero. The first of the returned list is the
         * quotient, the second the remainder. If the input is invalid, the quotient is Undefined,
         * the remainder u. */
        BasePtrList divide(const BasePtr& u, const BasePtr& v);
        BasePtrList divide(const BasePtr& u, const BasePtr& v, const BasePtrList& L);
        /* Returns pseudo-quotient and -remainder similar to the function above: */
        BasePtrList pseudoDivide(const BasePtr& u, const BasePtr& v, const BasePtr& x);
        /* As before, but avoids the computation of the pseudo-quotient: */
        BasePtr pseudoRemainder(const BasePtr& u, const BasePtr& v, const BasePtr& x);
        int unit(const BasePtr& polynomial, const BasePtr& x);
        BasePtr gcd(const BasePtr& u, const BasePtr& v);
        BasePtr gcd(const BasePtr& u, const BasePtr& v, const Gcd& algo);
        BasePtr content(const BasePtr& polynomial, const BasePtr& x);
        BasePtr content(const BasePtr& polynomial, const BasePtr& x, const Gcd& algo);
        /* A variation of the degree of a polynomial; returns the minimal degree, e.g. minDegree(a^2
         * + a^3) = 2, while the degree will return 3. Used internally by the content function. */
        int minDegree(const BasePtr& of, const BasePtr& variable);
    }
}

#endif
