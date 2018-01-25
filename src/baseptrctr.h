#ifndef TSYM_BASEPTRCTR_H
#define TSYM_BASEPTRCTR_H

#include "baseptr.h"
#include <list>

namespace tsym {
    typedef std::list<BasePtr> BasePtrCtr;

    std::ostream& operator << (std::ostream& stream, const BasePtrCtr& items);
}

namespace std
{
    template<> struct hash<tsym::BasePtrCtr>
    {
        size_t operator () (const tsym::BasePtrCtr& ctr) const;
    };

    template<> struct equal_to<tsym::BasePtrCtr>
    {
        bool operator () (const tsym::BasePtrCtr& lhs, const tsym::BasePtrCtr& rhs) const;
    };
}

#endif
