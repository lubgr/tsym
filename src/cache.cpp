
#include "cache.h"
#include <map>

namespace {
    auto& clearFunctions()
    {
        static std::map<const short*, std::function<void()>> clearFunctions;

        return clearFunctions;
    }
}

void tsym::cache::detail::registerCacheClearer(const short* address, std::function<void()>&& fct)
{
    clearFunctions()[address] = std::move(fct);
}

void tsym::cache::detail::deregisterCacheClearer(const short* address)
{
    clearFunctions().erase(address);
}

void tsym::cache::clearRegisteredCaches()
{
    for (auto& [unused, clearFctEntry] : clearFunctions())
        clearFctEntry();
}
