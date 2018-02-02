#ifndef TSYM_SUMSIMPL_H
#define TSYM_SUMSIMPL_H

#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    class SumSimpl {
        public:
            BasePtrList simplify(const BasePtrList& summands);

        private:
            BasePtrList simplWithoutCache(const BasePtrList& summands);
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
    };
}

#endif
