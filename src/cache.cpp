
#include "cache.h"
#include <map>

namespace {
    auto& clearFunctions()
    {
        static std::map<uintptr_t, std::function<void()>> clearFunctions;

        return clearFunctions;
    }
}

void tsym::cache::detail::registerCacheClearer(uintptr_t address, std::function<void()>&& fct)
{
    clearFunctions()[address] = fct;
}

void tsym::cache::detail::deregisterCacheClearer(uintptr_t address)
{
    clearFunctions().erase(address);
}

void tsym::cache::clearRegisteredCaches()
{
    for (auto& clearFctEntry : clearFunctions())
        clearFctEntry.second();
}
