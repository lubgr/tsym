#ifndef TSYM_BPLIST_H
#define TSYM_BPLIST_H

#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    namespace bplist {
        BasePtrList join(BasePtr&& first, BasePtrList&& second);
        BasePtrList join(BasePtrList&& first, BasePtrList&& second);

        /* Evaluates equality by comparing the dereferenced items: */
        bool areEqual(const BasePtrList& list1, const BasePtrList& list2);
        bool areDifferent(const BasePtrList& list1, const BasePtrList& list2);

        /* Returns true if any item is equal to the given object or the item's has method evaluates
         * to true for the given object (hence, different from std::find): */
        bool has(const BasePtrList& list, const BasePtr& element);

        /* Copies the given container and removes the first element: */
        BasePtrList rest(const BasePtrList& list);
        /* Removes the first element: */
        void rest(BasePtrList& list);

        /* Shortcuts to STL algorithm calls: */
        bool hasUndefinedElements(const BasePtrList& list);
        bool hasZeroElements(const BasePtrList& list);
        bool hasSumElements(const BasePtrList& list);
        bool areElementsNumericallyEvaluable(const BasePtrList& list);
        bool areAllElementsConst(const BasePtrList& list);
        unsigned complexitySum(const BasePtrList& list);

        /* The Constant class is treated as a variable, only Numerics and a numeric Power are
         * considered as constant items. */
        BasePtrList getConstElements(const BasePtrList& list);
        BasePtrList getNonConstElements(const BasePtrList& list);

        BasePtr expandAsProduct(const BasePtrList& list);

        /* Substitute by in-place modification: */
        void subst(BasePtrList& list, const BasePtr& from, const BasePtr& to);
        /* Substitute after copying the container: */
        BasePtrList subst(const BasePtrList& list, const BasePtr& from, const BasePtr& to);
    }
}

#endif
