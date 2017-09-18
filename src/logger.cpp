
#include "logger.h"
#include <iostream>

std::shared_ptr<const tsym::Logger> tsym::Logger::instance = std::make_shared<const tsym::Logger>();

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

void tsym::Logger::setInstance(std::shared_ptr<const Logger> logger)
{
    instance = logger;
}

const std::shared_ptr<const tsym::Logger>& tsym::Logger::getInstance()
{
    return instance;
}
