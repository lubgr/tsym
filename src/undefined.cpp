
#include "undefined.h"
#include <cassert>
#include "basefct.h"
#include "basetypestr.h"
#include "fraction.h"
#include "logging.h"
#include "numeric.h"

tsym::Undefined::Undefined(Base::CtorKey&&)
    : Base(typestring::undefined)
{
    setDebugString();
}

const tsym::BasePtr& tsym::Undefined::create()
{
    static const BasePtr instance(std::make_shared<const Undefined>(Base::CtorKey{}));

    return instance;
}

bool tsym::Undefined::isEqual(const Base& other) const
{
    return isEqualDifferentBase(other);
}

bool tsym::Undefined::isEqualDifferentBase(const Base& other) const
{
    if (isUndefined(other))
        TSYM_ERROR("Equality request between two undefined expressions! Returns false.");

    return false;
}

std::optional<tsym::Number> tsym::Undefined::numericEval() const
{
    return std::nullopt;
}

tsym::Fraction tsym::Undefined::normal(SymbolMap&) const
{
    return Fraction{clone()};
}

tsym::BasePtr tsym::Undefined::diffWrtSymbol(const Base&) const
{
    return clone();
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

bool tsym::Undefined::isDifferent(const Base& other) const
{
    if (isUndefined(other))
        TSYM_WARNING("Difference request between two Undefined! Returns true.");

    return true;
}

bool tsym::Undefined::has(const Base&) const
/* The Base implementation checks for equality, which doesn't make sense for an Undefined. */
{
    return false;
}

tsym::BasePtr tsym::Undefined::subst(const Base& from, const BasePtr& to) const
{
    if (isUndefined(from))
        return to;
    else
        return clone();
}

int tsym::Undefined::degree(const Base&) const
/* Same as in the has-method. */
{
    return 0;
}

unsigned tsym::Undefined::complexity() const
{
    return 0;
}
