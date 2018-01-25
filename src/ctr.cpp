
#include <iterator>
#include <numeric>
#include <algorithm>
#include "ctr.h"
#include "cache.h"
#include "numeric.h"
#include "sum.h"
#include "product.h"
#include "logging.h"

tsym::BasePtrCtr tsym::ctr::join(BasePtr&& first, BasePtrCtr&& second)
{
    BasePtrCtr result{ first };

    result.insert(end(result), begin(second), end(second));

    return result;
}

tsym::BasePtrCtr tsym::ctr::join(BasePtrCtr&& first, BasePtrCtr&& second)
{
    BasePtrCtr result{ first };

    result.insert(end(result), begin(second), end(second));

    return result;
}

bool tsym::ctr::areEqual(const BasePtrCtr& ctr1, const BasePtrCtr& ctr2)
{
    auto it1 = cbegin(ctr1);
    auto it2 = cbegin(ctr2);

    if (ctr1.size() != ctr2.size())
        return false;

    for(; it1 != cend(ctr1) && it2 != cend(ctr2); ++it1, ++it2)
        if ((*it1)->isDifferent(*it2))
            return false;

    return true;
}

bool tsym::ctr::areDifferent(const BasePtrCtr& ctr1, const BasePtrCtr& ctr2)
{
    return !areEqual(ctr1, ctr2);
}

bool tsym::ctr::has(const BasePtrCtr& ctr, const BasePtr& element)
{
    for (const auto& item : ctr)
        if (item->isEqual(element))
            return true;
        else if (item->has(element))
            return true;

    return false;
}

tsym::BasePtrCtr tsym::ctr::rest(const BasePtrCtr& ctr)
{
    BasePtrCtr rest(ctr);

    if (rest.empty())
        TSYM_WARNING("Requesting rest of an empty list!");
    else
        rest.pop_front();

    return rest;
}

void tsym::ctr::rest(BasePtrCtr& ctr)
{
    ctr.pop_front();
}

bool tsym::ctr::hasUndefinedElements(const BasePtrCtr& ctr)
{
    return std::any_of(ctr.begin(), ctr.end(), std::mem_fn(&Base::isUndefined));
}

bool tsym::ctr::hasZeroElements(const BasePtrCtr& ctr)
{
    return std::any_of(ctr.begin(), ctr.end(), std::mem_fn(&Base::isZero));
}

bool tsym::ctr::hasSumElements(const BasePtrCtr& ctr)
{
    return std::any_of(ctr.begin(), ctr.end(), std::mem_fn(&Base::isSum));
}

bool tsym::ctr::areElementsNumericallyEvaluable(const BasePtrCtr& ctr)
{
    return std::all_of(ctr.begin(), ctr.end(), std::mem_fn(&Base::isNumericallyEvaluable));
}

bool tsym::ctr::areAllElementsConst(const BasePtrCtr& ctr)
{
    return std::all_of(ctr.begin(), ctr.end(), std::mem_fn(&Base::isConst));
}

unsigned tsym::ctr::complexitySum(const BasePtrCtr& ctr)
{
    return std::accumulate(cbegin(ctr), cend(ctr), 0u,
            [](unsigned complexity, const auto& item){ return complexity + item->complexity(); });
}

tsym::BasePtrCtr tsym::ctr::getConstElements(const BasePtrCtr& ctr)
{
    BasePtrCtr items;

    std::copy_if(begin(ctr), end(ctr), std::back_inserter(items),
            std::mem_fn(&Base::isConst));

    return items;
}

tsym::BasePtrCtr tsym::ctr::getNonConstElements(const BasePtrCtr& ctr)
{
    BasePtrCtr items;

    std::copy_if(begin(ctr), end(ctr), std::back_inserter(items), [](const BasePtr& bp) {
            return !bp->isConst(); });

    return items;
}

namespace tsym {
    namespace {
        void defScalarAndSums(const BasePtrCtr& ctr, BasePtr& scalar, BasePtrCtr& sums)
            /* Splits the given product into a list of sums and everything else (termed 'scalar' here),
             * while the latter is saved as a product. */
        {
            BasePtrCtr scalarFactors;
            BasePtr expanded;

            for (const auto& item : ctr) {
                expanded = item->expand();

                if (expanded->isSum())
                    sums.push_back(expanded);
                else
                    scalarFactors.push_back(expanded);
            }

            scalar = scalarFactors.empty() ? Numeric::one() : Product::create(scalarFactors);
        }

        BasePtr expandProductOf(BasePtrCtr& sums)
            /* Recursively expands a the sum terms of a product, e.g. (a + b)*(c + d) = a*c + a*d + b*c +
             * b*d. */
        {
            const BasePtr first(sums.front());
            BasePtrCtr summands;
            BasePtr second;

            sums.pop_front();

            if (sums.empty())
                return first;

            second = sums.front();
            sums.pop_front();

            for (const auto& item : first->operands())
                summands.push_back(Product::create(item, second)->expand());

            sums.push_front(Sum::create(summands));

            return expandProductOf(sums);
        }

        BasePtr expandProductOf(const BasePtr& scalar, const BasePtr& sum)
        {
            BasePtrCtr summands;
            BasePtr product;

            for (const auto& item : sum->operands()) {
                product = Product::create(scalar, item);
                summands.push_back(product->expand());
            }

            return Sum::create(summands);
        }
    }
}

tsym::BasePtr tsym::ctr::expandAsProduct(const BasePtrCtr& ctr)
{
    static Cache<BasePtrCtr, BasePtr> cache;
    const BasePtr *cached(cache.retrieve(ctr));
    BasePtr expanded;
    BasePtrCtr sums;
    BasePtr scalar;

    if (cached != nullptr)
        return *cached;

    defScalarAndSums(ctr, scalar, sums);

    if (sums.empty())
        expanded = scalar;
    else
        expanded = expandProductOf(scalar, expandProductOf(sums));

    return cache.insertAndReturn(ctr, expanded);
}

void tsym::ctr::subst(BasePtrCtr& ctr, const BasePtr& from, const BasePtr& to)
{
    for (auto& item : ctr)
        item = item->subst(from, to);
}

tsym::BasePtrCtr tsym::ctr::subst(const BasePtrCtr& ctr, const BasePtr& from, const BasePtr& to)
{
    BasePtrCtr res;

    for (const auto& item : ctr)
        res.push_back(item->subst(from, to));

    return res;
}
