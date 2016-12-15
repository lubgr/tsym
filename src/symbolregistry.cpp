
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

void tsym::SymbolRegistry::add(const BasePtr& symbol)
{
    std::map<Name, unsigned> *reg(registry());
    std::map<Name, unsigned>::iterator lookup(reg->find(symbol->name()));

    if (lookup == reg->end()) {
        reg->insert(std::make_pair(symbol->name(), 1));
        logging::info() << "Register new symbol: " << symbol->name();
    } else
        ++lookup->second;
}

void tsym::SymbolRegistry::remove(const BasePtr& symbol)
{
    std::map<Name, unsigned> *reg(registry());
    std::map<Name, unsigned>::iterator lookup(reg->find(symbol->name()));

    assert(lookup != reg->end());

    if (--lookup->second == 0) {
        logging::info() << "Deregister symbol: " << symbol->name();
        reg->erase(lookup);
    }

    reg = registry(true);
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
