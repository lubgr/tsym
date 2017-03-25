#ifndef TSYM_CACHE_H
#define TSYM_CACHE_H

#include <unordered_map>
#include <string>
#include "logging.h"

namespace tsym {
    template<class S, class T> class Cache {
        /* Cache for key-value-pairs with std::hash and std::equal_to specializations. They shall be
         * instantiated as local static variables (anything else doesn't make sense) and serve as a
         * lookup-pool for expensive transformations and symbols. The latter enables usage of the
         * same Base object, when identicval Symbol instances are created, which speeds up an
         * equality comparison. */
        public:
            Cache() {};
            Cache(const Cache& other) = delete;
            const Cache& operator = (const Cache& rhs) = delete;

            const T& insertAndReturn(const S& key, const T& value)
            {
                return rep[key] = value;
            }

            const T *retrieve(const S& key) const
            {
                const auto lookup = rep.find(key);

                return lookup != rep.end() ? &lookup->second : nullptr;
            }

        private:
            std::unordered_map<S, T> rep;
            std::string description;
    };
}

#endif
