
#include <sstream>
#include "base.h"
#include "logging.h"
#include "tsymtests.h"

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
    TSYM_INFO("Following warnings/errors are probably an intended part of testing");

    plic::configFile("test/logdisable.py");
}

void enableLog()
{
    TSYM_INFO("Warnings/errors aren't intended from now on!");

    plic::configFile("test/logenable.py");
}
