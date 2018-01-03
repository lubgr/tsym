
#include <cassert>
#include "undefined.h"
#include "numeric.h"
#include "logging.h"

tsym::Undefined::Undefined()
{
    setDebugString();
}

const tsym::BasePtr& tsym::Undefined::create()
{
    static const auto instance = instantiate([]() { return new Undefined(); });

    return instance;
}

bool tsym::Undefined::isEqual(const BasePtr& other) const
{
    return isEqualDifferentBase(other);
}

bool tsym::Undefined::isEqualDifferentBase(const BasePtr& other) const
{
    if (other->isUndefined())
        TSYM_ERROR("Equality request between two undefined expressions! Returns false.");

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

bool tsym::Undefined::isPositive() const
{
    return false;
}

bool tsym::Undefined::isNegative() const
{
    return false;
}

size_t tsym::Undefined::hash() const
{
    return 1;
}

bool tsym::Undefined::isUndefined() const
{
    return true;
}

bool tsym::Undefined::isDifferent(const BasePtr& other) const
{
    if (other->isUndefined())
        TSYM_WARNING("Difference request between two Undefined! Returns true.");

    return true;
}

bool tsym::Undefined::has(const BasePtr&) const
    /* The Base implementation checks for equality, which doesn't make sense for an Undefined. */
{
    return false;
}

tsym::BasePtr tsym::Undefined::subst(const BasePtr& from, const BasePtr& to) const
{
    if (from->isUndefined())
        return to;
    else
        return clone();
}

int tsym::Undefined::degree(const BasePtr&) const
    /* Same as in the has-method. */
{
    return 0;
}

unsigned tsym::Undefined::complexity() const
{
    return 0;
}
