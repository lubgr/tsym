#ifndef TSYM_SUBRESULTANTGCD_H
#define TSYM_SUBRESULTANTGCD_H

#include "gcd.h"

namespace tsym {
    class SubresultantGcd : public Gcd {
        private:
            BasePtr gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrCtr& L) const;
            BasePtr gcd(const BasePtr& u, const BasePtr& v, const BasePtrCtr& L) const;
    };
}

#endif
