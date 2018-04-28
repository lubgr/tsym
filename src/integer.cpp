
#include "integer.h"
#include <boost/functional/hash.hpp>

size_t std::hash<tsym::integer::Type>::operator()(const tsym::integer::Type& n) const
{
    return boost::hash<tsym::integer::Type>{}(n);
}
