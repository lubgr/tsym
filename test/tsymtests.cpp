
#include <sstream>
#include "base.h"
#include "var.h"
#include "testsuitelogger.h"
#include "bplist.h"
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

bool operator == (const BasePtrList& lhs, const BasePtrList& rhs)
{
    return bplist::areEqual(lhs, rhs);
}

bool operator != (const BasePtrList& lhs, const BasePtrList& rhs)
{
    return bplist::areDifferent(lhs, rhs);
}

void disableLog()
{
    globalSuppressLogFlag() = true;
}

void enableLog()
{
    globalSuppressLogFlag() = false;
}

bool& globalSuppressLogFlag()
{
    static bool suppressLogs = false;

    return suppressLogs;
}
