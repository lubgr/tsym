#ifndef TSYM_CACHE_H
#define TSYM_CACHE_H

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

namespace tsym {
    void clearRegisteredCaches();

    namespace detail {
        void registerCacheClearer(const short* address, std::function<void()>&& fct);
        void deregisterCacheClearer(const short* address);
    }

    template <class Key, class Value, class Hash = std::hash<Key>, class EqualTo = std::equal_to<Key>>
    struct RegisteredCache {
        /* This wrapper stores exposes the cache container publicly and automatically
         * registers and unregisteres a member function reference to clear the cache. */
        RegisteredCache()
        {
            detail::registerCacheClearer(&address, [this]() { decltype(map){}.swap(map); });
        }

        RegisteredCache(const RegisteredCache&) = delete;
        RegisteredCache& operator=(const RegisteredCache&) = delete;
        RegisteredCache(RegisteredCache&&) = delete;
        RegisteredCache& operator=(RegisteredCache&&) = delete;

        ~RegisteredCache()
        {
            detail::deregisterCacheClearer(&address);
        }

        std::unordered_multimap<Key, Value, Hash, EqualTo> map;
        const short address = 0;
    };
}

#endif
