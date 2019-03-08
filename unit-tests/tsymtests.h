#ifndef TEST_TSYMTESTS_H
#define TEST_TSYMTESTS_H

/* Main header to be included in each compilation unit with test cases. */

#include <boost/test/unit_test.hpp>
#include <optional>
#include <ostream>
#include "baseptr.h"
#include "number.h"

namespace tsym {
    bool operator==(const BasePtr& lhs, const BasePtr& rhs);
    bool operator!=(const BasePtr& lhs, const BasePtr& rhs);

    std::ostream& operator<<(std::ostream& os, const std::optional<Number>& rhs);
    std::ostream& operator<<(std::ostream& os, const std::optional<BasePtr>& rhs);
}

using boost::test_tools::per_element;
using boost::unit_test::label;

#endif
