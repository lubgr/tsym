#ifndef TSYM_SUBRESULTANTGCD_H
#define TSYM_SUBRESULTANTGCD_H

#include "gcdstrategy.h"

namespace tsym {
    class SubresultantGcd : public GcdStrategy {
        private:
            BasePtr gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const;
            BasePtr gcd(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const;
    };
}

#endif
