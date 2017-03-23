
#include "numeric.h"
#include "function.h"

tsym::Function::Function(const BasePtrList& args, const std::string& name) :
    Base(args),
    functionName(name)
{}

tsym::Function::~Function() {}

bool tsym::Function::isEqualDifferentBase(const BasePtr& other) const
{
    if (sameType(other))
        return name() == other->name() && ops.isEqual(other->operands());
    else
        return false;
}

bool tsym::Function::sameType(const BasePtr& other) const
{
    return other->isFunction();
}

std::string tsym::Function::typeStr() const
{
    return "Function";
}

size_t tsym::Function::hash() const
{
    return std::hash<Name>{}(functionName) ^ (std::hash<BasePtrList>{}(ops) << 1);
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
