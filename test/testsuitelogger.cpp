
#include <cstdio>
#include <iostream>
#include "testsuitelogger.h"


TestSuiteLogger::TestSuiteLogger(bool& suppressLogs) :
    suppressLogs(suppressLogs)
{}

void TestSuiteLogger::log(const Logger::Message& msg) const
{
    std::printf("%s +%d: %s\n", msg.file.c_str(), msg.line, msg.payload.c_str());
}

void TestSuiteLogger::warning(const Logger::Message& msg) const
{
    if (!suppressLogs)
        log(msg);
}

void TestSuiteLogger::error(const Logger::Message& msg) const
{
    warning(msg);
}

void TestSuiteLogger::critical(const Logger::Message& msg) const
{
    warning(msg);
}
