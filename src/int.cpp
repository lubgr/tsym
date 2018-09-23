
#include "int.h"
#include <boost/functional/hash.hpp>

size_t std::hash<tsym::Int>::operator()(const tsym::Int& n) const
{
    return boost::hash<tsym::Int>{}(n);
}
