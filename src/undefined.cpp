
#include <cassert>
#include "undefined.h"
#include "numeric.h"
#include "logging.h"

tsym::Undefined::Undefined() {}

tsym::Undefined::~Undefined() {}

tsym::BasePtr tsym::Undefined::create()
{
    return BasePtr(new Undefined());
}

bool tsym::Undefined::isEqual(const BasePtr&) const
{
    return false;
}

bool tsym::Undefined::sameType(const BasePtr& other) const
{
    return other->isUndefined();
}

tsym::Number tsym::Undefined::numericEval() const
{
    return Number::createUndefined();
}

tsym::Fraction tsym::Undefined::normal(SymbolMap&) const
{
    return Fraction(clone());
}

tsym::BasePtr tsym::Undefined::diffWrtSymbol(const BasePtr&) const
{
    return clone();
}

std::string tsym::Undefined::typeStr() const
{
    return "Undefined";
}

bool tsym::Undefined::isUndefined() const
{
    return true;
}

bool tsym::Undefined::isDifferent(const BasePtr&) const
{
    return true;
}

tsym::BasePtr tsym::Undefined::subst(const BasePtr& from, const BasePtr& to) const
{
    if (from->isUndefined())
        return to;
    else
        return clone();
}
