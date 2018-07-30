#ifndef TSYM_LOGGER_H
#define TSYM_LOGGER_H

#include <memory>
#include <string>

namespace tsym {
    class Logger {
      public:
        Logger() = default;
        Logger(Logger&&) = default;
        Logger& operator=(Logger&&) = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        virtual ~Logger() = default;

        struct Message {
            const std::string ident;
            const std::string file;
            const int line;
            const std::string payload;
        };

        virtual void debug(const Message& msg) const;
        virtual void info(const Message& msg) const;
        virtual void warning(const Message& msg) const;
        virtual void error(const Message& msg) const;
        virtual void critical(const Message& msg) const;

        static void setInstance(std::unique_ptr<const Logger> logger);
        static const Logger& getInstance();

      private:
        static std::unique_ptr<const Logger> instance;
    };
}

#endif
