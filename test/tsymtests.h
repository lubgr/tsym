#ifndef TEST_TSYMTESTS_H
#define TEST_TSYMTESTS_H

/* Main header to be included in each compilation unit with test cases. This file includes the main
 * header of boost test and provides common definitions of functions necessary to suppress warning
 * logs (if intended) and the like. */

#include <boost/test/unit_test.hpp>
#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    bool operator == (const BasePtr& lhs, const BasePtr& rhs);
    bool operator != (const BasePtr& lhs, const BasePtr& rhs);
    bool operator == (const BasePtrList& lhs, const BasePtrList& rhs);
    bool operator != (const BasePtrList& lhs, const BasePtrList& rhs);
}

/* Functions to disable and enable logging messages except the fatal level. */
void disableLog();
void enableLog();
bool& globalSuppressLogFlag();

#endif
