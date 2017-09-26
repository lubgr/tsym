
#include "logger.h"
#include <iostream>

std::shared_ptr<const tsym::Logger> tsym::Logger::instance = std::make_shared<const tsym::Logger>();

void tsym::Logger::debug(const Message&) const
{
}

void tsym::Logger::info(const Message&) const
{
}

void tsym::Logger::warning(const Message& msg) const
{
    std::cout << msg.payload << std::endl;
}

void tsym::Logger::error(const Message& msg) const
{
    warning(msg);
}

void tsym::Logger::critical(const Message& msg) const
{
    warning(msg);
}

void tsym::Logger::setInstance(std::shared_ptr<const Logger> logger)
{
    instance = logger;
}

const std::shared_ptr<const tsym::Logger>& tsym::Logger::getInstance()
{
    return instance;
}
