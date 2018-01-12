
#include "base.h"
#include "plaintextprintengine.h"
#include "printer.h"
#include "baseptr.h"

std::ostream& tsym::operator << (std::ostream& stream, const BasePtr& ptr)
{
    auto engine = PlaintextPrintEngine{stream};

    printer::print(engine, ptr);

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
