
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
    trlog::appendToCerr(trlog::FATAL, trlog::PRINT_LEVEL);
}

void enableLog()
{
    trlog::appendToCerr(trlog::WARNING, trlog::PRINT_LEVEL);
}
