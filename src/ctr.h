#ifndef TSYM_CTR_H
#define TSYM_CTR_H

#include "baseptr.h"
#include "baseptrctr.h"

namespace tsym {
    namespace ctr {
        BasePtrCtr join(BasePtr&& first, BasePtrCtr&& second);
        BasePtrCtr join(BasePtrCtr&& first, BasePtrCtr&& second);

        /* Evaluates equality by comparing the dereferenced items: */
        bool areEqual(const BasePtrCtr& ctr1, const BasePtrCtr& ctr2);
        bool areDifferent(const BasePtrCtr& ctr1, const BasePtrCtr& ctr2);

        /* Returns true if any item is equal to the given object or the item's has method evaluates
         * to true for the given object (hence, different from std::find): */
        bool has(const BasePtrCtr& ctr, const BasePtr& element);

        /* Copies the given container and removes the first element: */
        BasePtrCtr rest(const BasePtrCtr& ctr);
        /* Removes the first element: */
        void rest(BasePtrCtr& ctr);

        /* Shortcuts to STL algorithm calls: */
        bool hasUndefinedElements(const BasePtrCtr& ctr);
        bool hasZeroElements(const BasePtrCtr& ctr);
        bool hasSumElements(const BasePtrCtr& ctr);
        bool areElementsNumericallyEvaluable(const BasePtrCtr& ctr);
        bool areAllElementsConst(const BasePtrCtr& ctr);
        unsigned complexitySum(const BasePtrCtr& ctr);

        /* The Constant class is treated as a variable, only Numerics and a numeric Power are
         * considered as constant items. */
        BasePtrCtr getConstElements(const BasePtrCtr& ctr);
        BasePtrCtr getNonConstElements(const BasePtrCtr& ctr);

        BasePtr expandAsProduct(const BasePtrCtr& ctr);

        /* Substitute by in-place modification: */
        void subst(BasePtrCtr& ctr, const BasePtr& from, const BasePtr& to);
        /* Substitute after copying the container: */
        BasePtrCtr subst(const BasePtrCtr& ctr, const BasePtr& from, const BasePtr& to);
    }
}

#endif
