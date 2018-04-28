#ifndef TSYM_OPTIONAL_H
#define TSYM_OPTIONAL_H

#include <boost/optional.hpp>

namespace tsym {
    template <class T> using optional = boost::optional<T>;
}

#endif
