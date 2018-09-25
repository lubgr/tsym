#ifndef TSYM_BASEPTRLISTFCT_H
#define TSYM_BASEPTRLISTFCT_H

#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    BasePtrList join(BasePtr&& first, BasePtrList&& second);
    BasePtrList join(BasePtrList&& first, BasePtrList&& second);

    /* Evaluates equality by comparing the dereferenced items: */
    bool areEqual(const BasePtrList& list1, const BasePtrList& list2);

    /* Copies the given container and removes the first element: */
    BasePtrList rest(BasePtrList list);

    /* Shortcuts to STL algorithm calls: */
    bool hasUndefinedElements(const BasePtrList& list);
    bool hasZeroElements(const BasePtrList& list);
    bool hasSumElements(const BasePtrList& list);
    bool areAllElementsConst(const BasePtrList& list);
    unsigned complexitySum(const BasePtrList& list);

    /* The Constant class is treated as a variable, only Numerics and a numeric Power are
     * considered as constant items. */
    BasePtrList getConstElements(const BasePtrList& list);
    BasePtrList getNonConstElements(const BasePtrList& list);

    BasePtr expandAsProduct(const BasePtrList& list);

    /* Substitute by in-place modification: */
    void subst(BasePtrList& list, const Base& from, const BasePtr& to);
    /* Substitute after copying the container: */
    [[nodiscard]] BasePtrList subst(const BasePtrList& list, const Base& from, const BasePtr& to);
}

#endif
