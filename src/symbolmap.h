#ifndef TSYM_SYMBOLMAP_H
#define TSYM_SYMBOLMAP_H

#include <cassert>
#include <map>
#include "baseptr.h"
#include "cache.h"

namespace tsym {
    class SymbolMap {
        /* Utility class for the implementation of normal-methods. It provides an interface for
         * replacing any BasePtr object with a temporary, unique Symbol. This replacement is saved
         * internally in a map for a back-replacement after further steps of normalization. */
        public:
            SymbolMap() = default;
            SymbolMap(const SymbolMap& other) = delete;
            const SymbolMap& operator = (const SymbolMap& rhs) = delete;

            BasePtr getTmpSymbolAndStore(const BasePtr& ptr);
            BasePtr replaceTmpSymbolsBackFrom(const BasePtr& ptr);

        private:
            Cache<BasePtr, BasePtr, true> cache;
    };
}

#endif
