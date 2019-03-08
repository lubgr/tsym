#ifndef TSYM_INT_H
#define TSYM_INT_H

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <limits>

namespace tsym {
    using Int = boost::multiprecision::cpp_int;
    using boost::multiprecision::abs;
    using boost::multiprecision::gcd;
    using boost::multiprecision::lcm;
    using boost::multiprecision::pow;

    template <class IntegralType> bool fitsInto(const Int& i)
    {
        static const Int upperLimit(std::numeric_limits<IntegralType>::max());
        static const Int lowerLimit(std::numeric_limits<IntegralType>::min());

        return i <= upperLimit && i >= lowerLimit;
    }
}

namespace std {
    template <> struct hash<tsym::Int> {
        size_t operator()(const tsym::Int& n) const;
    };
}

#endif
