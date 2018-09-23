
#include "primitivegcd.h"
#include "basefct.h"
#include "bplist.h"
#include "logging.h"
#include "numeric.h"
#include "poly.h"
#include "product.h"

tsym::BasePtr tsym::PrimitiveGcd::gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const
{
    const BasePtr x(L.front());
    const BasePtrList R(rest(L));
    const BasePtr uContent(poly::content(u, x, *this));
    const BasePtr vContent(poly::content(v, x, *this));
    const BasePtr d(compute(uContent, vContent, R));
    BasePtr uPrimPart(poly::divide(u, uContent, L).front());
    BasePtr vPrimPart(poly::divide(v, vContent, L).front());

    while (!isZero(*vPrimPart)) {
        const auto remainder = poly::pseudoRemainder(uPrimPart, vPrimPart, x);
        BasePtr rPrimPart;

        if (isZero(*remainder))
            rPrimPart = Numeric::zero();
        else if (remainder->isUndefined()) {
            TSYM_WARNING("Undefined remainder during primitive gcd computation, return 1.");
            return Numeric::one();
        } else
            rPrimPart = poly::divide(remainder, poly::content(remainder, x), L).front();

        uPrimPart = vPrimPart;
        vPrimPart = rPrimPart;
    }

    return Product::create(d, uPrimPart)->expand();
}
