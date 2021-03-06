#ifndef TSYM_TEST_FIXTURES_H
#define TSYM_TEST_FIXTURES_H

#include <boost/test/unit_test.hpp>
#include "baseptr.h"

namespace tsym {
    class AbcFixture {
      public:
        AbcFixture();
        AbcFixture(const AbcFixture&) = delete;
        AbcFixture(AbcFixture&&) = delete;
        AbcFixture& operator=(const AbcFixture&) = delete;
        AbcFixture& operator=(AbcFixture&&) = delete;
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
        TestSuiteLoggingFixture(const TestSuiteLoggingFixture&) = delete;
        TestSuiteLoggingFixture(TestSuiteLoggingFixture&&) = delete;
        TestSuiteLoggingFixture& operator=(const TestSuiteLoggingFixture&) = delete;
        TestSuiteLoggingFixture& operator=(TestSuiteLoggingFixture&&) = delete;
        virtual ~TestSuiteLoggingFixture();
    };

    inline decltype(auto) noLogs()
    {
        return *boost::unit_test::fixture<TestSuiteLoggingFixture>(true);
    }
}

#endif
