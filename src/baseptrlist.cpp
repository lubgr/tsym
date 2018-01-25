
#include <functional>
#include <iterator>
#include <numeric>
#include <algorithm>
#include "baseptrlist.h"
#include "numeric.h"
#include "undefined.h"
#include "product.h"
#include "sum.h"
#include "printer.h"
#include "plaintextprintengine.h"
#include "cache.h"
#include "logging.h"

namespace tsym {
    namespace {
        const BasePtr& staticUndefined()
        {
            static const BasePtr undefined(Undefined::create());

            return undefined;
        }
    }
}

tsym::BasePtrList::BasePtrList() {}

tsym::BasePtrList::BasePtrList(const BasePtr& ptr)
{
    list.push_back(ptr);
}

tsym::BasePtrList::BasePtrList(const BasePtr& ptr1, const BasePtr& ptr2)
{
    list = { ptr1, ptr2 };
}

tsym::BasePtrList::BasePtrList(const BasePtr& ptr, const BasePtrList& l)
{
    BasePtrList tmp;

    tmp.push_back(ptr);

    set(tmp, l);
}

tsym::BasePtrList::BasePtrList(const BasePtrList& l1, const BasePtrList& l2)
{
    set(l1, l2);
}

tsym::BasePtrList::BasePtrList(std::initializer_list<BasePtr> list) :
    list(list)
{}

void tsym::BasePtrList::set(const BasePtrList& l1, const BasePtrList& l2)
{
    list.insert(list.end(), l1.begin(), l1.end());
    list.insert(list.end(), l2.begin(), l2.end());
}

void tsym::BasePtrList::clear()
{
    list.clear();
}

void tsym::BasePtrList::push_front(const BasePtr& ptr)
{
    list.push_front(ptr);
}

void tsym::BasePtrList::push_back(const BasePtr& ptr)
{
    list.push_back(ptr);
}

tsym::BasePtr tsym::BasePtrList::pop_front()
{
    const BasePtr first(list.front());

    list.pop_front();

    return first;
}

tsym::BasePtr tsym::BasePtrList::pop_back()
{
    const BasePtr last(list.back());

    list.pop_back();

    return last;
}

void tsym::BasePtrList::insert(iterator pos, const_iterator first, const_iterator last)
{
    list.insert(pos, first, last);
}

tsym::BasePtrList::iterator tsym::BasePtrList::erase(BasePtrList::iterator& it)
{
    return list.erase(it);
}

const tsym::BasePtr& tsym::BasePtrList::front() const
{
    if (!empty())
        return list.front();

    TSYM_WARNING("Requesting first element of an empty list!");

    return staticUndefined();
}

const tsym::BasePtr& tsym::BasePtrList::back() const
{
    if (!empty())
        return list.back();

    TSYM_WARNING("Requesting last element of an empty list!");

    return staticUndefined();
}

bool tsym::BasePtrList::empty() const
{
    return list.empty();
}

size_t tsym::BasePtrList::size() const
{
    return list.size();
}

tsym::BasePtrList::iterator tsym::BasePtrList::begin()
{
    return list.begin();
}

tsym::BasePtrList::iterator tsym::BasePtrList::end()
{
    return list.end();
}

tsym::BasePtrList::const_iterator tsym::BasePtrList::begin() const
{
    return list.begin();
}

tsym::BasePtrList::const_iterator tsym::BasePtrList::end() const
{
    return list.end();
}

tsym::BasePtrList::reverse_iterator tsym::BasePtrList::rbegin()
{
    return list.rbegin();
}

tsym::BasePtrList::reverse_iterator tsym::BasePtrList::rend()
{
    return list.rend();
}

tsym::BasePtrList::const_reverse_iterator tsym::BasePtrList::rbegin() const
{
    return list.rbegin();
}

tsym::BasePtrList::const_reverse_iterator tsym::BasePtrList::rend() const
{
    return list.rend();
}

bool tsym::BasePtrList::isEqual(const BasePtrList& other) const
{
    const_iterator it1(list.begin());
    const_iterator it2(other.list.begin());

    if (size() != other.list.size())
        return false;

    for(; it1 != end() && it2 != other.list.end(); ++it1, ++it2)
        if ((*it1)->isDifferent(*it2))
            return false;

    return true;
}

bool tsym::BasePtrList::isDifferent(const BasePtrList& other) const
{
    return !isEqual(other);
}

bool tsym::BasePtrList::has(const BasePtr& element) const
{
    for (const auto& item : list)
        if (item->isEqual(element))
            return true;
        else if (item->has(element))
            return true;

    return false;
}

tsym::BasePtrList tsym::BasePtrList::rest() const
{
    BasePtrList rest(*this);

    if (rest.empty())
        TSYM_WARNING("Requesting rest of an empty list!");
    else
        rest.pop_front();

    return rest;
}

bool tsym::BasePtrList::hasUndefinedElements() const
{
    return std::any_of(list.begin(), list.end(), std::mem_fn(&Base::isUndefined));
}

bool tsym::BasePtrList::hasZeroElements() const
{
    return std::any_of(list.begin(), list.end(), std::mem_fn(&Base::isZero));
}

bool tsym::BasePtrList::hasSumElements() const
{
    return std::any_of(list.begin(), list.end(), std::mem_fn(&Base::isSum));
}

bool tsym::BasePtrList::areElementsNumericallyEvaluable() const
{
    return std::all_of(list.begin(), list.end(), std::mem_fn(&Base::isNumericallyEvaluable));
}

bool tsym::BasePtrList::areAllElementsConst() const
{
    return std::all_of(list.begin(), list.end(), std::mem_fn(&Base::isConst));
}

tsym::BasePtrList tsym::BasePtrList::getConstElements() const
{
    BasePtrList items;

    std::copy_if(list.begin(), list.end(), std::back_inserter(items.list),
            std::mem_fn(&Base::isConst));

    return items;
}

tsym::BasePtrList tsym::BasePtrList::getNonConstElements() const
{
    BasePtrList items;

    std::copy_if(list.begin(), list.end(), std::back_inserter(items.list), [](const BasePtr& bp) {
            return !bp->isConst(); });

    return items;
}

unsigned tsym::BasePtrList::complexitySum() const
{
    unsigned complexity = 0;

    for (const auto& item : list)
        complexity += item->complexity();

    return complexity;
}

tsym::BasePtr tsym::BasePtrList::expandAsProduct() const
{
    static Cache<BasePtrList, BasePtr> cache;
    const BasePtr *cached(cache.retrieve(*this));
    BasePtr expanded;
    BasePtrList sums;
    BasePtr scalar;

    if (cached != nullptr)
        return *cached;

    defScalarAndSums(scalar, sums);

    if (sums.empty())
        expanded = scalar;
    else
        expanded = expandProductOf(scalar, expandProductOf(sums));

    return cache.insertAndReturn(*this, expanded);
}

void tsym::BasePtrList::defScalarAndSums(BasePtr& scalar, BasePtrList& sums) const
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

tsym::BasePtr tsym::BasePtrList::expandProductOf(BasePtrList& sums) const
    /* Recursively expands a the sum terms of a product, e.g. (a + b)*(c + d) = a*c + a*d + b*c +
     * b*d. */
{
    const BasePtr first(sums.pop_front());
    BasePtrList summands;
    BasePtr second;

    if (sums.empty())
        return first;

    second = sums.pop_front();

    for (const auto& item : first->operands())
        summands.push_back(Product::create(item, second)->expand());

    sums.push_front(Sum::create(summands));

    return expandProductOf(sums);
}

tsym::BasePtr tsym::BasePtrList::expandProductOf(const BasePtr& scalar, const BasePtr& sum) const
{
    BasePtrList summands;
    BasePtr product;

    for (const auto& item : sum->operands()) {
        product = Product::create(scalar, item);
        summands.push_back(product->expand());
    }

    return Sum::create(summands);
}

tsym::BasePtrList tsym::BasePtrList::subst(const BasePtr& from, const BasePtr& to) const
{
    BasePtrList res;

    for (const auto& item : list)
        res.push_back(item->subst(from, to));

    return res;
}

std::ostream& tsym::operator << (std::ostream& stream, const BasePtrList& list)
{
    PlaintextPrintEngine engine(stream);

    stream << "[ ";

    for (const auto& item : list) {
        printer::print(engine, item);

        if (&item != &*(--list.end()))
            stream << "   ";
    }

    stream << " ]";

    return stream;
}

size_t std::hash<tsym::BasePtrList>::operator () (const tsym::BasePtrList& bpList) const
{
    const auto combineHash = [](size_t hashResult, const tsym::BasePtr& item) {
        return hashResult = hashResult ^ (std::hash<tsym::BasePtr>{}(item) << 1); };

    return std::accumulate(bpList.begin(), bpList.end(), (size_t)0, combineHash);
}

bool std::equal_to<tsym::BasePtrList>::operator () (const tsym::BasePtrList& lhs,
        const tsym::BasePtrList& rhs) const
{
    return lhs.isEqual(rhs);
}
