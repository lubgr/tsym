#ifndef TSYM_CACHE_H
#define TSYM_CACHE_H

#include <memory>
#include <unordered_map>
#include <string>
#include "options.h"

namespace tsym {
    template<class S, class T, bool forceCaching = false> class Cache {
        /* Cache for key-value-pairs with std::hash and std::equal_to specializations. It serves as
         * a lookup-pool for expensive transformations and/or symbols. The latter enables usage of
         * the same Base object, when identical Symbol instances are created, which speeds up an
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

            typename std::unordered_map<S, T>::const_iterator begin() const
            {
                return rep.begin();
            }

            typename std::unordered_map<S, T>::const_iterator end() const
            {
                return rep.end();
            }

        private:
            std::unordered_map<S, T> rep;
    };

    template<class S, class T> class Cache<S, T, false> {
        public:
            Cache() :
                useOptionalCaching(options::get<bool>(Option::USE_OPTIONAL_CACHING))
            {
                if (useOptionalCaching)
                    rep = std::unique_ptr<std::unordered_map<S, T>>(new std::unordered_map<S, T>());
            };

            ~Cache() = default;
            Cache(const Cache& other) = delete;
            const Cache& operator = (const Cache& rhs) = delete;

            const T& insertAndReturn(const S& key, const T& value)
            {
                return useOptionalCaching ? (*rep)[key] = value : value;
            }

            const T *retrieve(const S& key) const
            {
                if (!useOptionalCaching)
                    return nullptr;

                const auto lookup = rep->find(key);

                return lookup != rep->end() ? &lookup->second : nullptr;
            }

        private:
            const bool useOptionalCaching;
            std::unique_ptr<std::unordered_map<S, T>> rep;
    };
}

#endif
