#ifndef TSYM_PRIMITIVEGCD_H
#define TSYM_PRIMITIVEGCD_H

#include "gcdstrategy.h"

namespace tsym {
    class PrimitiveGcd : public GcdStrategy {
        private:
            BasePtr gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const;
    };
}

#endif
