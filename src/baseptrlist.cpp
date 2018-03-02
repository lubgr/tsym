
#include <numeric>
#include <boost/functional/hash.hpp>
#include "plaintextprintengine.h"
#include "printer.h"
#include "baseptrlist.h"
#include "bplist.h"

std::ostream& tsym::operator << (std::ostream& stream, const BasePtrList& items)
{
    PlaintextPrintEngine engine(stream);

    stream << "[ ";

    for (const auto& item : items) {
        printer::print(engine, item);

        if (&item != &*(--end(items)))
            stream << "   ";
    }

    stream << " ]";

    return stream;
}

size_t std::hash<tsym::BasePtrList>::operator () (const tsym::BasePtrList& list) const
{
    return boost::hash<tsym::BasePtrList>{}(list);
}

bool std::equal_to<tsym::BasePtrList>::operator () (const tsym::BasePtrList& lhs,
        const tsym::BasePtrList& rhs) const
{
    return tsym::bplist::areEqual(lhs, rhs);
}
