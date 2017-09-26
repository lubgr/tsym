#ifndef TSYM_LOGGER_H
#define TSYM_LOGGER_H

#include <string>
#include <memory>

namespace tsym {
    class Logger {
        public:
            Logger() = default;
            virtual ~Logger() = default;

            struct Message {
                Message(const Message& other) = delete;
                Message& operator =(const Message& rhs) = delete;

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
