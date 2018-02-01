
#include <numeric>
#include <boost/functional/hash.hpp>
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
    return boost::hash<tsym::BasePtrCtr>{}(ctr);
}

bool std::equal_to<tsym::BasePtrCtr>::operator () (const tsym::BasePtrCtr& lhs,
        const tsym::BasePtrCtr& rhs) const
{
    return tsym::ctr::areEqual(lhs, rhs);
}
