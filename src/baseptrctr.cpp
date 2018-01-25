
#include <numeric>
#include "plaintextprintengine.h"
#include "printer.h"
#include "baseptrctr.h"
#include "ctr.h"

std::ostream& tsym::operator << (std::ostream& stream, const BasePtrCtr& items)
{
    PlaintextPrintEngine engine(stream);

    stream << "[ ";

    for (const auto& item : items) {
        printer::print(engine, item);

        if (&item != &*(--items.end()))
            stream << "   ";
    }

    stream << " ]";

    return stream;
}

size_t std::hash<tsym::BasePtrCtr>::operator () (const tsym::BasePtrCtr& ctr) const
{
    const auto combineHash = [](size_t hashResult, const tsym::BasePtr& item) {
        return hashResult = hashResult ^ (std::hash<tsym::BasePtr>{}(item) << 1); };

    return std::accumulate(ctr.begin(), ctr.end(), (size_t)0, combineHash);
}

bool std::equal_to<tsym::BasePtrCtr>::operator () (const tsym::BasePtrCtr& lhs,
        const tsym::BasePtrCtr& rhs) const
{
    return tsym::ctr::areEqual(lhs, rhs);
}
