
#include "bplist.h"
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/numeric.hpp>
#include "cache.h"
#include "logging.h"
#include "numeric.h"
#include "product.h"
#include "sum.h"

tsym::BasePtrList tsym::bplist::join(BasePtr&& first, BasePtrList&& second)
{
    second.push_front(std::move(first));

    return std::move(second);
}

tsym::BasePtrList tsym::bplist::join(BasePtrList&& first, BasePtrList&& second)
{
    first.splice(end(first), std::move(second));

    return std::move(first);
}

bool tsym::bplist::areEqual(const BasePtrList& list1, const BasePtrList& list2)
{
    return boost::equal(list1, list2, [](const auto& bp1, const auto bp2) { return bp1->isEqual(*bp2); });
}

bool tsym::bplist::has(const BasePtrList& list, const Base& element)
{
    return boost::algorithm::any_of(
      list, [&element](const auto item) { return item->isEqual(element) || item->has(element); });
}

tsym::BasePtrList tsym::bplist::rest(BasePtrList list)
{
    if (list.empty())
        TSYM_WARNING("Requesting rest of an empty list!");
    else
        list.pop_front();

    return list;
}

bool tsym::bplist::hasUndefinedElements(const BasePtrList& list)
{
    return boost::algorithm::any_of(list, std::mem_fn(&Base::isUndefined));
}

bool tsym::bplist::hasZeroElements(const BasePtrList& list)
{
    return boost::algorithm::any_of(list, std::mem_fn(&Base::isZero));
}

bool tsym::bplist::hasSumElements(const BasePtrList& list)
{
    return boost::algorithm::any_of(list, std::mem_fn(&Base::isSum));
}

bool tsym::bplist::areElementsNumericallyEvaluable(const BasePtrList& list)
{
    return boost::algorithm::all_of(list, std::mem_fn(&Base::isNumericallyEvaluable));
}

bool tsym::bplist::areAllElementsConst(const BasePtrList& list)
{
    return boost::algorithm::all_of(list, std::mem_fn(&Base::isConst));
}

unsigned tsym::bplist::complexitySum(const BasePtrList& list)
{
    return boost::accumulate(
      list, 0u, [](unsigned complexity, const auto& item) { return complexity + item->complexity(); });
}

tsym::BasePtrList tsym::bplist::getConstElements(const BasePtrList& list)
{
    BasePtrList items;

    boost::push_back(items, list | boost::adaptors::filtered(std::mem_fn(&Base::isConst)));

    return items;
}

tsym::BasePtrList tsym::bplist::getNonConstElements(const BasePtrList& list)
{
    using boost::adaptors::filtered;
    BasePtrList items;

    boost::push_back(items, list | filtered(std::not_fn(&Base::isConst)));

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

        BasePtr expandProductOf(const BasePtr& scalar, const Base& sum)
        {
            BasePtrList summands;

            for (const auto& item : sum.operands()) {
                auto product = Product::create(scalar, item);
                summands.push_back(product->expand());
            }

            return Sum::create(std::move(summands));
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
        expanded = expandProductOf(scalar, *expandProductOf(sums));

    return map.insert({list, std::move(expanded)})->second;
}

void tsym::bplist::subst(BasePtrList& list, const Base& from, const BasePtr& to)
{
    for (auto& item : list)
        item = item->subst(from, to);
}

tsym::BasePtrList tsym::bplist::subst(const BasePtrList& list, const Base& from, const BasePtr& to)
{
    BasePtrList res;

    for (const auto& item : list)
        res.push_back(item->subst(from, to));

    return res;
}
