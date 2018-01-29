#ifndef TSYM_SYMBOLMAP_H
#define TSYM_SYMBOLMAP_H

#include <unordered_map>
#include "baseptr.h"

namespace tsym {
    class SymbolMap {
        /* Utility class for the implementation of normal-methods. It provides an interface for
         * replacing any BasePtr object with a temporary, unique Symbol. This replacement is saved
         * internally in a map for a back-replacement after further steps of normalization. */
        public:
            SymbolMap() = default;
            /* One instance at a time shall be used during normalization, there is no point in
             * copying/moving/assigning them. */
            SymbolMap(const SymbolMap& other) = delete;
            SymbolMap& operator = (const SymbolMap& rhs) = delete;
            SymbolMap(SymbolMap&& other) = delete;
            SymbolMap& operator = (SymbolMap&& rhs) = delete;
            ~SymbolMap() = default;

            const BasePtr& getTmpSymbolAndStore(const BasePtr& ptr);
            BasePtr replaceTmpSymbolsBackFrom(const BasePtr& ptr) const;

        private:
            std::unordered_map<BasePtr, BasePtr> rep;
    };
}

#endif
