
#include <sstream>
#include "symbol.h"
#include "numeric.h"

unsigned tsym::Symbol::tmpCounter = 0;

tsym::Symbol::Symbol(const Name& name, bool positive) :
    symbolName(name),
    positive(positive)
{}

tsym::Symbol::Symbol(unsigned tmpId, bool positive) :
    symbolName(tmpId),
    positive(positive)
{}

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
    else
        return BasePtr(new Symbol(name, positive));
}

tsym::BasePtr tsym::Symbol::createPositive(const std::string& name)
{
    return createPositive(Name(name));
}

tsym::BasePtr tsym::Symbol::createPositive(const Name& name)
{
    return create(name, true);
}

tsym::BasePtr tsym::Symbol::createTmpSymbol(unsigned *counter, bool positive)
{
    unsigned newTmpCount = ++tmpCounter;

    if (counter != nullptr)
        *counter = newTmpCount;

    return BasePtr(new Symbol(newTmpCount, positive));
}

bool tsym::Symbol::isEqual(const BasePtr& other) const
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

bool tsym::Symbol::isSymbol() const
{
    return true;
}

const tsym::Name& tsym::Symbol::name() const
{
    return symbolName;
}
