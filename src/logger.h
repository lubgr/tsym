#ifndef TSYM_LOGGER_H
#define TSYM_LOGGER_H

#include <string>

namespace tsym {
    class Logger {
        public:
            virtual void debug(const std::string& file, int line, const std::string& msg) const;
            virtual void info(const std::string& file, int line, const std::string& msg) const;
            virtual void warning(const std::string& file, int line, const std::string& msg) const;
            virtual void error(const std::string& file, int line, const std::string& msg) const;
            virtual void critical(const std::string& file, int line, const std::string& msg) const;

            static void setInstance(Logger *logger);
            static const Logger *getInstance();

        protected:
            Logger() = default;
            virtual ~Logger() = default;
            Logger(const Logger&) = delete;
            const Logger& operator = (const Logger&) = delete;

        private:
            static Logger *instance;
    };
}

#endif
