
#include "logger.h"
#include "testsuitelogger.h"
#include "tsymtests.h"
#include "cache.h"
#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char** argv)
{
    auto logger = std::make_unique<const TestSuiteLogger>(globalSuppressLogFlag());

    tsym::Logger::setInstance(std::move(logger));

    auto result = CommandLineTestRunner::RunAllTests(argc, argv);

    tsym::cache::clearRegisteredCaches();

    return result;
}
