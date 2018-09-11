
#include "subresultantgcd.h"
#include "basefct.h"
#include "bplist.h"
#include "logging.h"
#include "numeric.h"
#include "poly.h"
#include "power.h"
#include "product.h"

tsym::BasePtr tsym::SubresultantGcd::gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const
/* See Cohen [2003], pages 255 - 256. */
{
    const BasePtr& x(L.front());

    if (u->degree(*x) < v->degree(*x))
        return gcd(v, u, L);
    else
        return gcd(u, v, L);
}

tsym::BasePtr tsym::SubresultantGcd::gcd(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const
{
    const BasePtr& x(L.front());
    const BasePtrList R(bplist::rest(L));
    const BasePtr uContent(poly::content(u, x, *this));
    const BasePtr vContent(poly::content(v, x, *this));
    const BasePtr d(compute(uContent, vContent, R));
    BasePtr U(poly::divide(u, uContent, L).front());
    BasePtr V(poly::divide(v, vContent, L).front());
    const BasePtr g(compute(U->leadingCoeff(*x), V->leadingCoeff(*x), R));
    int delta = U->degree(*x) - V->degree(*x) + 1;
    BasePtr beta(Power::create(Numeric::mOne(), Numeric::create(delta)));
    BasePtr psi(Numeric::mOne());
    BasePtr tmp;
    int i = 0;

    while (true) {
        const BasePtr remainder = poly::pseudoRemainder(U, V, x);

        if (isZero(*remainder)) {
            U = V;
            break;
        } else if (remainder->isUndefined()) {
            TSYM_WARNING("Undefined remainder during subres. gcd computation, return 1.");
            return Numeric::one();
        }

        if (++i > 1) {
            const int deltaP = delta;
            delta = U->degree(*x) - V->degree(*x) + 1;

            tmp = Product::minus(U->leadingCoeff(*x));

            psi = poly::divide(Power::create(tmp, Numeric::create(deltaP - 1))->expand(),
              Power::create(psi, Numeric::create(deltaP - 2))->expand(), R)
                    .front();

            beta = Product::create(tmp, Power::create(psi, Numeric::create(delta - 1)))->expand();
        }

        U = V;
        V = poly::divide(remainder, beta, L).front();
    }

    tmp = poly::divide(U->leadingCoeff(*x), g, R).front();
    tmp = poly::divide(U, tmp, L).front();
    tmp = poly::divide(tmp, poly::content(tmp, x, *this), L).front();

    return Product::create(d, tmp)->expand();
}
