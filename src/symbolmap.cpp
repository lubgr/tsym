
#include "symbolmap.h"
#include "symbol.h"

tsym::SymbolMap::SymbolMap() {}

tsym::BasePtr tsym::SymbolMap::getTmpSymbolAndStore(const BasePtr& ptr)
{
    const BasePtr *existingReplacement(cache.retrieve(ptr));
    BasePtr replacement;

    if (existingReplacement)
        return *existingReplacement;

    replacement = Symbol::createTmpSymbol(ptr->isPositive());

    return cache.insertAndReturn(ptr, replacement);
}

tsym::BasePtr tsym::SymbolMap::replaceTmpSymbolsBackFrom(const BasePtr& orig)
{
    BasePtr result(orig);

    for (const auto& entry : cache.getUnderlyingMap())
        result = result->subst(entry.second, entry.first);

    if (result->isUndefined())
        /* Catch this in advance to avoid a possible comparison with an Undefined instance: */
        return result;
    else if (result->isDifferent(orig))
        /* There might be nested replacements by temporary symbols. */
        return replaceTmpSymbolsBackFrom(result);
    else
        return result;
}
