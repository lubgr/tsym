
#include <sstream>
#include "base.h"
#include "logging.h"
#include "cpputest.h"

using namespace tsym;

bool operator == (const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isEqual(rhs);
}

bool operator != (const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isDifferent(rhs);
}

void disableLog()
{
    trlog::info() << "Following warnings/errors are probably an intended part of testing";
    trlog::appendToCerr(trlog::FATAL, trlog::PRINT_LEVEL);
}

void enableLog()
{
    trlog::appendToCerr(trlog::WARNING, trlog::PRINT_LEVEL);
    trlog::info() << "Warnings/errors aren't intended from now on!";
}
