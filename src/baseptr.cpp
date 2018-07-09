
#include "baseptr.h"
#include <boost/functional/hash.hpp>
#include "base.h"
#include "plaintextprintengine.h"
#include "printer.h"

std::ostream& tsym::operator<<(std::ostream& stream, const BasePtr& ptr)
{
    auto engine = PlaintextPrintEngine{stream};

    printer::print(engine, ptr);

    return stream;
}

size_t tsym::hash_value(const BasePtr& ptr)
{
    size_t seed = 0;

    boost::hash_combine(seed, ptr->typeStr());
    boost::hash_combine(seed, ptr->hash());

    return seed;
}

size_t std::hash<tsym::BasePtr>::operator()(const tsym::BasePtr& ptr) const
{
    return tsym::hash_value(ptr);
}

bool std::equal_to<tsym::BasePtr>::operator()(const tsym::BasePtr& lhs, const tsym::BasePtr& rhs) const
{
    return lhs->isEqual(*rhs);
}
