
#include <cassert>
#include "productsimpl.h"
#include "product.h"
#include "numeric.h"
#include "power.h"
#include "sum.h"
#include "order.h"
#include "symbol.h"
#include "numpowersimpl.h"
#include "logging.h"

namespace tsym {
    namespace {
        bool isFraction(const BasePtr& arg)
        {
            return arg->isNumeric() && arg->numericEval().isFrac();
        }

        bool isInteger(const BasePtr& arg)
        {
            return arg->isNumeric() && arg->numericEval().isInt();
        }
    }
}

tsym::BasePtrList tsym::ProductSimpl::simplify(const BasePtrList& origFactors)
{
    BasePtrList factors(origFactors);

    prepare(factors);

    if (factors.size() == 2)
        return simplTwoFactors(factors);
    else
        return simplNFactors(factors);
}

void tsym::ProductSimpl::prepare(BasePtrList& fac)
    /* We need to extract the factors of included powers at this point, due to the handling of
     * numeric powers. This differs from Cohen's algorithm. */
{
    extractProducts(fac);
    contractTrigonometrics(fac);
}

void tsym::ProductSimpl::extractProducts(BasePtrList& u)
{
    auto it(u.begin());

    while (it != u.end()) {
        if ((*it)->isProduct()) {
            u.insert(it, (*it)->operands().begin(), (*it)->operands().end());
            it = u.erase(it);
        } else
            ++it;
    }
}

void tsym::ProductSimpl::contractTrigonometrics(BasePtrList& u)
{
    contract(u, &ProductSimpl::areContractableTrigFctPowers, &ProductSimpl::contractTrigFctPowers);
}

void tsym::ProductSimpl::contract(BasePtrList& u,
        bool (ProductSimpl::*check) (const BasePtr& f1, const BasePtr& f2),
        BasePtrList (ProductSimpl::*simpl)(const BasePtr& f1, const BasePtr& f2))
    /* This method is somewhat compliated, as it operates on the given list, possibly modifying it,
     * while checking for possible contraction of two list items. If that is the case, they are
     * simplified with the result being a BasePtrList with one or two items, that have to be
     * inserted (only if it differs from the input). The two original items must be erased from the
     * list. This method calls itself recursively, whenever a change to the given BasePtrList u was
     * made, to ensure correct simplification of every possible combination of factors. */
{
    bool hasChanged = false;
    auto it1(u.begin());
    decltype(it1) it2;
    BasePtrList res;
    bool found;

    while (it1 != u.end()) {
        for (found = false, it2 = it1, ++it2; it2 != u.end(); ++it2)
            if ((this->*(check))(*it1, *it2)) {
                res = (this->*(simpl))(*it1, *it2);

                if (res.size() == 2 && res.front()->isEqual(*it1) && res.back()->isEqual(*it2))
                    continue;

                it1 = u.erase(it1);
                u.insert(it1, res.begin(), res.end());
                std::advance(it1, -static_cast<long>(res.size()));
                u.erase(it2);
                it2 = it1;

                hasChanged = found = true;
            }

        if (!found)
            ++it1;
    }

    if (hasChanged)
        contract(u, check, simpl);
}

bool tsym::ProductSimpl::isContractableTrigFctPower(const BasePtr& pow)
{
    static Name trigoNames[3] = { Name("sin"), Name("cos"), Name("tan") };
    const Name& name(pow->base()->name());

    if (pow->base()->isFunction() && pow->exp()->isNumericallyEvaluable())
        return name == trigoNames[0] || name == trigoNames[1] || name == trigoNames[2];

    return false;
}

tsym::BasePtrList tsym::ProductSimpl::contractTrigFctPowers(const BasePtr& f1, const BasePtr& f2)
    /* At this point, f1 and f2 are (possibly powers of) trigonometric function with identical
     * arguments. */
{
    const BasePtr newArg(f1->base()->operands().front());
    const BasePtr sin(trigSymbReplacement(Trigonometric::Type::SIN, newArg));
    const BasePtr cos(trigSymbReplacement(Trigonometric::Type::COS, newArg));
    BasePtr r1(trigFunctionPowerReplacement(f1, sin, cos));
    BasePtr r2(trigFunctionPowerReplacement(f2, sin, cos));
    BasePtrList res;
    BasePtr newExp;
    BasePtr exp1;
    BasePtr exp2;

    res = simplTwoFactors(r1, r2);

    exp1 = res.front()->exp();
    exp2 = res.back()->exp();

    if (res.size() == 2 && exp1->isEqual(Product::minus(exp2))) {
        /* A combination of sin and cos should lead to tan or 1/tan. */
        if (res.front()->base()->isEqual(cos))
            newExp = Product::minus(exp1);
        else
            newExp = exp1;

        return BasePtrList(Power::create(Trigonometric::createTan(newArg), newExp));
    }

    res = res.subst(sin, Trigonometric::createSin(newArg)).subst(cos,
            Trigonometric::createCos(newArg));

    if (order::doPermute(res.front(), res.back()))
        return BasePtrList(res.back(), res.front());
    else
        return res;
}

tsym::BasePtr tsym::ProductSimpl::trigSymbReplacement(Trigonometric::Type type, const BasePtr& arg)
{
    const BasePtr signTest(type == Trigonometric::Type::SIN ? Trigonometric::createSin(arg) :
            Trigonometric::createCos(arg));
    const bool isPositive = signTest->isPositive();

    assert(type == Trigonometric::Type::SIN || type == Trigonometric::Type::COS);

    return Symbol::createTmpSymbol(isPositive);
}

tsym::BasePtr tsym::ProductSimpl::trigFunctionPowerReplacement(const BasePtr& pow,
        const BasePtr& sin, const BasePtr& cos)
{
    if (pow->base()->name() == Name("sin"))
        return Power::create(sin, pow->exp());
    else if (pow->base()->name() == Name("cos"))
        return Power::create(cos, pow->exp());

    assert(pow->base()->name() == Name("tan"));

    return Power::create(Product::create(sin, Power::oneOver(cos)), pow->exp());
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoFactors(const BasePtrList& u)
{
    BasePtr f1(*u.begin());
    BasePtr f2(*(++u.begin()));

    return simplTwoFactors(f1, f2);
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoFactors(const BasePtr& f1, const BasePtr& f2)
{
    if (f1->isProduct() || f2->isProduct())
        return simplTwoFactorsWithProduct(f1, f2);
    else
        return simplTwoFactorsWithoutProduct(f1, f2);
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoFactorsWithProduct(const BasePtr& f1,
        const BasePtr& f2)
{
    BasePtrList l1 = f1->isProduct() ? f1->operands() : BasePtrList(f1);
    BasePtrList l2 = f2->isProduct() ? f2->operands() : BasePtrList(f2);

    return merge(l1, l2);
}

tsym::BasePtrList tsym::ProductSimpl::merge(const BasePtrList& l1, const BasePtrList& l2)
{
    if (l1.empty())
        return l2;
    else if (l2.empty())
        return l1;
    else
        return mergeNonEmpty(l1, l2);
}

tsym::BasePtrList tsym::ProductSimpl::mergeNonEmpty(const BasePtrList& p, const BasePtrList& q)
{
    const BasePtr p1(p.front());
    const BasePtr q1(q.front());
    const BasePtrList p1q1(p1, q1);
    const BasePtrList q1p1(q1, p1);
    BasePtrList pRest(p.rest());
    BasePtrList qRest(q.rest());
    BasePtrList res;

    res = simplTwoFactors(p1, q1);

    if (res.empty())
        return merge(pRest, qRest);
    else if (res.size() == 1 && res.front()->isOne())
        return BasePtrList(merge(pRest, qRest));
    else if (res.size() == 1)
        return BasePtrList(res, merge(pRest, qRest));
    else if (res.isEqual(p1q1))
        return BasePtrList(p1, merge(pRest, q));
    else if (res.isEqual(q1p1))
        return BasePtrList(q1, merge(p, qRest));

    TSYM_ERROR("ProductSimpl: Error merging %S and %S to %S", p1, q1, res);

    return BasePtrList();
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoFactorsWithoutProduct(const BasePtr& f1,
        const BasePtr& f2)
{
    if (f1->isOne())
        return BasePtrList(f2);
    else if (f2->isOne())
        return BasePtrList(f1);
    else if (f1->isConst() && f2->isConst())
        /* Here, we differ from Cohen's algorithm, as numerics and constant powers (sqrt(2) etc.)are
         * handled together and somewhat similarly. */
        return simplTwoConst(f1, f2);
    else if (haveEqualBases(f1, f2))
        return simplTwoEqualBases(f1, f2);
    else if (order::doPermute(f1, f2))
        return BasePtrList(f2, f1);
    else
        return BasePtrList(f1, f2);
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoConst(const BasePtr& f1, const BasePtr& f2)
{
    if (f1->isNumeric() && f2->isNumeric())
        return simplTwoNumerics(f1, f2);
    else if (f1->isNumeric())
        /* The product of a numeric and a numeric power is treated in a special way: 2*sqrt(2) is
         * not evaluated to 2^(3/2), but stays a product. If f2 is a constant sum, its expansion is
         * handled later on. */
        return simplNumAndConst(f1, f2);
    else if (f2->isNumeric())
        return simplNumAndConst(f2, f1);
    else if (haveEqualBases(f1, f2))
        /* 2*sqrt(2) has been handled earlier, so this will catch terms like 2^(1/3)*2(1/4). */
        return simplTwoEqualBases(f1, f2);
    else if (areNumPowersWithEqualExp(f1, f2))
        /* ... while this is for sqrt(2)*sqrt(3) = sqrt(6). */
        return simplTwoEqualExp(f1, f2);
    else if (areNumPowersWithZeroSumExp(f1, f2))
        /* ... and this is for 2^(1/4)*3^(-1/4) = (2/3)^(1/4). */
        return simplTwoZeroSumExp(f1, f2);
    else if (areNumPowersWithEqualExpDenom(f1, f2))
        return simplTwoEqualExpDenom(f1, f2);
    else if (order::doPermute(f1, f2))
        /* This has to be checked additionaly for e.g. (1 + sqrt(2))*sqrt(3), which doesn't need
         * simplification at this point, but should be perturbed in its order. */
        return BasePtrList(f2, f1);

    return BasePtrList(f1, f2);
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoNumerics(const BasePtr& f1, const BasePtr& f2)
{
    const Number n1(f1->numericEval());
    const Number n2(f2->numericEval());
    const Number res(n1*n2);

    if (res.isOne())
        return BasePtrList();
    else
        return BasePtrList(Numeric::create(res));
}

tsym::BasePtrList tsym::ProductSimpl::simplNumAndConst(const BasePtr& numeric,
        const BasePtr& constant)
{
    /* At this point, the second parameter should either be a sum, a numeric power or a constant
     * power. */
    assert(!constant->isConstant());

    if (constant->isSum())
        return BasePtrList(numeric, constant);
    else if (constant->isNumericPower())
        return simplNumAndNumPow(numeric, constant);
    else if (constant->isPower())
        return BasePtrList(numeric, constant);

    TSYM_ERROR("Wrong type during ProductSimpl of two const. expressions!"
            "Got %S as Numeric and %S as const.!", numeric, constant);

    return BasePtrList(numeric, constant);
}

tsym::BasePtrList tsym::ProductSimpl::simplNumAndNumPow(const BasePtr& numeric,
        const BasePtr& numPow)
{
    const Number base(numPow->base()->numericEval());
    const Number exp(numPow->exp()->numericEval());
    const Number preFactor(numeric->numericEval());

    return simplNumAndNumPow(preFactor, base, exp);
}

tsym::BasePtrList tsym::ProductSimpl::simplNumAndNumPow(const Number& preFactor, const Number& base,
        const Number& exp)
{
    NumPowerSimpl numericPow;
    BasePtr newBase;
    BasePtr newExp;
    BasePtr preFac;

    numericPow.setPower(base, exp);
    numericPow.setPreFac(preFactor);

    newBase = Numeric::create(numericPow.getNewBase());
    newExp = Numeric::create(numericPow.getNewExp());
    preFac = Numeric::create(numericPow.getPreFactor());

    if (preFac->isOne())
        return BasePtrList(Power::create(newBase, newExp));
    else
        return BasePtrList(preFac, Power::create(newBase, newExp));
}

bool tsym::ProductSimpl::haveEqualBases(const BasePtr& f1, const BasePtr& f2)
{
    return f1->base()->isEqual(f2->base());
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoEqualBases(const BasePtr& f1, const BasePtr& f2)
{
    const BasePtr newBase(f1->base());
    const BasePtr e1(f1->exp());
    const BasePtr e2(f2->exp());
    const BasePtr newExp(Sum::create(e1, e2));

    if (newBase->isPositive() || newBase->isNegative())
        /* If base < 0, both exponents can't be part of an undefined power expressions (fraction or
         * double exponent), thus the addition of exponents must be valid, too. */
        ;
    else if (isFraction(e1) && isFraction(e2) && isInteger(newExp))
        /* Exponent addition would hide the fact that the power factors could be undefined. */
        return BasePtrList(f1, f2);

    return BasePtrList(Power::create(newBase, newExp));
}

bool tsym::ProductSimpl::areContractableTrigFctPowers(const BasePtr& f1, const BasePtr& f2)
    /* Returns true for Powers with numerically evaluable exponents and bases that are trigonometric
     * functions of the same argument. */
{
    if (isContractableTrigFctPower(f1) && isContractableTrigFctPower(f2))
        return f1->base()->operands().front()->isEqual(f2->base()->operands().front());
    else
        return false;
}

bool tsym::ProductSimpl::areNumPowersWithEqualExp(const BasePtr& f1, const BasePtr& f2)
{
    BasePtr exp1;
    BasePtr exp2;

    if (!f1->isNumericPower())
        return false;
    else if (!f2->isNumericPower())
        return false;

    exp1 = f1->exp();
    exp2 = f2->exp();

    return exp1->isEqual(exp2);
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoEqualExp(const BasePtr& f1, const BasePtr& f2)
{
    const Number base1(f1->base()->numericEval());
    const Number base2(f2->base()->numericEval());
    const Number& exp(f1->exp()->numericEval());

    return simplNumAndNumPow(1, base1*base2, exp);
}

bool tsym::ProductSimpl::areNumPowersWithZeroSumExp(const BasePtr& f1, const BasePtr& f2)
{
    const BasePtr sum(Sum::create(f1->exp(), f2->exp()));

    return sum->isZero();
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoZeroSumExp(const BasePtr& f1, const BasePtr& f2)
{
    /* f1 and f2 are both numeric powers. */
    const Number base1(f1->base()->numericEval());
    const BasePtr exp1(f1->exp());
    Number base2(f2->base()->numericEval());

    base2 = base2.toThe(-1);

    /* No care must be taken for the sign of f1 and f2, the subsequent treatment will choose the
     * positive exponent. */
    return BasePtrList(Power::create(Numeric::create(base1*base2), exp1));
}

bool tsym::ProductSimpl::areNumPowersWithEqualExpDenom(const BasePtr& f1, const BasePtr& f2)
{
    if (f1->isNumericPower() && f2->isNumericPower())
        return f1->exp()->numericEval().denominator() == f2->exp()->numericEval().denominator();
    else
        return false;
}

tsym::BasePtrList tsym::ProductSimpl::simplTwoEqualExpDenom(const BasePtr& f1, const BasePtr& f2)
    /* This method has to manually perform an evaluation of integer exponentiation and
     * multiplication. */
{
    assert(f1->isNumericPower() && f2->isNumericPower());
    const BasePtr newExp(Numeric::create(1, f1->exp()->numericEval().denominator()));
    const Int& limit(NumPowerSimpl::getMaxPrimeResolution());
    const Int denom[] = { evalDenomExpNumerator(f1), evalDenomExpNumerator(f2) };
    const Int num[] = { evalNumExpNumerator(f1), evalNumExpNumerator(f2) };
    Number newBase;
    Int newDenom;
    Int newNum;

    newNum = num[0]*num[1];
    newDenom = denom[0]*denom[1];

    newBase = Number(newNum, newDenom);

    if (newBase.numerator() > limit || newBase.denominator() > limit)
        return BasePtrList(f1, f2);
    else
        return BasePtrList(Power::create(Numeric::create(newBase), newExp));
}

tsym::Int tsym::ProductSimpl::evalNumExpNumerator(const BasePtr& numPow)
{
    const Int exp(numPow->exp()->numericEval().numerator());
    const Number base(numPow->base()->numericEval());

    return evalExpNumerator(base, exp);
}

tsym::Int tsym::ProductSimpl::evalExpNumerator(const Number& base, const Int& exp)
{
    assert(integer::fitsInto<unsigned>(integer::abs(exp)));
    const Int& selectedBase = exp > 0 ? base.numerator() : base.denominator();
    const unsigned integralExp = static_cast<unsigned>(integer::abs(exp));

    return integer::pow(selectedBase, integralExp);
}

tsym::Int tsym::ProductSimpl::evalDenomExpNumerator(const BasePtr& numPow)
{
    const Number base(numPow->base()->numericEval().toThe(-1));
    const Int exp(numPow->exp()->numericEval().numerator());

    return evalExpNumerator(base, exp);
}

tsym::BasePtrList tsym::ProductSimpl::simplNFactors(BasePtrList u)
{
    prepareConst(u);

    return simplPreparedFactors(u);
}

void tsym::ProductSimpl::prepareConst(BasePtrList& u)
    /* Some elements of the factor list have to be preprocessed due to the handling of numeric
     * powers: as the contraction of two numeric powers may result in a product of an integer and a
     * different numeric power (e.g. sqrt(3)*sqrt(6) = 3*sqrt(2)), the usual ordering of
     * non-simplified terms wouldn't work properly, because only one operation per expression pair
     * is provided (in the example: it could be necessary to shift the integer 3 to the beginning of
     * the factor list to contract it with another integer). */
{
    u.sort(order::doPermute);
    contractNumerics(u);
    contractConst(u);

    contract(u, &ProductSimpl::areNumPowersWithEqualExp, &ProductSimpl::simplTwoEqualExp);
    contract(u, &ProductSimpl::areNumPowersWithEqualExpDenom, &ProductSimpl::simplTwoEqualExpDenom);
}

void tsym::ProductSimpl::contractNumerics(BasePtrList& u)
{
    auto it(u.begin());
    Number n(1);

    while (it != u.end())
        if ((*it)->isNumeric()) {
            n *= (*it)->numericEval();
            it = u.erase(it);
        } else
            ++it;

    if (!n.isOne() || u.empty())
        u.push_front(Numeric::create(n));
}

void tsym::ProductSimpl::contractConst(BasePtrList& u)
{
    for (auto it1 = u.begin(); it1 != u.end(); ++it1)
            for (auto it2 = it1; it2 != u.end(); ++it2)
                if (it1 == it2)
                    continue;
                else if (areTwoContractableConst(*it1, *it2))
                    contractTwoConst(it1, it2, u);
}

bool tsym::ProductSimpl::areTwoContractableConst(const BasePtr& f1, const BasePtr& f2)
{
    if (!f1->isConst())
        return false;
    else if (!f2->isConst())
        return false;
    else if (f1->isNumeric() || f1->isNumericPower())
        return f2->isNumeric() || f2->isNumericPower();
    else
        return false;
}

void tsym::ProductSimpl::contractTwoConst(BasePtrList::iterator& it1,
    BasePtrList::iterator& it2, BasePtrList& u)
{
    const BasePtrList res(simplTwoConst(*it1, *it2));

    if (res.size() == 1) {
        *it1 = res.front();
        it2 = u.erase(it2);
    } else if (res.size() == 2) {
        *it1 = res.front();
        *it2 = res.back();
    } else
        TSYM_ERROR("Error contracting %S and %S to %S", *it1, *it2, res);
}

tsym::BasePtrList tsym::ProductSimpl::simplPreparedFactors(const BasePtrList& u)
{
    if (u.size() == 1)
        return u;
    else if (u.size() == 2)
        return simplTwoFactors(u);
    else
        return simplNPreparedFactors(u);
}

tsym::BasePtrList tsym::ProductSimpl::simplNPreparedFactors(const BasePtrList& u)
{
    const BasePtrList uRest(u.rest());
    const BasePtr u1(u.front());
    BasePtrList simplRest;

    simplRest = simplify(uRest);

    /* Again, slightly different from Cohen's algorithm: u1 can't be a product, because products
     * components have been merged into the input BasePtrList at the very beginning. */
    return merge(BasePtrList(u1), simplRest);
}
