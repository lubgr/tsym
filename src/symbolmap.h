#ifndef TSYM_SYMBOLMAP_H
#define TSYM_SYMBOLMAP_H

#include <cassert>
#include <map>
#include "baseptr.h"
#include "symbol.h"

namespace tsym {
    class SymbolMap {
        /* Utility class for the implementation of normal-methods. It provides an interface for
         * replacing any BasePtr object with a temporary, unique Symbol. This replacement is saved
         * internally in a map for a back-replacement after further steps of normalization. */
        public:
            BasePtr getTmpSymbolAndStore(const BasePtr& ptr);
            BasePtr replaceTmpSymbolsBackFrom(const BasePtr& ptr);

        private:
            const BasePtr *getExisting(const BasePtr& ptr);

            struct CompareSymbols {
                bool operator () (const BasePtr& lhs, const BasePtr& rhs) const
                {
                    assert(lhs->isSymbol() && rhs->isSymbol());

                    return lhs->name() < rhs->name();
                }
            };

            std::map<BasePtr, BasePtr, CompareSymbols> map;
    };
}

#endif
