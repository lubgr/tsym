#ifndef TSYM_PTRLIST_H
#define TSYM_PTRLIST_H

#include <list>
#include <initializer_list>
#include <functional>
#include "baseptr.h"

namespace tsym {
    /* Simple wrapper around a list of BasePtr. Only obvious functions are 'forwarded' to a public
     * interface, e.g. only constant iterators. */
    class BasePtrList {
        public:
            typedef std::list<BasePtr>::iterator iterator;
            typedef std::list<BasePtr>::const_iterator const_iterator;
            typedef std::list<BasePtr>::reverse_iterator reverse_iterator;
            typedef std::list<tsym::BasePtr>::const_reverse_iterator const_reverse_iterator;

        public:
            BasePtrList();
            /* Some additional constructors are provided to provide easy creation of often used
             * unions of BasePtr or BasePtrList objects. */
            explicit BasePtrList(const BasePtr& ptr);
            BasePtrList(const BasePtr& ptr1, const BasePtr& ptr2);
            BasePtrList(const BasePtr& ptr, const BasePtrList& l);
            BasePtrList(const BasePtrList& l1, const BasePtrList& l2);
            BasePtrList(std::initializer_list<BasePtr> list);
            BasePtrList(const BasePtrList& other) = default;
            BasePtrList(BasePtrList&& other) = default;
            BasePtrList& operator = (const BasePtrList& rhs) = default;
            BasePtrList& operator = (BasePtrList&& rhs) = default;

            void clear();
            void push_front(const BasePtr& ptr);
            void push_back(const BasePtr& ptr);
            /* Pop elements and return that element. This differs from the standard list: */
            BasePtr pop_front();
            BasePtr pop_back();
            void insert(iterator pos, const_iterator first, const_iterator last);
            iterator erase(iterator& it);

            const BasePtr& front() const;
            const BasePtr& back() const;

            bool empty() const;
            size_t size() const;

            iterator begin();
            iterator end();
            const_iterator begin() const;
            const_iterator end() const;
            reverse_iterator rbegin();
            reverse_iterator rend();
            const_reverse_iterator rbegin() const;
            const_reverse_iterator rend() const;

            template <class Compare> void sort(Compare compare) { list.sort(compare); }

            bool isEqual(const BasePtrList& other) const;
            bool isDifferent(const BasePtrList& other) const;
            bool has(const BasePtr& element) const;
            /* Returns a list identical to the current one, but with the first element removed: */
            BasePtrList rest() const;
            bool hasUndefinedElements() const;
            bool hasZeroElements() const;
            bool hasSumElements() const;
            bool areElementsNumericallyEvaluable() const;
            bool areAllElementsConst() const;
            /* Here, the Constant class is treated as a variable, only Numerics and a numeric Power
             * are considered as constant items. */
            BasePtrList getConstElements() const;
            BasePtrList getNonConstElements() const;
            /* Takes elements as factors of a product and expands them: */
            BasePtr expandAsProduct() const;
            BasePtrList subst(const BasePtr& from, const BasePtr& to) const;

        private:
            bool isTrueForAtLeastOneElement(bool (Base::*method)() const) const;
            bool isTrueForAllElements(bool (Base::*method)() const) const;
            void set(const BasePtrList& l1, const BasePtrList& l2);
            void defScalarAndSums(BasePtr& scalar, BasePtrList& sums) const;
            BasePtr expandProductOf(BasePtrList& sums) const;
            BasePtr expandProductOf(const BasePtr& scalar, const BasePtr& sum) const;

            std::list<BasePtr> list;
    };

    std::ostream& operator << (std::ostream& stream, const BasePtrList& list);
}

namespace std
{
    template<> struct hash<tsym::BasePtrList>
    {
        size_t operator () (const tsym::BasePtrList& bpList) const;
    };

    template<> struct equal_to<tsym::BasePtrList>
    {
        bool operator () (const tsym::BasePtrList& lhs, const tsym::BasePtrList& rhs) const;
    };
}

#endif
