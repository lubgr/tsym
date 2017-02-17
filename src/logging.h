#ifndef TSYM_LOGGING_H
#define TSYM_LOGGING_H

#include <cstring>
#include "plic/plic.h"

#define TSYM_FILE (std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__)

#define TSYM_DEBUG(...) plic::debug("tsym", plic::FILENAME, TSYM_FILE, plic::LINE, __LINE__, \
        __VA_ARGS__)
#define TSYM_INFO(...) plic::info("tsym", plic::FILENAME, TSYM_FILE, plic::LINE, __LINE__, \
        __VA_ARGS__)
#define TSYM_WARNING(...) plic::warning("tsym", plic::FILENAME, TSYM_FILE, plic::LINE, __LINE__, \
        __VA_ARGS__)
#define TSYM_ERROR(...) plic::error("tsym", plic::FILENAME, TSYM_FILE, plic::LINE, __LINE__, \
        __VA_ARGS__)
#define TSYM_CRITICAL(...) plic::critical("tsym", plic::FILENAME, TSYM_FILE, plic::LINE, __LINE__, \
        __VA_ARGS__)

#endif
