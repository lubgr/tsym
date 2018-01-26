#ifndef TSYM_TEST_TESTLOGGER_H
#define TSYM_TEST_TESTLOGGER_H

#include <vector>
#include "logger.h"

class TestSuiteLogger : public tsym::Logger {
    public:
        TestSuiteLogger() = default;

        void warning(const Logger::Message& msg) const override;
        void error(const Logger::Message& msg) const override;
        void critical(const Logger::Message& msg) const override;

        static void disableStdout();
        static void enableStdout();

    private:
        void log(const Logger::Message& msg) const;

        static bool logToStdout;
};

#endif
