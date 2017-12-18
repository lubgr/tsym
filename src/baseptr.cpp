#include <cstddef>
#include "base.h"
#include "baseptr.h"
#include "printer.h"
#include "logging.h"

std::ostream& tsym::operator << (std::ostream& stream, const BasePtr& ptr)
{
    Printer printer(ptr);

    printer.print(stream);

    return stream;
}

size_t std::hash<tsym::BasePtr>::operator () (const tsym::BasePtr& ptr) const
{
    return std::hash<std::string>{}(ptr->typeStr()) ^ (ptr->hash() << 1);
}

bool std::equal_to<tsym::BasePtr>::operator () (const tsym::BasePtr& lhs,
        const tsym::BasePtr& rhs) const
{
    return lhs->isEqual(rhs);
}
