
#include "base.h"
#include <sstream>
#include <utility>
#include "baseptr.h"
#include "bplist.h"
#include "cache.h"
#include "fraction.h"
#include "logging.h"
#include "name.h"
#include "numeric.h"
#include "plaintextprintengine.h"
#include "printer.h"
#include "product.h"
#include "symbolmap.h"
#include "undefined.h"

tsym::Base::Base(BasePtrList operands)
    : ops(std::move(operands))
{}

bool tsym::Base::isZero() const
{
    return false;
}

bool tsym::Base::isOne() const
{
    return false;
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

bool tsym::Base::isEqual(const Base& other) const
{
    return this == &other || isEqualDifferentBase(other);
}

bool tsym::Base::isDifferent(const Base& other) const
{
    return !isEqual(other);
}

bool tsym::Base::has(const Base& other) const
{
    if (isEqual(other))
        return true;
    else if (!ops.empty())
        return bplist::has(ops, other);
    else
        return false;
}

bool tsym::Base::isConst() const
{
    if (ops.empty())
        return false;
    else
        return bplist::areAllElementsConst(ops);
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

tsym::BasePtr tsym::Base::subst(const Base& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return clone();
}

tsym::BasePtr tsym::Base::coeff(const Base& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (!has(variable) && exp == 0)
        return clone();
    else
        return Numeric::zero();
}

tsym::BasePtr tsym::Base::leadingCoeff(const Base& variable) const
{
    return coeff(variable, degree(variable));
}

int tsym::Base::degree(const Base& variable) const
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
    return shared_from_this();
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

    return map.insert({key, normalWithoutCache()})->second;
}

tsym::BasePtr tsym::Base::normalWithoutCache() const
{
    Fraction normalizedFrac;
    SymbolMap map;

    normalizedFrac = normal(map);

    return eval(
      {map.replaceTmpSymbolsBackFrom(normalizedFrac.num), map.replaceTmpSymbolsBackFrom(normalizedFrac.denom)});
}

tsym::BasePtr tsym::Base::diff(const Base& symbol) const
{
    if (symbol.isSymbol())
        return diffWrtSymbol(symbol);

    TSYM_WARNING("Differentiation w.r.t. %s! Only Symbols work, return Undefined.", typeStr().c_str());

    return Undefined::create();
}

const tsym::BasePtrList& tsym::Base::operands() const
{
    return ops;
}

bool tsym::Base::isEqualByTypeAndOperands(const Base& other) const
{
    if (sameType(other))
        return bplist::areEqual(ops, other.ops);
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

std::ostream& tsym::operator<<(std::ostream& stream, const Base& arg)
{
    auto engine = PlaintextPrintEngine{stream};

    printer::print(engine, arg);

    return stream;
}
