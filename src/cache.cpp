
#include "cache.h"
#include <map>

namespace {
    auto& clearFunctions()
    {
        static std::map<const short*, std::function<void()>> clearFunctions;

        return clearFunctions;
    }
}

void tsym::detail::registerCacheClearer(const short* address, std::function<void()>&& fct)
{
    clearFunctions()[address] = std::move(fct);
}

void tsym::detail::deregisterCacheClearer(const short* address)
{
    clearFunctions().erase(address);
}

void tsym::clearRegisteredCaches()
{
    for ([[maybe_unused]] auto& [unused, clearFctEntry] : clearFunctions())
        clearFctEntry();
}
