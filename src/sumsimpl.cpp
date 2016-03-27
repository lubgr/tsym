
#include <cassert>
#include "sumsimpl.h"
#include "sum.h"
#include "product.h"
#include "order.h"
#include "numeric.h"
#include "logging.h"

tsym::BasePtrList tsym::SumSimpl::simplify(const BasePtrList& summands)
{
    if (summands.size() == 2)
        return simplTwoSummands(summands);
    else
        return simplNSummands(summands);
}

tsym::BasePtrList tsym::SumSimpl::simplTwoSummands(const BasePtrList& u)
{
    BasePtr s1(*u.begin());
    BasePtr s2(*(++u.begin()));

    return simplTwoSummands(s1, s2);
}

tsym::BasePtrList tsym::SumSimpl::simplTwoSummands(const BasePtr& s1, const BasePtr& s2)
{
    if (s1->isSum() || s2->isSum())
        return simplTwoSummandsWithSum(s1, s2);
    else
        return simplTwoSummandsWithoutSum(s1, s2);
}

tsym::BasePtrList tsym::SumSimpl::simplTwoSummandsWithSum(const BasePtr& s1, const BasePtr& s2)
{
    BasePtrList l1 = s1->isSum() ? s1->operands() : BasePtrList(s1);
    BasePtrList l2 = s2->isSum() ? s2->operands() : BasePtrList(s2);

    return merge(l1, l2);
}

tsym::BasePtrList tsym::SumSimpl::merge(const BasePtrList& l1, const BasePtrList& l2)
{
    if (l1.empty())
        return l2;
    else if (l2.empty())
        return l1;
    else
        return mergeNonEmpty(l1, l2);
}

tsym::BasePtrList tsym::SumSimpl::mergeNonEmpty(const BasePtrList& p, const BasePtrList& q)
{
    const BasePtr p1(p.front());
    const BasePtr q1(q.front());
    const BasePtrList p1q1(p1, q1);
    const BasePtrList q1p1(q1, p1);
    BasePtrList pRest(p.rest());
    BasePtrList qRest(q.rest());
    BasePtrList res;

    res = simplTwoSummands(p1, q1);

    if (res.empty())
        return merge(pRest, qRest);
    else if (res.size() == 1 && res.front()->isZero())
        return BasePtrList(merge(pRest, qRest));
    else if (res.size() == 1)
        return BasePtrList(res, merge(pRest, qRest));
    else if (res.isEqual(p1q1))
        return BasePtrList(p1, merge(pRest, q));
    else if (res.isEqual(q1p1))
        return BasePtrList(q1, merge(p, qRest));

    logging::error() << "Error merging non-empty lists: " << p << " " << q << "!";

    return BasePtrList();
}

tsym::BasePtrList tsym::SumSimpl::simplTwoSummandsWithoutSum(const BasePtr& s1, const BasePtr& s2)
{
    if (s1->isZero())
        return BasePtrList(s2);
    else if (s2->isZero())
        return BasePtrList(s1);
    else if (s1->isNumeric() && s2->isNumeric())
        return simplTwoNumerics(s1, s2);
    else if (haveEqualNonConstTerms(s1, s2))
        /* Catches const. terms as prefactors, e.g. sqrt(3)*a + 2*a = (2 + sqrt(3))*a. Constants are
         * treated as symbols here. */
        return simplEqualNonConstTerms(s1, s2);
    else if (haveEqualNonNumericTerms(s1, s2))
        /* Num. powers aren't prefactors now, for e.g. 2*sqrt(2) + sqrt(2) = 3*sqrt(2). Constants
         * still play the same role as symbols. */
        return simplEqualNonNumericTerms(s1, s2);
    else if (haveContractableSinCos(s1, s2))
        return BasePtrList(s1->constTerm());
    else if (order::doPermute(s1, s2))
        return BasePtrList(s2, s1);
    else
        return BasePtrList(s1, s2);
}

tsym::BasePtrList tsym::SumSimpl::simplTwoNumerics(const BasePtr& s1, const BasePtr& s2)
{
    const Number n1(s1->numericEval());
    const Number n2(s2->numericEval());
    /* An integer overflow may happen here for large values of n1 and n2. The number class handles
     * this by converting the result to double. */
    const Number res(n1 + n2);

    if (res.isZero())
        return BasePtrList();
    else
        return BasePtrList(Numeric::create(res));
}

bool tsym::SumSimpl::haveEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2)
{
    const BasePtr nonConst1(s1->nonConstTerm());
    const BasePtr nonConst2(s2->nonConstTerm());

    if (nonConst1->isOne() || nonConst2->isOne())
        return false;
    else
        return nonConst1->isEqual(nonConst2);
}

tsym::BasePtrList tsym::SumSimpl::simplEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2)
    /* This will process e.g. 2*sqrt(2)*a + sqrt(2)*a = 3*sqrt(3)*a. This simplification will
     * however only affect cases, where the sum of collected coefficients isn't a sum. Doing
     * otherwise would lead to inifinite calls of Product simplification, as the result would be
     * expaned afterwards. */
{
    const BasePtr sum(Sum::create(s1->constTerm(), s2->constTerm()));

    if (sum->isSum())
        return BasePtrList(s1, s2);
    else
        return BasePtrList(Product::create(sum, s1->nonConstTerm()));
}

bool tsym::SumSimpl::haveEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2)
{
    const BasePtr nonNumeric1(s1->nonNumericTerm());
    const BasePtr nonNumeric2(s2->nonNumericTerm());

    /* Both aren't of type Numeric (has been processes earlier). So they can't be both one. */
    return nonNumeric1->isEqual(nonNumeric2);
}

tsym::BasePtrList tsym::SumSimpl::simplEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2)
{
    const BasePtr n(Sum::create(s1->numericTerm(), s2->numericTerm()));
    const BasePtr product(Product::create(n, s1->nonNumericTerm()));

    /* This check has to be done to avoid useless zero summands (a + b - b = a + 0). */
    if (product->isZero())
        return BasePtrList();
    else
        return BasePtrList(product);
}

bool tsym::SumSimpl::haveContractableSinCos(const BasePtr& s1, const BasePtr& s2)
{
    const BasePtr nonConst1(s1->nonConstTerm());
    const BasePtr nonConst2(s2->nonConstTerm());
    const BasePtr const1(s1->constTerm());
    const BasePtr const2(s2->constTerm());

    if (const1->isEqual(const2) && areSinAndCosSquare(nonConst1, nonConst2))
        return haveEqualFirstOperands(nonConst1, nonConst2);
    else
        return false;
}

bool tsym::SumSimpl::areSinAndCosSquare(const BasePtr& s1, const BasePtr& s2)
{
    if (!s1->isPower() || !s2->isPower())
        return false;
    else if (!s1->exp()->isNumericallyEvaluable() || !s2->exp()->isNumericallyEvaluable())
        return false;
    else if (s1->exp()->numericEval() == 2 && s2->exp()->numericEval() == 2)
        return areSinAndCos(s1->base(), s2->base());
    else
        return false;
}

bool tsym::SumSimpl::areSinAndCos(const BasePtr& s1, const BasePtr& s2)
{
    const Name sin("sin");
    const Name cos("cos");

    if (!s1->isFunction() || !s2->isFunction())
        return false;
    else
        return (s1->name() == sin && s2->name() == cos) || (s1->name() == cos && s2->name() == sin);
}

bool tsym::SumSimpl::haveEqualFirstOperands(const BasePtr& pow1, const BasePtr& pow2)
{
    const BasePtr arg1(pow1->base()->operands().front());
    const BasePtr arg2(pow2->base()->operands().front());

    return arg1->isEqual(arg2) || arg1->normal()->isEqual(arg2->normal());
}

tsym::BasePtrList tsym::SumSimpl::simplNSummands(const BasePtrList& u)
{
    const BasePtrList uRest(u.rest());
    const BasePtr u1(u.front());
    BasePtrList simplRest;

    simplRest = simplify(uRest);

    if (u1->isSum())
        return merge(u1->operands(), simplRest);
    else
        return merge(BasePtrList(u1), simplRest);
}
