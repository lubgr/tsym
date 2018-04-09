
#include "base.h"
#include "bplist.h"
#include "tsymtests.h"

bool tsym::operator == (const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isEqual(rhs);
}

bool tsym::operator != (const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isDifferent(rhs);
}

bool tsym::operator == (const BasePtrList& lhs, const BasePtrList& rhs)
{
    return bplist::areEqual(lhs, rhs);
}

bool tsym::operator != (const BasePtrList& lhs, const BasePtrList& rhs)
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
