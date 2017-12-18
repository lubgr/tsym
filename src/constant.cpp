
#include <cmath>
#include "constant.h"
#include "symbolmap.h"
#include "numeric.h"

tsym::Constant::Constant(Type type, Name&& name) :
    type(type),
    constantName(std::move(name))
{
    setDebugString();
}

tsym::BasePtr tsym::Constant::createPi()
{
    return create(Type::PI, Name("pi"));
}

tsym::BasePtr tsym::Constant::createE()
{
    return create(Type::E, Name("e"));
}

tsym::BasePtr tsym::Constant::create(Type type, Name&& name)
{
    return instantiate([type, &name]() { return new Constant(type, std::move(name)); });
}

tsym::Constant::~Constant() {}

bool tsym::Constant::isEqualDifferentBase(const BasePtr& other) const
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

size_t tsym::Constant::hash() const
{
    typedef std::underlying_type<Type>::type EnumType;

    return std::hash<EnumType>{}(static_cast<EnumType>(type));
}

unsigned tsym::Constant::complexity() const
{
    return 4;
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
