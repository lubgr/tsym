
#include "symbolmap.h"
#include "symbol.h"

const tsym::BasePtr& tsym::SymbolMap::getTmpSymbolAndStore(const BasePtr& ptr)
{
    const auto lookup = rep.find(ptr);

    if (lookup != cend(rep))
        return lookup->second;

    return rep.insert({ptr, Symbol::createTmpSymbol(ptr->isPositive())}).first->second;
}

tsym::BasePtr tsym::SymbolMap::replaceTmpSymbolsBackFrom(const BasePtr& orig) const
{
    BasePtr result(orig);

    for (const auto& entry : rep)
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
