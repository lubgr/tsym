#ifndef TSYM_SUMSIMPL_H
#define TSYM_SUMSIMPL_H

#include "baseptrlist.h"

namespace tsym {
    class SumSimpl {
        public:
            BasePtrList simplify(const BasePtrList& summands);

        private:
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
            BasePtrList simplNSummands(const BasePtrList& u);
    };
}

#endif
