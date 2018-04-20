#ifndef TEST_TSYMTESTS_H
#define TEST_TSYMTESTS_H

/* Main header to be included in each compilation unit with test cases. */

#include <boost/test/unit_test.hpp>
#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    bool operator == (const BasePtr& lhs, const BasePtr& rhs);
    bool operator != (const BasePtr& lhs, const BasePtr& rhs);
    bool operator == (const BasePtrList& lhs, const BasePtrList& rhs);
    bool operator != (const BasePtrList& lhs, const BasePtrList& rhs);
}

#endif
