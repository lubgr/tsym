
#include <cmath>
#include "constant.h"
#include "symbolmap.h"
#include "numeric.h"

tsym::Constant::Constant(Type type, const Name& name) :
    type(type),
    constantName(name)
{}

tsym::BasePtr tsym::Constant::createPi()
{
    return BasePtr(new Constant(Type::PI, Name("pi")));
}

tsym::BasePtr tsym::Constant::createE()
{
    return BasePtr(new Constant(Type::E, Name("e")));
}

tsym::Constant::~Constant() {}

bool tsym::Constant::isEqual(const BasePtr& other) const
{
    if (other->isConstant())
        return name() == other->name();
    else
        return false;
}

bool tsym::Constant::sameType(const BasePtr& other) const
{
    return other->isConstant();
}

tsym::Number tsym::Constant::numericEval() const
{
    switch (type) {
        case Type::PI:
            return M_PI;
        case Type::E:
            return M_E;
        default:
            return 0.0;
    }
}

tsym::Fraction tsym::Constant::normal(SymbolMap& map) const
{
    const BasePtr replacement(map.getTmpSymbolAndStore(clone()));

    return Fraction(replacement);
}

tsym::BasePtr tsym::Constant::diffWrtSymbol(const BasePtr&) const
{
    return Numeric::zero();
}

std::string tsym::Constant::typeStr() const
{
    return "Constant";
}

bool tsym::Constant::isPositive() const
{
    return true;
}

bool tsym::Constant::isNegative() const
{
    return false;
}

bool tsym::Constant::isNumericallyEvaluable() const
{
    return true;
}

bool tsym::Constant::isConstant() const
{
    return true;
}

const tsym::Name& tsym::Constant::name() const
{
    return constantName;
}
