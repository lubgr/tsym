
#include "symbolmap.h"
#include <boost/range/numeric.hpp>
#include "basefct.h"
#include "symbol.h"

const tsym::BasePtr& tsym::SymbolMap::getTmpSymbolAndStore(const BasePtr& ptr)
{
    if (const auto lookup = rep.find(ptr); lookup != cend(rep))
        return lookup->second;

    return rep.insert({ptr, Symbol::createTmpSymbol(ptr->isPositive())}).first->second;
}

tsym::BasePtr tsym::SymbolMap::replaceTmpSymbolsBackFrom(const BasePtr& orig) const
{
    const BasePtr result = boost::accumulate(
      rep, orig, [](const auto& bp, const auto& entry) { return bp->subst(*entry.second, entry.first); });

    if (isUndefined(*result))
        /* Catch this in advance to avoid a possible comparison with an Undefined instance: */
        return result;
    else if (result->isDifferent(*orig))
        /* There might be nested replacements by temporary symbols. */
        return replaceTmpSymbolsBackFrom(result);
    else
        return result;
}
