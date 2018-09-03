
#include "tsymtests.h"
#include "base.h"
#include "bplist.h"

bool tsym::operator==(const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isEqual(*rhs);
}

bool tsym::operator!=(const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isDifferent(*rhs);
}

std::ostream& tsym::operator<<(std::ostream& os, const std::optional<Number>& rhs)
{
    if (rhs)
        return os << *rhs;
    else
        return os << "std::nullopt";
}

std::ostream& tsym::operator<<(std::ostream& os, const std::optional<BasePtr>& rhs)
{
    if (rhs)
        return os << *rhs;
    else
        return os << "std::nullopt";
}
