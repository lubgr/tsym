#ifndef TSYM_TEST_TESTLOGGER_H
#define TSYM_TEST_TESTLOGGER_H

#include <vector>
#include "logger.h"

class TestSuiteLogger : public tsym::Logger {
    public:
        explicit TestSuiteLogger(bool suppressLogs);

        void warning(const Logger::Message& msg) const override;
        void error(const Logger::Message& msg) const override;
        void critical(const Logger::Message& msg) const override;

    private:
        void log(std::string&& level, const Logger::Message& msg) const;

        bool suppressLogs;
};

#endif
