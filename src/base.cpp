
#include "base.h"
#include "baseptr.h"
#include "baseptrlist.h"
#include "numeric.h"
#include "symbolmap.h"
#include "product.h"
#include "cache.h"
#include "printer.h"
#include "logging.h"

tsym::Base::Base()
{}

tsym::Base::Base(const BasePtrList& operands) :
    ops(operands)
{}

tsym::Base::~Base() {}

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
        return ops.areElementsNumericallyEvaluable();
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
        return ops.has(other);
    else
        return false;
}

bool tsym::Base::isConst() const
{
    if (ops.empty())
        return false;
    else
        return ops.areAllElementsConst();
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
    // return BasePtr(this);
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
    static Cache<BasePtr, BasePtr> cache;
    const BasePtr *cached(cache.retrieve(clone()));
    BasePtr result;

    if (cached != nullptr)
        return *cached;

    result = normalWithoutCache();

    cache.insertAndReturn(clone(), result);

    return cache.insertAndReturn(result, result);
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

const tsym::BasePtrList& tsym::Base::operands() const
{
    return ops;
}

bool tsym::Base::isEqualByTypeAndOperands(const BasePtr& other) const
{
    if (sameType(other))
        return ops.isEqual(other->ops);
    else
        return false;
}

void tsym::Base::setDebugString()
{
#ifdef TSYM_DEBUG_STRINGS
    /* When this class is instantiated, the reference count is zero. Construction and
     * destruction of a BasePtr will then delete the object, to circumvent this, we artificially
     * increment the reference count temporarily here. */
    prettyStr = Printer(BasePtr(this)).getStr();
#endif
}
