
#include "fraction.h"
#include <utility>
#include "basefct.h"
#include "logging.h"
#include "numeric.h"
#include "poly.h"
#include "power.h"
#include "product.h"
#include "undefined.h"

tsym::BasePtr tsym::eval(const Fraction& f)
{
    return Product::create(f.num, Power::oneOver(f.denom));
}

tsym::Fraction tsym::invert(const Fraction& f)
{
    return Fraction{f.denom, f.num};
}

namespace tsym {
    namespace {
        Fraction cancelNonTrivial(const Fraction& f)
        {
            const BasePtr gcd(poly::gcd(f.num, f.denom));
            const BasePtr newDenom(poly::divide(f.denom, gcd).front());
            const BasePtr newNum(poly::divide(f.num, gcd).front());

            return Fraction{newNum, newDenom};
        }
    }
}

tsym::Fraction tsym::cancel(const Fraction& f)
{
    if (isOne(*f.denom))
        return Fraction{f.num};
    else if (isZero(*f.num))
        return Fraction{f.num};
    else if (isZero(*f.denom->expand())) {
        TSYM_WARNING("Zero f.denom encountered during fraction cancellation");
        return Fraction{Undefined::create()};
    } else
        return cancelNonTrivial(f);
}
