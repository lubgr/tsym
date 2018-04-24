#ifndef TSYM_TEST_FIXTURES_H
#define TSYM_TEST_FIXTURES_H

#include <boost/test/unit_test.hpp>
#include "baseptr.h"

namespace tsym {
    class AbcFixture {
        public:
            AbcFixture();
            virtual ~AbcFixture() = default;

            const BasePtr a;
            const BasePtr b;
            const BasePtr c;
            const BasePtr d;
            const BasePtr e;
            const BasePtr f;
            const BasePtr g;
            const BasePtr h;

            const BasePtr& zero;
            const BasePtr& one;
            const BasePtr& two;
            const BasePtr& three;
            const BasePtr& four;
            const BasePtr five;
            const BasePtr six;
            const BasePtr seven;
            const BasePtr eight;
            const BasePtr nine;
            const BasePtr ten;

            const BasePtr& pi;
            const BasePtr& undefined;
    };

    struct TestSuiteLoggingFixture {
        explicit TestSuiteLoggingFixture(bool suppressLogs);
        virtual ~TestSuiteLoggingFixture();
    };

    inline auto noLogs() -> decltype(*boost::unit_test::fixture<TestSuiteLoggingFixture>(true))
    {
        return *boost::unit_test::fixture<TestSuiteLoggingFixture>(true);
    }
}

#endif
