
#include <cstdio>
#include <iostream>
#include "testsuitelogger.h"

std::FILE * TestSuiteLogger::debugFp = nullptr;
std::FILE * TestSuiteLogger::infoFp = nullptr;
bool TestSuiteLogger::logToStdout = true;

TestSuiteLogger::TestSuiteLogger(const char *debugFilename, const char *infoFilename)
{
    open(&debugFp, debugFilename);
    open(&infoFp, infoFilename);
}

TestSuiteLogger::~TestSuiteLogger()
{
    std::fclose(debugFp);
    std::fclose(infoFp);
}

void TestSuiteLogger::open(std::FILE **fp, const char *filename)
{
    if (*fp != nullptr)
        return;

    *fp = std::fopen(filename, "w");

    if (*fp == nullptr)
        std::cerr << "Couldn't open " << filename << " for logging output" << std::endl;
}

void TestSuiteLogger::log(const Logger::Message& msg,
        const std::vector<std::FILE*>& filePointer) const
{
    for (auto&& fp : filePointer)
        std::fprintf(fp, "%s +%d: %s\n", msg.file.c_str(), msg.line, msg.payload.c_str());
}

void TestSuiteLogger::debug(const Logger::Message& msg) const
{
    log(msg, { debugFp });
}

void TestSuiteLogger::info(const Logger::Message& msg) const
{
    log(msg, { debugFp, infoFp });
}

void TestSuiteLogger::warning(const Logger::Message& msg) const
{
    std::vector<std::FILE*> fpList = { debugFp, infoFp };

    if (logToStdout)
        fpList.push_back(stdout);

    log(msg, fpList);
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
