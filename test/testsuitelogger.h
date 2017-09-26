#ifndef TSYM_TEST_TESTLOGGER_H
#define TSYM_TEST_TESTLOGGER_H

#include <vector>
#include "logger.h"

class TestSuiteLogger : public tsym::Logger {
    public:
        TestSuiteLogger(const char *debugFilename, const char *infoFilename);
        ~TestSuiteLogger();

        void debug(const Logger::Message& msg) const;
        void info(const Logger::Message& msg) const;
        void warning(const Logger::Message& msg) const;
        void error(const Logger::Message& msg) const;
        void critical(const Logger::Message& msg) const;

        static void disableStdout();
        static void enableStdout();

    private:
        void open(std::FILE **fp, const char *filename);
        void log(const Logger::Message& msg,
                const std::vector<std::FILE*> filePointer) const;

        static std::FILE *debugFp;
        static std::FILE *infoFp;
        static bool logToStdout;
};

#endif
