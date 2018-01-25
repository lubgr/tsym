#ifndef TSYM_SUMSIMPL_H
#define TSYM_SUMSIMPL_H

#include "baseptr.h"
#include "baseptrctr.h"

namespace tsym {
    class SumSimpl {
        public:
            BasePtrCtr simplify(const BasePtrCtr& summands);

        private:
            BasePtrCtr simplTwoSummands(const BasePtrCtr& u);
            BasePtrCtr simplTwoSummands(const BasePtr& s1, const BasePtr& s2);
            BasePtrCtr simplTwoSummandsWithSum(const BasePtr& s1, const BasePtr& s2);
            BasePtrCtr merge(const BasePtrCtr& l1, const BasePtrCtr& l2);
            BasePtrCtr mergeNonEmpty(const BasePtrCtr& p, const BasePtrCtr& q);
            BasePtrCtr simplTwoSummandsWithoutSum(const BasePtr& s1, const BasePtr& s2);
            BasePtrCtr simplTwoNumerics(const BasePtr& s1, const BasePtr& s2);
            bool haveEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2);
            BasePtrCtr simplEqualNonConstTerms(const BasePtr& s1, const BasePtr& s2);
            bool haveEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2);
            BasePtrCtr simplEqualNonNumericTerms(const BasePtr& s1, const BasePtr& s2);
            bool haveContractableSinCos(const BasePtr& s1, const BasePtr& s2);
            bool areSinAndCosSquare(const BasePtr& s1, const BasePtr& s2);
            bool areSinAndCos(const BasePtr& s1, const BasePtr& s2);
            bool haveEqualFirstOperands(const BasePtr& pow1, const BasePtr& pow2);
            BasePtrCtr simplNSummands(const BasePtrCtr& u);
    };
}

#endif
