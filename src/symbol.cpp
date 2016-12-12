
#include <sstream>
#include "symbol.h"
#include "numeric.h"
#include "symbolregistry.h"

tsym::Symbol::Symbol(const Name& name, bool positive) :
    symbolName(name),
    positive(positive)
{}

tsym::Symbol::~Symbol() {}

tsym::BasePtr tsym::Symbol::create(const std::string& name)
{
    return create(Name(name));
}

tsym::BasePtr tsym::Symbol::create(const Name& name)
{
    return BasePtr(new Symbol(name, false));
}

tsym::BasePtr tsym::Symbol::createPositive(const std::string& name)
{
    return createPositive(Name(name));
}

tsym::BasePtr tsym::Symbol::createPositive(const Name& name)
{
    return BasePtr(new Symbol(name, true));
}

tsym::BasePtr tsym::Symbol::createTmpSymbol(bool positive)
{
    const Name name(getTmpName());

    return BasePtr(new Symbol(name, positive));
}

tsym::Name tsym::Symbol::getTmpName()
{
    std::ostringstream stream;
    unsigned count = 0;
    Name name;

    do {
        stream.str("");
        stream << ++count;
        name = Name("tmp", stream.str());
    } while (SymbolRegistry::count(name) != 0);

    return name;
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
