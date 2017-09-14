
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

void TestSuiteLogger::log(const std::string& file, int line, const std::string& msg,
        const std::vector<std::FILE*> filePointer) const
{
    for (auto&& fp : filePointer)
        std::fprintf(fp, "%s +%d: %s\n", file.c_str(), line, msg.c_str());
}

void TestSuiteLogger::debug(const std::string& file, int line, const std::string& msg) const
{
    log(file, line, msg, { debugFp });
}

void TestSuiteLogger::info(const std::string& file, int line, const std::string& msg) const
{
    log(file, line, msg, { debugFp, infoFp });
}

void TestSuiteLogger::warning(const std::string& file, int line, const std::string& msg) const
{
    std::vector<std::FILE*> fpList = { debugFp, infoFp };
    
    if (logToStdout)
        fpList.push_back(stdout);

    log(file, line, msg, fpList);
}

void TestSuiteLogger::error(const std::string& file, int line, const std::string& msg) const
{
    warning(file, line, msg);
}

void TestSuiteLogger::critical(const std::string& file, int line, const std::string& msg) const
{
    warning(file, line, msg);
}

void TestSuiteLogger::disableStdout()
{
    logToStdout = false;
}

void TestSuiteLogger::enableStdout()
{
    logToStdout = true;
}
