
#include "logger.h"
#include <iostream>

tsym::Logger *tsym::Logger::instance = new Logger();

void tsym::Logger::debug(const std::string&, int, const std::string&) const
{
}

void tsym::Logger::info(const std::string&, int, const std::string&) const
{
}

void tsym::Logger::warning(const std::string&, int, const std::string& msg) const
{
    std::cout << msg << std::endl;
}

void tsym::Logger::error(const std::string&, int, const std::string& msg) const
{
    std::cout << msg << std::endl;
}

void tsym::Logger::critical(const std::string&, int, const std::string& msg) const
{
    std::cout << msg << std::endl;
}

void tsym::Logger::setInstance(Logger *logger)
{
    if (instance == logger)
        return;
    else if (instance != nullptr)
        delete instance;

    instance = logger;
}

const tsym::Logger *tsym::Logger::getInstance()
{
    return instance;
}
