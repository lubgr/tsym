
#include "integer.h"

size_t std::hash<tsym::integer::Type>::operator () (const tsym::integer::Type& n) const
{
    if (tsym::integer::fitsInto<long long>(n))
        return std::hash<long long>{}(static_cast<long long>(n));
    else
        return std::hash<std::string>{}(
                static_cast<std::ostringstream&>(std::ostringstream() << n).str());
}
