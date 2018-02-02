#ifndef TSYM_BASEPTRLIST_H
#define TSYM_BASEPTRLIST_H

#include "baseptr.h"
#include <list>

namespace tsym {
    typedef std::list<BasePtr> BasePtrList;

    std::ostream& operator << (std::ostream& stream, const BasePtrList& items);
}

namespace std
{
    template<> struct hash<tsym::BasePtrList>
    {
        size_t operator () (const tsym::BasePtrList& list) const;
    };

    template<> struct equal_to<tsym::BasePtrList>
    {
        bool operator () (const tsym::BasePtrList& lhs, const tsym::BasePtrList& rhs) const;
    };
}

#endif
