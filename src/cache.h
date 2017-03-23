#ifndef TSYM_CACHE_H
#define TSYM_CACHE_H

#include "baseptr.h"

namespace tsym {
    namespace cache {
        enum Method { NORMAL, SYMBOL, GCD, EXPANSION, POLY_DIVISION };

        /* Retrieval functions return nullptr, if not found: */
        const BasePtr *retrieve(const BasePtr& key, Method index);
        const BasePtr *retrieve(const BasePtrList& key, Method index);
        const BasePtrList *retrieveList(const BasePtrList& key, Method index);

        const BasePtr& insertAndGet(const BasePtr& key, const BasePtr& value, Method index);
        const BasePtr& insertAndGet(const BasePtrList& key, const BasePtr& value, Method index);
        const BasePtrList& insertAndGet(const BasePtrList& key, const BasePtrList& value, Method index);
    }
}

#endif
