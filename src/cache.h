#ifndef TSYM_CACHE_H
#define TSYM_CACHE_H

#include <memory>
#include <unordered_map>
#include <string>

namespace tsym {
    template<class Key, class Value, bool forceCaching = false> class Cache {
        /* Cache for key-value-pairs with std::hash and std::equal_to specializations. It serves as
         * a lookup-pool for expensive transformations and/or symbols. The latter enables usage of
         * the same Base object, when identical Symbol instances are created, which speeds up an
         * equality comparison. */
        public:
            Cache() {};
            Cache(const Cache& other) = delete;
            const Cache& operator = (const Cache& rhs) = delete;

            const Value& insertAndReturn(const Key& key, const Value& value)
            {
                rep.insert(std::make_pair(key, value));

                return value;
            }

            const Value *retrieve(const Key& key) const
            {
                const auto lookup = rep.find(key);

                return lookup != rep.end() ? &lookup->second : nullptr;
            }

            typename std::unordered_map<Key, Value>::const_iterator begin() const
            {
                return rep.begin();
            }

            typename std::unordered_map<Key, Value>::const_iterator end() const
            {
                return rep.end();
            }

        private:
            std::unordered_multimap<Key, Value> rep;
    };

#ifdef TSYM_NO_OPTIONAL_CACHE
    template<class Key, class Value> class Cache<Key, Value, false> {
        public:
            Cache() = default;
            ~Cache() = default;
            Cache(const Cache& other) = delete;
            const Cache& operator = (const Cache& rhs) = delete;

            const Value& insertAndReturn(const Key&, const Value& value)
            {
                return value;
            }

            const Value *retrieve(const Key&) const
            {
                return nullptr;
            }
    };
#endif
}

#endif
