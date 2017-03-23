
#include <unordered_map>
#include <cassert>
#include "base.h"
#include "logging.h"
#include "cache.h"

namespace tsym {
    namespace {
        class BasePtrEquality {
            public:
                bool operator () (const BasePtr& lhs, const BasePtr& rhs) const
                {
                    return lhs->isEqual(rhs);
                }
        };

        class BasePtrListEquality {
            public:
                bool operator () (const BasePtrList& lhs, const BasePtrList& rhs) const
                {
                    return lhs.isEqual(rhs);
                }
        };

        typedef std::unordered_map<BasePtr, BasePtr, std::hash<BasePtr>, BasePtrEquality>
            CacheBpToBp;
        typedef std::unordered_map<BasePtrList, BasePtr, std::hash<BasePtrList>,
                BasePtrListEquality> CacheBplToBp;
        typedef std::unordered_map<BasePtrList, BasePtrList, std::hash<BasePtrList>,
                BasePtrListEquality> CacheBplToBpl;

        CacheBpToBp& getBpToBpCache(cache::Method index)
        {
            static CacheBpToBp cache[2];

            assert(index == cache::NORMAL || index == cache::SYMBOL);

            return cache[index];
        }

        CacheBplToBp& getBplToBpCache(cache::Method index)
        {
            static CacheBplToBp cache[2];

            assert(index == cache::GCD || index == cache::EXPANSION);

            (void) index;

            return index == cache::GCD ? cache[0] : cache[1];
        }

        CacheBplToBpl& getBplToBplCache(cache::Method index)
        {
            static CacheBplToBpl cache;

            assert(index == cache::POLY_DIVISION);

            (void) index;

            return cache;
        }

        template<class S, class T> const tsym::BasePtr *lookupAndReturn(const S& cache, const T& key)
        {
            const auto lookup = cache.find(key);

            return lookup != cache.end() ? &lookup->second : nullptr;
        }

        template<class S, class T> const tsym::BasePtr& insertAndReturn(S& cache, const T& key,
                const BasePtr& value)
        {
            cache[key] = value;

            return value;
        }
    }
}

const tsym::BasePtr *tsym::cache::retrieve(const BasePtr& key, Method index)
{
    auto& cache(getBpToBpCache(index));

    return lookupAndReturn(cache, key);
}

const tsym::BasePtr *tsym::cache::retrieve(const BasePtrList& key, Method index)
{
    auto& cache(getBplToBpCache(index));

    return lookupAndReturn(cache, key);
}

const tsym::BasePtrList *tsym::cache::retrieveList(const BasePtrList& key, Method index)
{
    auto& cache(getBplToBplCache(index));
    const auto lookup = cache.find(key);

    return lookup != cache.end() ? &lookup->second : nullptr;
}

const tsym::BasePtr& tsym::cache::insertAndGet(const BasePtr& key, const BasePtr& value,
        Method index)
{
    auto& cache(getBpToBpCache(index));

    if (index == SYMBOL)
        TSYM_INFO("Cache symbol: ", key);
    else
        TSYM_INFO("Cache normalization ", key, " : ", value);

    insertAndReturn(cache, key, value);

    return insertAndReturn(cache, value, value);
}

const tsym::BasePtr& tsym::cache::insertAndGet(const BasePtrList& key, const BasePtr& value,
        Method index)
{
    auto& cache(getBplToBpCache(index));

    TSYM_INFO("Cache ", index == GCD ? "gcd " : "expansion ", key, " : ", value);

    return insertAndReturn(cache, key, value);
}

const tsym::BasePtrList& tsym::cache::insertAndGet(const BasePtrList& key, const BasePtrList& value,
        Method index)
{
    auto& cache(getBplToBplCache(index));

    assert(index == POLY_DIVISION);

    TSYM_INFO("Cache polynomial division ", key, " : ", value);

    cache[key] = value;

    return value;
}
