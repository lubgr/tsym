
#include <sstream>
#include "base.h"
#include "baseptr.h"
#include "numeric.h"
#include "symbolmap.h"
#include "product.h"
#include "name.h"
#include "undefined.h"
#include "fraction.h"
#include "cache.h"
#include "plaintextprintengine.h"
#include "ctr.h"
#include "printer.h"
#include "logging.h"

tsym::Base::Base(const BasePtrCtr& operands) :
    ops(operands)
{}

bool tsym::Base::isZero() const
{
    return false;
}

bool tsym::Base::isOne() const
{
    return false;
}

bool tsym::Base::isNumericallyEvaluable() const
{
    if (ops.empty())
        return false;
    else
        return ctr::areElementsNumericallyEvaluable(ops);
}

bool tsym::Base::isUndefined() const
{
    return false;
}

bool tsym::Base::isSymbol() const
{
    return false;
}

bool tsym::Base::isNumeric() const
{
    return false;
}

bool tsym::Base::isPower() const
{
    return false;
}

bool tsym::Base::isNumericPower() const
{
    return false;
}

bool tsym::Base::isSum() const
{
    return false;
}

bool tsym::Base::isProduct() const
{
    return false;
}

bool tsym::Base::isFunction() const
{
    return false;
}

bool tsym::Base::isConstant() const
{
    return false;
}

bool tsym::Base::isEqual(const BasePtr& other) const
{
    return this == &*other || isEqualDifferentBase(other);
}

bool tsym::Base::isDifferent(const BasePtr& other) const
{
    return !isEqual(other);
}

bool tsym::Base::has(const BasePtr& other) const
{
    if (isEqual(other))
        return true;
    else if (!ops.empty())
        return ctr::has(ops, other);
    else
        return false;
}

bool tsym::Base::isConst() const
{
    if (ops.empty())
        return false;
    else
        return ctr::areAllElementsConst(ops);
}

tsym::BasePtr tsym::Base::numericTerm() const
{
    return Numeric::one();
}

tsym::BasePtr tsym::Base::nonNumericTerm() const
{
    return clone();
}

tsym::BasePtr tsym::Base::constTerm() const
{
    return isConst() ? clone() : Numeric::one();
}

tsym::BasePtr tsym::Base::nonConstTerm() const
{
    return isConst() ? Numeric::one() : clone();
}

tsym::BasePtr tsym::Base::expand() const
{
    return clone();
}

tsym::BasePtr tsym::Base::subst(const BasePtr& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return clone();
}

tsym::BasePtr tsym::Base::coeff(const BasePtr& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (!has(variable) && exp == 0)
        return clone();
    else
        return Numeric::zero();
}

tsym::BasePtr tsym::Base::leadingCoeff(const BasePtr& variable) const
{
    return coeff(variable, degree(variable));
}

int tsym::Base::degree(const BasePtr& variable) const
{
    if (isEqual(variable))
        return 1;
    else
        return 0;
}

tsym::BasePtr tsym::Base::base() const
{
    return clone();
}

tsym::BasePtr tsym::Base::exp() const
{
    return Numeric::one();
}

const tsym::Name& tsym::Base::name() const
{
    static const Name empty("");

    return empty;
}

tsym::BasePtr tsym::Base::clone() const
{
    return BasePtr(shared_from_this());
}

tsym::BasePtr tsym::Base::normal() const
{
    if (ops.empty())
        return normalWithoutCache();
    else
        return normalViaCache();
}

tsym::BasePtr tsym::Base::normalViaCache() const
{
    static cache::RegisteredCache<BasePtr, BasePtr> cache;
    static auto& map(cache.map);
    const BasePtr key = clone();
    const auto lookup = map.find(key);

    if (lookup != cend(map))
        return lookup->second;

    return map.insert({ std::move(key), normalWithoutCache() })->second;
}

tsym::BasePtr tsym::Base::normalWithoutCache() const
{
    Fraction normalizedFrac;
    SymbolMap map;

    normalizedFrac = normal(map);

    return Fraction(map.replaceTmpSymbolsBackFrom(normalizedFrac.num()),
            map.replaceTmpSymbolsBackFrom(normalizedFrac.denom())).eval();
}

tsym::BasePtr tsym::Base::diff(const BasePtr& symbol) const
{
    if (symbol->isSymbol())
        return diffWrtSymbol(symbol);

    TSYM_WARNING("Differentiation w.r.t. %s! Only Symbols work, return Undefined.",
            typeStr().c_str());

    return Undefined::create();
}

const tsym::BasePtrCtr& tsym::Base::operands() const
{
    return ops;
}

bool tsym::Base::isEqualByTypeAndOperands(const BasePtr& other) const
{
    if (sameType(other))
        return ctr::areEqual(ops, other->ops);
    else
        return false;
}

void tsym::Base::setDebugString()
{
#ifdef TSYM_WITH_DEBUG_STRINGS
    std::ostringstream stream;
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);

    printer::printDebug(engine, *this);

    prettyStr = stream.str();
#endif
}
