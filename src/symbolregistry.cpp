
#include <map>
#include <cassert>
#include "symbol.h"
#include "symbolregistry.h"

namespace tsym {
    namespace {
        std::map<Name, unsigned> *registry()
        {
            static std::map<Name, unsigned> *symbols = NULL;

            if (symbols == NULL)
                symbols = new std::map<Name, unsigned>();

            return symbols;
        }
    }
}

void tsym::SymbolRegistry::add(const BasePtr& symbol)
{
    std::map<Name, unsigned> *reg(registry());
    std::map<Name, unsigned>::iterator lookup(reg->find(symbol->name()));

    if (lookup == reg->end())
        reg->insert(std::make_pair(symbol->name(), 1));
    else
        ++lookup->second;
}

void tsym::SymbolRegistry::remove(const BasePtr& symbol)
{
    std::map<Name, unsigned> *reg(registry());
    std::map<Name, unsigned>::iterator lookup(reg->find(symbol->name()));

    assert(lookup != reg->end());

    if (--lookup->second == 0)
        reg->erase(lookup);

    if (reg->empty())
        delete reg;
}

unsigned tsym::SymbolRegistry::count(const Name& name)
{
    const std::map<Name, unsigned> *reg(registry());
    const std::map<Name, unsigned>::const_iterator lookup(reg->find(name));

    if (lookup == reg->end())
        return 0;
    else
        return lookup->second;
}
