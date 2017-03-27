
#include <sstream>
#include "symbol.h"
#include "cache.h"
#include "numeric.h"

unsigned tsym::Symbol::tmpCounter = 0;

tsym::Symbol::Symbol(const Name& name, bool positive) :
    symbolName(name),
    positive(positive)
{
    setDebugString();
}

tsym::Symbol::Symbol(unsigned tmpId, bool positive) :
    symbolName(tmpId),
    positive(positive)
{
    setDebugString();
}

tsym::Symbol::~Symbol()
{
    if (symbolName.isNumericId())
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
    if (name.getName().empty())
        return Undefined::create();
    return createNonEmptyName(name, positive);
}

tsym::BasePtr tsym::Symbol::createNonEmptyName(const Name& name, bool positive)
{
    static Cache<BasePtr, BasePtr> pool;
    const BasePtr symbol(new Symbol(name, positive));
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
    return BasePtr(new Symbol(++tmpCounter, positive));
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
    return Number::createUndefined();
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

bool tsym::Symbol::isSymbol() const
{
    return true;
}

const tsym::Name& tsym::Symbol::name() const
{
    return symbolName;
}
