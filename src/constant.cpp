
#include "constant.h"
#include <cmath>
#include "fraction.h"
#include "numeric.h"
#include "symbolmap.h"

tsym::Constant::Constant(Type type, Name&& name, Base::CtorKey&&)
    : type(type)
    , constantName(std::move(name))
{
    setDebugString();
}

const tsym::BasePtr& tsym::Constant::createPi()
{
    static const BasePtr instance = create(Type::PI, Name("pi"));

    return instance;
}

const tsym::BasePtr& tsym::Constant::createE()
{
    static const BasePtr instance = create(Type::E, Name("e"));

    return instance;
}

tsym::BasePtr tsym::Constant::create(Type type, Name&& name)
{
    return std::make_shared<const Constant>(type, std::move(name), Base::CtorKey{});
}

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
