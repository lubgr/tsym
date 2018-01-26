
#include <cstdio>
#include <iostream>
#include "testsuitelogger.h"

bool TestSuiteLogger::logToStdout = true;

void TestSuiteLogger::log(const Logger::Message& msg) const
{
    std::printf("%s +%d: %s\n", msg.file.c_str(), msg.line, msg.payload.c_str());
}

void TestSuiteLogger::warning(const Logger::Message& msg) const
{
    if (logToStdout)
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

void TestSuiteLogger::disableStdout()
{
    logToStdout = false;
}

void TestSuiteLogger::enableStdout()
{
    logToStdout = true;
}
