
#include <cmath>
#include <cassert>
#include "powersimpl.h"
#include "power.h"
#include "product.h"
#include "numeric.h"
#include "numpowersimpl.h"
#include "constant.h"
#include "logarithm.h"
#include "logging.h"

tsym::PowerSimpl::PowerSimpl() :
    one(Numeric::one())
{}

tsym::BasePtrList tsym::PowerSimpl::simplify(const BasePtr& base, const BasePtr& exp)
{
    if (base->isNumeric())
        return simplifyNumericBase(base, exp);
    else if (base->isPower())
        return simplifyPowerBase(base, exp);
    else if (base->isProduct())
        return simplifyProductBase(base, exp);
    else if (base->isUndefined())
        return BasePtrList(Undefined::create(), one);
    else if (base->isConstant())
        return simplifyConstantBase(base, exp);
    else
        /* No simplification applicable. */
        return BasePtrList(base, exp);
}

tsym::BasePtrList tsym::PowerSimpl::simplifyNumericBase(const BasePtr& base, const BasePtr& exp)
{
    if (exp->isNumeric())
        return simplifyNumericPower(base, exp);
    else
        return BasePtrList(base, exp);
}

tsym::BasePtrList tsym::PowerSimpl::simplifyNumericPower(const BasePtr& base,
        const BasePtr& exp)
{
    const Number nBase(base->numericEval());
    const Number nExp(exp->numericEval());

    return simplifyNumericPower(nBase, nExp);
}

tsym::BasePtrList tsym::PowerSimpl::simplifyNumericPower(const Number& base, const Number& exp)
{
    NumPowerSimpl numericPow;
    BasePtr newBase;
    BasePtr newExp;
    BasePtr preFac;

    numericPow.setPower(base, exp);

    preFac = Numeric::create(numericPow.getPreFactor());
    newBase = Numeric::create(numericPow.getNewBase());
    newExp = Numeric::create(numericPow.getNewExp());

    if (preFac->isOne())
        return BasePtrList(newBase, newExp);
    else
        return BasePtrList(Product::create(preFac, Power::create(newBase, newExp)), one);
}

tsym::BasePtrList tsym::PowerSimpl::simplifyPowerBase(const BasePtr& base, const BasePtr& exp)
    /* Simplification: ((base)^e1)^e2 = (base)^(e1*e2). Achieved by creating a new product (which
     * itself is automatically simplified) to be the new exponent. */
{
    const BasePtr newBase(base->base());
    BasePtr newExp;

    newExp = Product::create(base->exp(), exp);

    if (newExp->isZero())
        /* For very small numeric exponents, this could be the case. */
        return BasePtrList(one, one);

    return simplify(newBase, newExp);
}

tsym::BasePtrList tsym::PowerSimpl::simplifyProductBase(const BasePtr& base, const BasePtr& exp)
    /* Performs (a*b)^c = a^c*b^c. */
{
    BasePtrList::const_iterator it;
    BasePtrList components;

    for (it = base->operands().begin(); it != base->operands().end(); ++it)
        components.push_back(Power::create(*it, exp));

    return BasePtrList(Product::create(components), one);
}

tsym::BasePtrList tsym::PowerSimpl::simplifyConstantBase(const BasePtr& base, const BasePtr& exp)
{
    if (isBaseEulerConstantAndExpLogarithm(base, exp))
        return BasePtrList(exp->operands().front(), one);
    else
        return BasePtrList(base, exp);
}

bool tsym::PowerSimpl::isBaseEulerConstantAndExpLogarithm(const BasePtr& base, const BasePtr& exp)
{
    if (base->isEqual(Constant::createE()) && exp->isFunction())
        return exp->isEqual(Logarithm::create(exp->operands().front()));
    else
        return false;
}
