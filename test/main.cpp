
#define BOOST_TEST_MODULE tsym unit tests
#include <boost/test/unit_test.hpp>
#include "logger.h"
#include "testsuitelogger.h"
#include "tsymtests.h"
#include "cache.h"

class TestSuiteLoggingFixture {
    public:
        TestSuiteLoggingFixture()
        {
            auto logger = std::make_unique<const TestSuiteLogger>(globalSuppressLogFlag());

            tsym::Logger::setInstance(std::move(logger));
        }

        ~TestSuiteLoggingFixture()
        {
            tsym::cache::clearRegisteredCaches();
        }
};

BOOST_TEST_GLOBAL_FIXTURE(TestSuiteLoggingFixture);
