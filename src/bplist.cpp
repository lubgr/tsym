
#include "bplist.h"
#include <algorithm>
#include <iterator>
#include <numeric>
#include "cache.h"
#include "logging.h"
#include "numeric.h"
#include "product.h"
#include "sum.h"

tsym::BasePtrList tsym::bplist::join(BasePtr&& first, BasePtrList&& second)
{
    BasePtrList result{first};

    result.insert(end(result), begin(second), end(second));

    return result;
}

tsym::BasePtrList tsym::bplist::join(BasePtrList&& first, BasePtrList&& second)
{
    BasePtrList result{first};

    result.insert(end(result), begin(second), end(second));

    return result;
}

bool tsym::bplist::areEqual(const BasePtrList& list1, const BasePtrList& list2)
{
    auto it1 = cbegin(list1);
    auto it2 = cbegin(list2);

    if (list1.size() != list2.size())
        return false;

    for (; it1 != cend(list1) && it2 != cend(list2); ++it1, ++it2)
        if ((*it1)->isDifferent(*it2))
            return false;

    return true;
}

bool tsym::bplist::has(const BasePtrList& list, const BasePtr& element)
{
    for (const auto& item : list)
        if (item->isEqual(element))
            return true;
        else if (item->has(element))
            return true;

    return false;
}

tsym::BasePtrList tsym::bplist::rest(const BasePtrList& list)
{
    BasePtrList rest(list);

    if (rest.empty())
        TSYM_WARNING("Requesting rest of an empty list!");
    else
        rest.pop_front();

    return rest;
}

void tsym::bplist::rest(BasePtrList& list)
{
    list.pop_front();
}

bool tsym::bplist::hasUndefinedElements(const BasePtrList& list)
{
    return std::any_of(cbegin(list), cend(list), std::mem_fn(&Base::isUndefined));
}

bool tsym::bplist::hasZeroElements(const BasePtrList& list)
{
    return std::any_of(cbegin(list), cend(list), std::mem_fn(&Base::isZero));
}

bool tsym::bplist::hasSumElements(const BasePtrList& list)
{
    return std::any_of(cbegin(list), cend(list), std::mem_fn(&Base::isSum));
}

bool tsym::bplist::areElementsNumericallyEvaluable(const BasePtrList& list)
{
    return std::all_of(cbegin(list), cend(list), std::mem_fn(&Base::isNumericallyEvaluable));
}

bool tsym::bplist::areAllElementsConst(const BasePtrList& list)
{
    return std::all_of(cbegin(list), cend(list), std::mem_fn(&Base::isConst));
}

unsigned tsym::bplist::complexitySum(const BasePtrList& list)
{
    return std::accumulate(cbegin(list), cend(list), 0u,
      [](unsigned complexity, const auto& item) { return complexity + item->complexity(); });
}

tsym::BasePtrList tsym::bplist::getConstElements(const BasePtrList& list)
{
    BasePtrList items;

    std::copy_if(cbegin(list), cend(list), std::back_inserter(items), std::mem_fn(&Base::isConst));

    return items;
}

tsym::BasePtrList tsym::bplist::getNonConstElements(const BasePtrList& list)
{
    BasePtrList items;

    std::copy_if(cbegin(list), cend(list), std::back_inserter(items), [](const BasePtr& bp) { return !bp->isConst(); });

    return items;
}

namespace tsym {
    namespace {
        void defScalarAndSums(const BasePtrList& list, BasePtr& scalar, BasePtrList& sums)
        /* Splits the given product into a list of sums and everything else (termed 'scalar' here),
         * while the latter is saved as a product. */
        {
            BasePtrList scalarFactors;
            BasePtr expanded;

            for (const auto& item : list) {
                expanded = item->expand();

                if (expanded->isSum())
                    sums.push_back(expanded);
                else
                    scalarFactors.push_back(expanded);
            }

            scalar = scalarFactors.empty() ? Numeric::one() : Product::create(scalarFactors);
        }

        BasePtr expandProductOf(BasePtrList& sums)
        /* Recursively expands a the sum terms of a product, e.g. (a + b)*(c + d) = a*c + a*d + b*c +
         * b*d. */
        {
            const BasePtr first(sums.front());
            BasePtrList summands;
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
            BasePtrList summands;
            BasePtr product;

            for (const auto& item : sum->operands()) {
                product = Product::create(scalar, item);
                summands.push_back(product->expand());
            }

            return Sum::create(summands);
        }
    }
}

tsym::BasePtr tsym::bplist::expandAsProduct(const BasePtrList& list)
{
    static cache::RegisteredCache<BasePtrList, BasePtr> cache;
    static auto& map(cache.map);
    const auto lookup = map.find(list);
    BasePtr expanded;
    BasePtrList sums;
    BasePtr scalar;

    if (lookup != cend(map))
        return lookup->second;

    defScalarAndSums(list, scalar, sums);

    if (sums.empty())
        expanded = scalar;
    else
        expanded = expandProductOf(scalar, expandProductOf(sums));

    return map.insert({list, std::move(expanded)})->second;
}

void tsym::bplist::subst(BasePtrList& list, const BasePtr& from, const BasePtr& to)
{
    for (auto& item : list)
        item = item->subst(from, to);
}

tsym::BasePtrList tsym::bplist::subst(const BasePtrList& list, const BasePtr& from, const BasePtr& to)
{
    BasePtrList res;

    for (const auto& item : list)
        res.push_back(item->subst(from, to));

    return res;
}
