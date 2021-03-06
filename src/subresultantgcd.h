#ifndef TSYM_SUBRESULTANTGCD_H
#define TSYM_SUBRESULTANTGCD_H

#include "gcd.h"

namespace tsym {
    class SubresultantGcd : public Gcd {
      private:
        BasePtr gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const override;
        BasePtr gcd(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const;
    };
}

#endif
