
#include <cassert>
#include <cmath>
#include <limits>
#include "power.h"
#include "numeric.h"
#include "powersimpl.h"
#include "logging.h"
#include "logarithm.h"
#include "product.h"
#include "sum.h"
#include "powernormal.h"

tsym::Power::Power(const BasePtr& base, const BasePtr& exponent) :
    Base(BasePtrList(base, exponent)),
    baseRef(ops.front()),
    expRef(ops.back())
{
    assert(ops.size() == 2);
}

tsym::Power::~Power() {}

tsym::BasePtr tsym::Power::create(const BasePtr& base, const BasePtr& exponent)
{
    if (exponent->isUndefined() || base->isUndefined())
        return Undefined::create();

    return createNotUndefined(base, exponent);
}

tsym::BasePtr tsym::Power::oneOver(const BasePtr& base)
{
    return create(base, Numeric::mOne());
}

tsym::BasePtr tsym::Power::sqrt(const BasePtr& base)
{
    return create(base, Numeric::create(1, 2));
}

tsym::BasePtr tsym::Power::createNotUndefined(const BasePtr& base, const BasePtr& exponent)
{
    /* Handle trivial cases first. */
    if (exponent->isZero() || base->isOne())
        return Numeric::one();
    else if (base->isZero() && exponent->isNegative()) {
        logging::warning() << "Division by zero during Power creation!";
        return Undefined::create();
    } else if (base->isZero())
        return Numeric::zero();
    else if (exponent->isOne())
        return base;

    return createNonTrivial(base, exponent);
}

tsym::BasePtr tsym::Power::createNonTrivial(const BasePtr& base, const BasePtr& exponent)
{
    PowerSimpl simpl;
    BasePtrList res;

    res = simpl.simplify(base, exponent);

    if (res.size() != 2) {
        logging::error() << "Obtained wrong list from PowerSimpl: " << res << ". Return Undefined.";
        return Undefined::create();
    }

    /* Handle trivial cases again. */
    if (res.back()->isOne())
        return res.front();
    else if (res.front()->isOne())
        /* Will probably never be the case, just a security check. */
        return Numeric::one();

    return BasePtr(new Power(res.front(), res.back()));
}

bool tsym::Power::isEqual(const BasePtr& other) const
{
    return isEqualByTypeAndOperands(other);
}

bool tsym::Power::sameType(const BasePtr& other) const
{
    return other->isPower();
}

tsym::Number tsym::Power::numericEval() const
    /* If base or exponent can't be numerically evaluated, they will return an undefined Number and
     * the result will be undefined, too. */
{
    Number nExp;
    Number res;

    nExp = expRef->numericEval();
    res = baseRef->numericEval();
    res = res.toThe(nExp);

    return res;
}

tsym::Fraction tsym::Power::normal(SymbolMap& map) const
{
    PowerNormal pn(map);

    pn.setBase(baseRef);
    pn.setExponent(expRef);

    return pn.normal();
}

tsym::BasePtr tsym::Power::diffWrtSymbol(const BasePtr& symbol) const
{
    BasePtrList summands;

    summands.push_back(Product::create(Logarithm::create(baseRef), expRef->diffWrtSymbol(symbol)));
    summands.push_back(Product::create(expRef, oneOver(baseRef), baseRef->diffWrtSymbol(symbol)));

    return Product::create(clone(), Sum::create(summands));
}

std::string tsym::Power::typeStr() const
{
    return "Power";
}

bool tsym::Power::isPositive() const
{
    if (baseRef->isPositive())
        return true;
    else if (isExponentRationalNumeric())
        return expRef->numericEval().numerator() % 2 == 0;
    else
        return false;
}

bool tsym::Power::isNegative() const
{
    /* Currently, negative powers are always resolved as e.g. (-a)^(1/3) = (-1)*a^(1/3) for product
     * bases or (-2)^(1/3) = (-1)*2^(1/3) for numeric powers. */
    return false;
}

bool tsym::Power::isExponentRationalNumeric() const
{
    return expRef->isNumericallyEvaluable() && expRef->numericEval().isRational();
}

bool tsym::Power::isPower() const
{
    return true;
}

bool tsym::Power::isNumericPower() const
{
    return baseRef->isNumeric() && expRef->isNumeric();
}

tsym::BasePtr tsym::Power::expand() const
{
    if (isInteger(expRef))
        return expandIntegerExponent();
    else
        return clone();
}

bool tsym::Power::isInteger(const BasePtr& ptr) const
{
    return ptr->isNumeric() && ptr->numericEval().isInt();
}

tsym::BasePtr tsym::Power::expandIntegerExponent() const
{
    if (baseRef->isSum())
        return expandSumBaseIntExp();
    else if (baseRef->isProduct())
        /* Should have been resolved during standard product simplification. */
        logging::error() << "Illegal power expression, base: " << baseRef << ", exp.: " << expRef;

    return Power::create(baseRef, expRef);
}

tsym::BasePtr tsym::Power::expandSumBaseIntExp() const
{
    const Int nExp(expRef->numericEval().numerator());
    BasePtrList sums;
    BasePtr res;

    for (Int i(0); i < nExp.abs(); ++i)
        sums.push_back(baseRef);

    res = sums.expandAsProduct();

    if (nExp < 0)
        res = Power::oneOver(res);

    return res;
}

tsym::BasePtr tsym::Power::subst(const BasePtr& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else
        return create(baseRef->subst(from, to), expRef->subst(from, to));
}

tsym::BasePtr tsym::Power::coeff(const BasePtr& variable, int exp) const
{
    if (isEqual(variable))
        return exp == 1 ? Numeric::one() : Numeric::zero();
    else if (isEqual(create(variable, Numeric::create(exp))))
        /* We won't get here for an exp < 2, otherwise *this wasn't a Power. */
        return Numeric::one();
    else if (!baseRef->isEqual(variable))
        return exp == 0 ? clone() : Numeric::zero();
    else
        return Numeric::zero();
}

int tsym::Power::degree(const BasePtr& variable) const
{
    int baseDegree;
    Int nExp;

    if (isEqual(variable))
        return 1;
    else if (isInteger(expRef))
        nExp = expRef->numericEval().numerator();

    baseDegree = baseRef->degree(variable);

    if (nExp.fitsIntoInt() && nExp.toDouble() < std::numeric_limits<int>::max()/(double)baseDegree
            && nExp.toDouble() > std::numeric_limits<int>::min()/(double)baseDegree)
        return nExp.toInt()*baseDegree;

    logging::error() << "Degree of " << clone() <<
        " doens't fit into a primitive integer! Return 0 as degree.";

    return 0;
}

tsym::BasePtr tsym::Power::base() const
{
    return baseRef;
}

tsym::BasePtr tsym::Power::exp() const
{
    return expRef;
}
