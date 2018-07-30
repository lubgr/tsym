#ifndef TSYM_CACHE_H
#define TSYM_CACHE_H

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

namespace tsym {
    namespace cache {
        void clearRegisteredCaches();

        namespace detail {
            void registerCacheClearer(uintptr_t address, std::function<void()>&& fct);
            void deregisterCacheClearer(uintptr_t address);
        }

        template <class Key, class Value, class Hash = std::hash<Key>, class EqualTo = std::equal_to<Key>>
        struct RegisteredCache {
            /* This wrapper stores exposes the cache container publicly and automatically
             * registers and unregisteres a member function reference to clear the cache. */
            RegisteredCache()
            {
                detail::registerCacheClearer(
                  reinterpret_cast<uintptr_t>(&map), [this]() { decltype(map){}.swap(map); });
            }

            RegisteredCache(const RegisteredCache&) = delete;
            RegisteredCache& operator=(const RegisteredCache&) = delete;
            RegisteredCache(RegisteredCache&&) = delete;
            RegisteredCache& operator=(RegisteredCache&&) = delete;

            ~RegisteredCache()
            {
                detail::deregisterCacheClearer(reinterpret_cast<uintptr_t>(&map));
            }

            std::unordered_multimap<Key, Value, Hash, EqualTo> map;
        };
    }
}

#endif
