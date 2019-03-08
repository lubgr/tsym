#ifndef TSYM_PRODUCTSIMPL_H
#define TSYM_PRODUCTSIMPL_H

#include "baseptr.h"
#include "baseptrlist.h"
#include "int.h"
#include "trigonometric.h"

namespace tsym {
    /* Automatic simplification of a product of a number of factors according to Cohen, Computer
     * Algebra and Symbolic Computation [2003], chapter 3.2.
     *
     * It varies in the handling of constant powers, i.e. powers, where both the base as well as the
     * exponent are numerics. One implication is that 2*sqrt(2) won't be evaluated to 2^(3/2) but
     * rather stays being a product, see the NumPowerSimpl class for details, e.g. numeric powers
     * with equal exponents are transformed according to sqrt(3)*sqrt(6) = sqrt(18) = 3*sqrt(2).
     * Another feature is the contraction of numeric powers with an equal denominator of the
     * exponent, e.g. 2^(1/3)*3^(2/3) = 18^(1/3). To avoid unexpected high integers, this
     * contraction is carried out only if the new base (numerator or denominator) is less than the
     * limit for the prime factorization, see again the NumPowerSimpl class.
     *
     * An additional feature is the automatic simplification of appropiate products of trigonometric
     * functions: sin(a)/cos(a) = tan(a) and cos(a)/sin(a) = 1/tan(a). This has to be done in
     * advance to Cohen's algorithm to ensure its proper functionality. */
    BasePtrList simplifyProduct(const BasePtrList& factors);
}

#endif
