
#include "baseptrlist.h"
#include "numeric.h"
#include "product.h"
#include "sum.h"
#include "printer.h"
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
    list.push_back(ptr1);
    list.push_back(ptr2);
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
    for (const_iterator it = list.begin(); it != list.end(); ++it)
        if ((*it)->isEqual(element))
            return true;
        else if ((*it)->has(element))
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
    return isTrueForAtLeastOneElement(&Base::isUndefined);
}

bool tsym::BasePtrList::isTrueForAtLeastOneElement(bool (Base::*method)() const) const
{
    for (const_iterator it = list.begin(); it != list.end(); ++it)
        if (((**it).*method)())
            return true;

    return false;
}

bool tsym::BasePtrList::hasZeroElements() const
{
    return isTrueForAtLeastOneElement(&Base::isZero);
}

bool tsym::BasePtrList::hasSumElements() const
{
    return isTrueForAtLeastOneElement(&Base::isSum);
}

bool tsym::BasePtrList::areElementsNumericallyEvaluable() const
{
    return isTrueForAllElements(&Base::isNumericallyEvaluable);
}

bool tsym::BasePtrList::isTrueForAllElements(bool (Base::*method)() const) const
{
    for (const_iterator it = list.begin(); it != list.end(); ++it)
        if (!((**it).*method)())
            return false;

    return true;
}

bool tsym::BasePtrList::areAllElementsConst() const
{
    return isTrueForAllElements(&Base::isConst);
}

tsym::BasePtrList tsym::BasePtrList::getConstElements() const
{
    BasePtrList items;

    for (const_iterator it = list.begin(); it != list.end(); ++it)
        if ((*it)->isConst())
            items.push_back(*it);

    return items;
}

tsym::BasePtrList tsym::BasePtrList::getNonConstElements() const
{
    BasePtrList items;

    for (const_iterator it = list.begin(); it != list.end(); ++it)
        if (!(*it)->isConst())
            items.push_back(*it);

    return items;
}

tsym::BasePtr tsym::BasePtrList::expandAsProduct() const
{
    BasePtrList sums;
    BasePtr scalar;

    defScalarAndSums(scalar, sums);

    if (sums.empty())
        return scalar;
    else
        return expandProductOf(scalar, expandProductOf(sums));
}

void tsym::BasePtrList::defScalarAndSums(BasePtr& scalar, BasePtrList& sums) const
    /* Splits the given product into a list of sums and everything else (termed 'scalar' here),
     * while the latter is saved as a product. */
{
    BasePtrList scalarFactors;
    BasePtr expanded;

    for (const_iterator it = list.begin(); it != list.end(); ++it) {
        expanded = (*it)->expand();

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

    for (const_iterator it = first->operands().begin(); it != first->operands().end(); ++it)
        summands.push_back(Product::create(*it, second)->expand());

    sums.push_front(Sum::create(summands));

    return expandProductOf(sums);
}

tsym::BasePtr tsym::BasePtrList::expandProductOf(const BasePtr& scalar, const BasePtr& sum) const
{
    const BasePtrList& terms(sum->operands());
    BasePtrList summands;
    BasePtr product;

    for (const_iterator it = terms.begin(); it != terms.end(); ++it) {
        product = Product::create(scalar, *it);
        summands.push_back(product->expand());
    }

    return Sum::create(summands);
}

tsym::BasePtrList tsym::BasePtrList::subst(const BasePtr& from, const BasePtr& to) const
{
    BasePtrList res;

    for (const_iterator it = begin(); it != end(); ++it)
        res.push_back((*it)->subst(from, to));

    return res;
}

std::ostream& tsym::operator << (std::ostream& stream, const BasePtrList& list)
{
    BasePtrList::const_iterator it;
    Printer printer;

    stream << "[ ";

    for (it = list.begin(); it != list.end(); ++it) {
        printer.set(*it);
        printer.print(stream);

        if (it != --list.end())
            stream << "   ";
    }

    stream << " ]";

    return stream;
}
