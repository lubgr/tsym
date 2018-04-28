#ifndef TSYM_INTEGER_H
#define TSYM_INTEGER_H

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <limits>

namespace tsym {
    namespace integer {
        typedef boost::multiprecision::cpp_int Type;

        using boost::multiprecision::abs;
        using boost::multiprecision::gcd;
        using boost::multiprecision::lcm;
        using boost::multiprecision::pow;

        template <class IntegralType> bool fitsInto(const Type& i)
        {
            static const Type upperLimit(std::numeric_limits<IntegralType>::max());
            static const Type lowerLimit(std::numeric_limits<IntegralType>::min());

            return i <= upperLimit && i >= lowerLimit;
        }
    }
}

namespace std {
    template <> struct hash<tsym::integer::Type> {
        size_t operator()(const tsym::integer::Type& n) const;
    };
}

#endif
