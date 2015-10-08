
#include <sstream>
#include "cpputest.h"
#include "base.h"
#include "logging.h"

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
#ifdef USE_TRLOG
    trlog::appendToCerr(trlog::FATAL, trlog::PRINT_LEVEL);
#else
    logging::verbosity = logging::FATAL;
#endif
}

void enableLog()
{
#ifdef USE_TRLOG
    trlog::appendToCerr(trlog::WARNING, trlog::PRINT_LEVEL);
#else
    logging::verbosity = logging::WARNING;
#endif
}
