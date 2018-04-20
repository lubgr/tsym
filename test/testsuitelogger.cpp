
#include <cstdio>
#include <iostream>
#include "testsuitelogger.h"
#include <boost/test/unit_test.hpp>

TestSuiteLogger::TestSuiteLogger(bool suppressLogs) :
    suppressLogs(suppressLogs)
{}

void TestSuiteLogger::log(std::string&& level, const Logger::Message& msg) const
{
    if (!suppressLogs)
        BOOST_ERROR("Unintended " << level << " msg., " << msg.file << " +" << msg.line << ": " << msg.payload);
    else
        BOOST_TEST_MESSAGE(msg.file << " +" << msg.line << ": " << msg.payload);
}

void TestSuiteLogger::warning(const Logger::Message& msg) const
{
    log("warning", msg);
}

void TestSuiteLogger::error(const Logger::Message& msg) const
{
    log("error", msg);
}

void TestSuiteLogger::critical(const Logger::Message& msg) const
{
    log("critical", msg);
}
