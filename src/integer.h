#ifndef TSYM_INTEGER_H
#define TSYM_INTEGER_H

#include <limits>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>

namespace tsym {
    namespace integer {
        typedef boost::multiprecision::cpp_int Type;

        using boost::multiprecision::gcd;
        using boost::multiprecision::lcm;
        using boost::multiprecision::pow;
        using boost::multiprecision::abs;

        template<class IntegralType> bool fitsInto(const Type& i)
        {
            static const Type upperLimit(std::numeric_limits<IntegralType>::max());
            static const Type lowerLimit(std::numeric_limits<IntegralType>::min());

            return i <= upperLimit && i >= lowerLimit;
        }
    }
}

namespace std {
    template<> struct hash<tsym::integer::Type>
    {
        size_t operator () (const tsym::integer::Type& n) const;
    };
}

#endif
