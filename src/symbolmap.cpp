
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
    std::map<BasePtr, BasePtr>::const_iterator it;

    for (it = map.begin(); it != map.end(); ++it)
        if (it->second->isEqual(ptr))
            return &it->first;

    return NULL;
}

tsym::BasePtr tsym::SymbolMap::replaceTmpSymbolsBackFrom(const BasePtr& ptr)
{
    std::map<BasePtr, BasePtr>::const_iterator it;
    BasePtr result(ptr);

    for (it = map.begin(); it != map.end(); ++it)
        result = result->subst(it->first, it->second);

    return result;
}
