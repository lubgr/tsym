
#include "name.h"
#include <boost/functional/hash.hpp>
#include <ostream>
#include <tuple>
#include "namefct.h"

bool tsym::operator==(const Name& lhs, const Name& rhs)
{
    return std::tie(lhs.value, lhs.subscript, lhs.superscript) == std::tie(rhs.value, rhs.subscript, rhs.superscript);
}

bool tsym::operator!=(const Name& lhs, const Name& rhs)
{
    return !(lhs == rhs);
}

bool tsym::operator<(const Name& lhs, const Name& rhs)
{
    return std::tie(lhs.value, lhs.subscript, lhs.superscript) < std::tie(rhs.value, rhs.subscript, rhs.superscript);
}

bool tsym::operator<=(const Name& lhs, const Name& rhs)
{
    return !(lhs > rhs);
}

bool tsym::operator>(const Name& lhs, const Name& rhs)
{
    return rhs < lhs;
}

bool tsym::operator>=(const Name& lhs, const Name& rhs)
{
    return !(lhs < rhs);
}

std::ostream& tsym::operator<<(std::ostream& stream, const Name& name)
{
    return stream << concat(name);
}

size_t tsym::hash_value(const Name& name)
{
    size_t seed = 0;

    boost::hash_combine(seed, std::hash<std::string>{}(name.value));
    boost::hash_combine(seed, std::hash<std::string>{}(name.subscript));
    boost::hash_combine(seed, std::hash<std::string>{}(name.superscript));

    return seed;
}

size_t std::hash<tsym::Name>::operator()(const tsym::Name& name) const
{
    return hash_value(name);
}
