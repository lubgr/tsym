
#include <sstream>
#include "symbol.h"
#include "undefined.h"
#include "fraction.h"
#include "cache.h"
#include "numeric.h"
#include "logging.h"

unsigned tsym::Symbol::tmpCounter = 0;

tsym::Symbol::Symbol(const Name& name, bool positive) :
    symbolName(name),
    positive(positive)
{
    setDebugString();
}

tsym::Symbol::Symbol(unsigned tmpId, bool positive) :
    symbolName(std::string(tmpSymbolNamePrefix) + std::to_string(tmpId)),
    positive(positive)
{
    setDebugString();
}

tsym::Symbol::~Symbol()
{
    if (symbolName.getName().find(tmpSymbolNamePrefix) == 0)
        --tmpCounter;
}

tsym::BasePtr tsym::Symbol::create(const std::string& name)
{
    return create(Name(name));
}

tsym::BasePtr tsym::Symbol::create(const Name& name)
{
    return create(name, false);
}

tsym::BasePtr tsym::Symbol::create(const Name& name, bool positive)
{
    if (name.getName().empty()) {
        TSYM_ERROR("Creating Symbol with empty name, return Undefined instead");
        return Undefined::create();
    } else if (name.getName().find(tmpSymbolNamePrefix) == 0) {
        TSYM_ERROR("");
        return createTmpSymbol(positive);
    }

    return createNonEmptyName(name, positive);
}

tsym::BasePtr tsym::Symbol::createNonEmptyName(const Name& name, bool positive)
{
    const BasePtr symbol(instantiate([&name, positive]() { return new Symbol(name, positive); }));
    static Cache<BasePtr, BasePtr> pool;
    const BasePtr *cached = pool.retrieve(symbol);

    if (cached != nullptr)
        return *cached;
    else
        return pool.insertAndReturn(symbol, symbol);
}

tsym::BasePtr tsym::Symbol::createPositive(const std::string& name)
{
    return createPositive(Name(name));
}

tsym::BasePtr tsym::Symbol::createPositive(const Name& name)
{
    return create(name, true);
}

tsym::BasePtr tsym::Symbol::createTmpSymbol(bool positive)
{
    ++tmpCounter;

    return instantiate([positive]() { return new Symbol(tmpCounter, positive); });
}

bool tsym::Symbol::isEqualDifferentBase(const BasePtr& other) const
{
    if (other->isSymbol())
        return isEqualOtherSymbol(other);
    else
        return false;
}

bool tsym::Symbol::isEqualOtherSymbol(const BasePtr& other) const
{
    if (symbolName == other->name())
        return positive == other->isPositive();
    else
        return false;
}

bool tsym::Symbol::sameType(const BasePtr& other) const
{
    return other->isSymbol();
}

tsym::Number tsym::Symbol::numericEval() const
{
    throw std::logic_error("Symbol can't be numerically evaluated");
}

tsym::Fraction tsym::Symbol::normal(SymbolMap&) const
{
    return Fraction(clone());
}

tsym::BasePtr tsym::Symbol::diffWrtSymbol(const BasePtr& symbol) const
{
    return isEqual(symbol) ? Numeric::one() : Numeric::zero();
}

std::string tsym::Symbol::typeStr() const
{
    return "Symbol";
}

bool tsym::Symbol::isPositive() const
{
    return positive;
}

bool tsym::Symbol::isNegative() const
{
    return false;
}

size_t tsym::Symbol::hash() const
{
    const size_t nameHash = std::hash<Name>{}(symbolName);
    const size_t signHash = std::hash<bool>{}(positive);

    return nameHash ^ (signHash << 1);
}

unsigned tsym::Symbol::complexity() const
{
    return 5;
}

bool tsym::Symbol::isSymbol() const
{
    return true;
}

const tsym::Name& tsym::Symbol::name() const
{
    return symbolName;
}
