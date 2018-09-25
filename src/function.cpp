
#include "function.h"
#include <boost/functional/hash.hpp>
#include "baseptrlistfct.h"
#include "numeric.h"

tsym::Function::Function(const BasePtrList& args, Name&& name)
    : Base(args)
    , functionName{std::move(name)}
{}

bool tsym::Function::isEqualDifferentBase(const Base& other) const
{
    if (sameType(other))
        return name() == other.name() && areEqual(ops, other.operands());
    else
        return false;
}

bool tsym::Function::sameType(const Base& other) const
{
    return other.isFunction();
}

std::string_view tsym::Function::typeStr() const
{
    return "Function";
}

size_t tsym::Function::hash() const
{
    size_t seed = 0;

    boost::hash_combine(seed, functionName);
    boost::hash_combine(seed, ops);

    return seed;
}

bool tsym::Function::isFunction() const
{
    return true;
}

tsym::BasePtr tsym::Function::constTerm() const
{
    return Numeric::one();
}

tsym::BasePtr tsym::Function::nonConstTerm() const
{
    return clone();
}

const tsym::Name& tsym::Function::name() const
{
    return functionName;
}

bool tsym::Function::isConst() const
{
    return false;
}
