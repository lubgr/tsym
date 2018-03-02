
#include <cassert>
#include "sumsimpl.h"
#include "sum.h"
#include "product.h"
#include "order.h"
#include "name.h"
#include "numeric.h"
#include "bplist.h"
#include "cache.h"
#include "logging.h"

namespace tsym {
    namespace {
        BasePtrList simplTwoSummands(const BasePtrList& u);
        BasePtrList simplTwoSummands(const BasePtr& s1, const BasePtr& s2);
        BasePtrList simplTwoSummandsWithSum(const BasePtr& s1, const BasePtr& s2);
        BasePtrList merge(const BasePtrList& l1, const BasePtrList& l2);
        BasePtrList mergeNonEmpty(const BasePtrList& p, const BasePtrList& q);
        BasePtrList simplTwoSummandsWithoutSum(const BasePtr& s1, const BasePtr& s2);
        BasePtrList simplTwoNumerics(const BasePtr& s1, const BasePtr& s2);
        bool haveEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2);
        BasePtrList simplEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2);
        bool haveEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2);
        BasePtrList simplEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2);
        bool haveContractableSinCos(const BasePtr& s1, const BasePtr& s2);
        bool areSinAndCosSquare(const BasePtr& s1, const BasePtr& s2);
        bool areSinAndCos(const BasePtr& s1, const BasePtr& s2);
        bool haveEqualFirstOperands(const BasePtr& pow1, const BasePtr& pow2);
        BasePtrList simplNSummands(const BasePtrList& u);

        BasePtrList simplWithoutCache(const BasePtrList& summands)
        {
            if (summands.size() == 2)
                return simplTwoSummands(summands);
            else
                return simplNSummands(summands);
        }

        BasePtrList simplTwoSummands(const BasePtrList& u)
        {
            BasePtr s1(*cbegin(u));
            BasePtr s2(*(++cbegin(u)));

            return simplTwoSummands(s1, s2);
        }

        BasePtrList simplTwoSummands(const BasePtr& s1, const BasePtr& s2)
        {
            if (s1->isSum() || s2->isSum())
                return simplTwoSummandsWithSum(s1, s2);
            else
                return simplTwoSummandsWithoutSum(s1, s2);
        }

        BasePtrList simplTwoSummandsWithSum(const BasePtr& s1, const BasePtr& s2)
        {
            BasePtrList l1 = s1->isSum() ? s1->operands() : BasePtrList{ s1 };
            BasePtrList l2 = s2->isSum() ? s2->operands() : BasePtrList{ s2 };

            return merge(l1, l2);
        }

        BasePtrList merge(const BasePtrList& l1, const BasePtrList& l2)
        {
            if (l1.empty())
                return l2;
            else if (l2.empty())
                return l1;
            else
                return mergeNonEmpty(l1, l2);
        }

        BasePtrList mergeNonEmpty(const BasePtrList& p, const BasePtrList& q)
        {
            BasePtr p1(p.front());
            BasePtr q1(q.front());
            const BasePtrList p1q1{ p1, q1 };
            const BasePtrList q1p1{ q1, p1 };
            BasePtrList pRest(bplist::rest(p));
            BasePtrList qRest(bplist::rest(q));
            BasePtrList res;

            res = simplTwoSummands(p1, q1);

            if (res.empty())
                return merge(pRest, qRest);
            else if (res.size() == 1 && res.front()->isZero())
                return merge(pRest, qRest);
            else if (res.size() == 1)
                return bplist::join(std::move(res), merge(pRest, qRest));
            else if (bplist::areEqual(res, p1q1))
                return bplist::join(std::move(p1), merge(pRest, q));
            else if (bplist::areEqual(res, q1p1))
                return bplist::join(std::move(q1), merge(p, qRest));

            TSYM_ERROR("Error merging non-empty lists: %S, %S", p, q);

            return {};
        }

        BasePtrList simplTwoSummandsWithoutSum(const BasePtr& s1, const BasePtr& s2)
        {
            if (s1->isZero())
                return { s2 };
            else if (s2->isZero())
                return { s1 };
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
                return { s1->constTerm() };
            else if (order::doPermute(s1, s2))
                return { s2, s1 };
            else
                return { s1, s2 };
        }

        BasePtrList simplTwoNumerics(const BasePtr& s1, const BasePtr& s2)
        {
            const Number sum(s1->numericEval() + s2->numericEval());

            if (sum.isZero())
                return {};
            else
                return { Numeric::create(sum) };
        }

        bool haveEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2)
        {
            const BasePtr nonConst1(s1->nonConstTerm());
            const BasePtr nonConst2(s2->nonConstTerm());

            if (nonConst1->isOne() || nonConst2->isOne())
                return false;
            else
                return nonConst1->isEqual(nonConst2);
        }

        BasePtrList simplEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2)
            /* This will process e.g. 2*sqrt(2)*a + sqrt(2)*a = 3*sqrt(3)*a. This simplification will
             * however only affect cases, where the sum of collected coefficients isn't a sum. Doing
             * otherwise would lead to inifinite calls of Product simplification, as the result would be
             * expaned afterwards. */
        {
            const BasePtr sum(Sum::create(s1->constTerm(), s2->constTerm()));

            if (!sum->isSum())
                return { Product::create(sum, s1->nonConstTerm()) };
            else if (order::doPermute(s1, s2))
                return { s2, s1 };
            else
                return { s1, s2 };
        }

        bool haveEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2)
        {
            const BasePtr nonNumeric1(s1->nonNumericTerm());
            const BasePtr nonNumeric2(s2->nonNumericTerm());

            /* Both aren't of type Numeric (has been processes earlier). So they can't be both one. */
            return nonNumeric1->isEqual(nonNumeric2);
        }

        BasePtrList simplEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2)
        {
            const BasePtr n(Sum::create(s1->numericTerm(), s2->numericTerm()));
            const BasePtr product(Product::create(n, s1->nonNumericTerm()));

            /* This check has to be done to avoid useless zero summands (a + b - b = a + 0). */
            if (product->isZero())
                return {};
            else
                return { product };
        }

        bool haveContractableSinCos(const BasePtr& s1, const BasePtr& s2)
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

        bool areSinAndCosSquare(const BasePtr& s1, const BasePtr& s2)
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

        bool areSinAndCos(const BasePtr& s1, const BasePtr& s2)
        {
            const Name sin("sin");
            const Name cos("cos");

            if (!s1->isFunction() || !s2->isFunction())
                return false;
            else
                return (s1->name() == sin && s2->name() == cos) || (s1->name() == cos && s2->name() == sin);
        }

        bool haveEqualFirstOperands(const BasePtr& pow1, const BasePtr& pow2)
        {
            const BasePtr arg1(pow1->base()->operands().front());
            const BasePtr arg2(pow2->base()->operands().front());

            return arg1->isEqual(arg2) || arg1->normal()->isEqual(arg2->normal());
        }

        BasePtrList simplNSummands(const BasePtrList& u)
        {
            const BasePtrList uRest(bplist::rest(u));
            const BasePtr u1(u.front());
            BasePtrList simplRest;

            simplRest = simplWithoutCache(uRest);

            if (u1->isSum())
                return merge(u1->operands(), simplRest);
            else
                return merge({ u1 }, simplRest);
        }
    }
}

tsym::BasePtrList tsym::sumsimpl::simplify(const BasePtrList& summands)
{
    static cache::RegisteredCache<BasePtrList, BasePtrList> cache;
    static auto& map(cache.map);
    const auto lookup = map.find(summands);

    if (lookup != cend(map))
        return lookup->second;

    const auto result = simplWithoutCache(summands);

    return map.insert({ summands, result })->second;
}
