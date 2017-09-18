#ifndef TSYM_LOGGER_H
#define TSYM_LOGGER_H

#include <string>
#include <memory>

namespace tsym {
    class Logger {
        public:
            Logger() = default;
            virtual ~Logger() = default;

            virtual void debug(const std::string& file, int line, const std::string& msg) const;
            virtual void info(const std::string& file, int line, const std::string& msg) const;
            virtual void warning(const std::string& file, int line, const std::string& msg) const;
            virtual void error(const std::string& file, int line, const std::string& msg) const;
            virtual void critical(const std::string& file, int line, const std::string& msg) const;

            static void setInstance(std::shared_ptr<const Logger> logger);
            static const std::shared_ptr<const Logger>& getInstance();

        protected:
            Logger(const Logger&) = default;
            Logger& operator = (const Logger&) = default;

        private:
            static std::shared_ptr<const Logger> instance;
    };
}

#endif
