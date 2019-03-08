#ifndef TSYM_PRIMITIVEGCD_H
#define TSYM_PRIMITIVEGCD_H

#include "gcd.h"

namespace tsym {
    class PrimitiveGcd : public Gcd {
      private:
        BasePtr gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const override;
    };
}

#endif
