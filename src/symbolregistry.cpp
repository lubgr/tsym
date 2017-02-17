
#include <map>
#include <cassert>
#include "symbol.h"
#include "symbolregistry.h"
#include "logging.h"

namespace tsym {
    namespace {
        std::map<Name, unsigned> *registry(bool deleteIfEmpty = false)
        {
            static std::map<Name, unsigned> *symbols = NULL;

            if (symbols != NULL && symbols->empty() && deleteIfEmpty) {
                delete symbols;
                symbols = NULL;
            } else if (symbols == NULL)
                symbols = new std::map<Name, unsigned>();

            return symbols;
        }
    }
}

void tsym::SymbolRegistry::add(const Name& symbolName)
{
    std::map<Name, unsigned> *reg(registry());
    std::map<Name, unsigned>::iterator lookup(reg->find(symbolName));

    if (lookup == reg->end()) {
        reg->insert(std::make_pair(symbolName, 1));
        TSYM_INFO("Register new symbol: ", symbolName);
    } else
        ++lookup->second;
}

void tsym::SymbolRegistry::remove(const Name& symbolName)
{
    std::map<Name, unsigned> *reg(registry());
    std::map<Name, unsigned>::iterator lookup(reg->find(symbolName));

    assert(lookup != reg->end());

    if (--lookup->second == 0) {
        TSYM_INFO("Deregister symbol: ", symbolName);
        reg->erase(lookup); }

    reg = registry(true);
}

unsigned tsym::SymbolRegistry::count(const Name& symbolName)
{
    const std::map<Name, unsigned> *reg(registry());
    const std::map<Name, unsigned>::const_iterator lookup(reg->find(symbolName));

    if (lookup == reg->end())
        return 0;
    else
        return lookup->second;
}
