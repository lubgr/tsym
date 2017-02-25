
#include <map>
#include "symbolmap.h"
#include "symbol.h"

tsym::BasePtr tsym::SymbolMap::getTmpSymbolAndStore(const BasePtr& ptr)
{
    const BasePtr *existingReplacement(getExisting(ptr));
    BasePtr replacement;

    if (existingReplacement)
        return *existingReplacement;

    replacement = Symbol::createTmpSymbol();

    map.insert(std::make_pair(replacement, ptr));

    return replacement;
}

const tsym::BasePtr *tsym::SymbolMap::getExisting(const BasePtr& ptr)
{
    for (const auto& entry : map)
        if (entry.second->isEqual(ptr))
            return &entry.first;

    return nullptr;
}

tsym::BasePtr tsym::SymbolMap::replaceTmpSymbolsBackFrom(const BasePtr& orig)
{
    BasePtr result(orig);

    for (const auto & entry : map)
        result = result->subst(entry.first, entry.second);

    if (result->isUndefined())
        /* Catch this in advance to avoid a possible comparison with an Undefined instance: */
        return result;
    else if (result->isDifferent(orig))
        /* There might be nested replacements by temporary symbols. */
        return replaceTmpSymbolsBackFrom(result);
    else
        return result;
}
