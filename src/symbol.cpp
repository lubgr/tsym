
#include <sstream>
#include "symbol.h"
#include "numeric.h"
#include "symbolregistry.h"

tsym::Symbol::Symbol(const Name& name) :
    symbolName(name)
{}

tsym::Symbol::~Symbol() {}

tsym::BasePtr tsym::Symbol::create(const std::string& name)
{
    return create(Name(name));
}

tsym::BasePtr tsym::Symbol::create(const Name& name)
{
    return BasePtr(new Symbol(name));
}

tsym::BasePtr tsym::Symbol::createTmpSymbol()
{
    const Name name(getTmpName());

    return BasePtr(new Symbol(name));
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
        return symbolName == other->name();
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

bool tsym::Symbol::isSymbol() const
{
    return true;
}

const tsym::Name& tsym::Symbol::name() const
{
    return symbolName;
}
