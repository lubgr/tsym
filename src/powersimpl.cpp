
#include "powersimpl.h"
#include <cassert>
#include <cmath>
#include "basefct.h"
#include "constant.h"
#include "logarithm.h"
#include "logging.h"
#include "number.h"
#include "numberfct.h"
#include "numeric.h"
#include "numpowersimpl.h"
#include "power.h"
#include "product.h"
#include "undefined.h"

namespace tsym {
    namespace {
        BasePtrList simplifyNumericBase(const BasePtr& base, const BasePtr& exp);
        BasePtrList simplifyNumericPower(const BasePtr& base, const BasePtr& exp);
        BasePtrList simplifyNumericPower(const Number& base, const Number& exp);
        const BasePtr& one();
        BasePtrList simplifyPowerBase(const BasePtr& powBase, const BasePtr& e2);
        bool doContractExpFirst(const BasePtr& base, const BasePtr& e1, const BasePtr& e2);
        bool areTwoFractionExpWithOddDenom(const BasePtr& exp1, const BasePtr& exp2);
        bool doesChangeSign(const BasePtr& base, const BasePtr& exp1);
        bool doContractExpSecond(const BasePtr& e1, const BasePtr& e2);
        bool doContractExpSecond(const Number& e1, const Number& e2);
        BasePtrList simplifyProductBase(const BasePtr& base, const BasePtr& exp);
        BasePtrList simplifyConstantBase(const BasePtr& base, const BasePtr& exp);
        bool isBaseEulerConstantAndExpLogarithm(const BasePtr& base, const BasePtr& exp);

        bool doesInvolveComplexNumbers(const BasePtr& base, const BasePtr& exp)
        {
            if (const auto nExp = exp->numericEval(); base->isNegative() && nExp)
                return !isInt(*nExp);
            else
                return false;
        }

        BasePtrList simplifyNumericBase(const BasePtr& base, const BasePtr& exp)
        {
            if (exp->isNumeric())
                return simplifyNumericPower(base, exp);
            else
                return {base, exp};
        }

        BasePtrList simplifyNumericPower(const BasePtr& base, const BasePtr& exp)
        {
            const auto nBase(base->numericEval());
            const auto nExp(exp->numericEval());

            assert(nBase && nExp);

            return simplifyNumericPower(*nBase, *nExp);
        }

        BasePtrList simplifyNumericPower(const Number& base, const Number& exp)
        {
            NumPowerSimpl numericPow;
            BasePtr newBase;
            BasePtr newExp;
            BasePtr preFac;

            numericPow.setPower(base, exp);

            preFac = Numeric::create(numericPow.getPreFactor());
            newBase = Numeric::create(numericPow.getNewBase());
            newExp = Numeric::create(numericPow.getNewExp());

            if (isOne(*preFac))
                return {newBase, newExp};
            else
                return {Product::create(preFac, Power::create(newBase, newExp)), one()};
        }

        const BasePtr& one()
        {
            return Numeric::one();
        }

        BasePtrList simplifyPowerBase(const BasePtr& powBase, const BasePtr& e2)
        /* Performs ((base)^e1)^e2 = (base)^(e1*e2) if it's possible. */
        {
            const BasePtr e1(powBase->exp());
            const BasePtr base(powBase->base());
            BasePtr newBase;
            BasePtr newExp;

            if (doContractExpFirst(base, e1, e2))
                newBase = base;
            else if (doesChangeSign(base, e1))
                /* This enables ((-a - b)^2)^c = (a + b)^c with a, b > 0. */
                newBase = Product::minus(base);
            else if (doContractExpSecond(e1, e2))
                newBase = base;
            else
                /* No simplification possible. */
                return {powBase, e2};

            newExp = Product::create(e1, e2);

            if (isZero(*newExp))
                /* For very small numeric exponents, this could be the case. */
                return {one(), one()};

            return powersimpl::simplify(newBase, newExp);
        }

        bool doContractExpFirst(const BasePtr& base, const BasePtr& e1, const BasePtr& e2)
        {
            if (isInteger(*e1) && isInteger(*e2))
                return true;
            else if (areTwoFractionExpWithOddDenom(e1, e2))
                return true;
            else if (base->isPositive())
                return true;
            else
                return false;
        }

        bool areTwoFractionExpWithOddDenom(const BasePtr& exp1, const BasePtr& exp2)
        {
            if (!exp1->isNumeric() || !exp2->isNumeric())
                return false;

            const auto nExp1 = exp1->numericEval();
            const auto nExp2 = exp2->numericEval();

            if (isFraction(*nExp1) && isFraction(*nExp2))
                return nExp1->denominator() % 2 != 0 && nExp2->denominator() % 2 != 0;

            return false;
        }

        bool doesChangeSign(const BasePtr& base, const BasePtr& exp1)
        {
            if (!base->isNegative())
                return false;
            else if (!exp1->isNumeric())
                return false;

            return isEvenInt(*exp1->numericEval());
        }

        bool doContractExpSecond(const BasePtr& e1, const BasePtr& e2)
        {
            const auto e1Num = e1->numericEval();
            const auto e2Num = e2->numericEval();

            if (e1Num && e2Num)
                return doContractExpSecond(*e1Num, *e2Num);

            return false;
        }

        bool doContractExpSecond(const Number& e1, const Number& e2)
        {
            const Number newExp = e1 * e2;

            if (isOddInt(e1))
                return abs(newExp) != 1;
            else if (isEvenInt(e1))
                return isEvenInt(newExp);
            else if (isFraction(e1) && (isInt(e2) || isFraction(e2)))
                return isFraction(newExp);

            return !isEvenInt(e1);
        }

        BasePtrList simplifyProductBase(const BasePtr& base, const BasePtr& exp)
        /* Performs (a*b)^c = a^c*b^c if possible. */
        {
            const bool doExpandAll = isInteger(*exp);
            BasePtrList simplified;
            BasePtrList keep;

            for (const auto& factor : base->operands())
                if (doExpandAll || factor->isPositive())
                    simplified.push_back(Power::create(factor, exp));
                else
                    keep.push_back(factor);

            if (simplified.empty())
                return {base, exp};

            simplified.push_back(Power::create(Product::create(keep), exp));

            return {Product::create(simplified), one()};
        }

        BasePtrList simplifyConstantBase(const BasePtr& base, const BasePtr& exp)
        {
            if (isBaseEulerConstantAndExpLogarithm(base, exp))
                return {exp->operands().front(), one()};
            else
                return {base, exp};
        }

        bool isBaseEulerConstantAndExpLogarithm(const BasePtr& base, const BasePtr& exp)
        {
            if (base->isEqual(*Constant::createE()) && exp->isFunction())
                return exp->isEqual(*Logarithm::create(exp->operands().front()));
            else
                return false;
        }
    }
}

tsym::BasePtrList tsym::powersimpl::simplify(const BasePtr& base, const BasePtr& exp)
{
    if (doesInvolveComplexNumbers(base, exp))
        return {Undefined::create(), one()};
    else if (base->isNumeric())
        return simplifyNumericBase(base, exp);
    else if (base->isPower())
        return simplifyPowerBase(base, exp);
    else if (base->isProduct())
        return simplifyProductBase(base, exp);
    else if (base->isUndefined())
        return {Undefined::create(), one()};
    else if (base->isConstant())
        return simplifyConstantBase(base, exp);
    else
        /* No simplification applicable. */
        return {base, exp};
}
