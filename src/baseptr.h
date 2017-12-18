#ifndef TSYM_BASEPTR_H
#define TSYM_BASEPTR_H

#include <functional>
#include <memory>

namespace tsym {
    class Base;
    typedef std::shared_ptr<const Base> BasePtr;

    std::ostream& operator << (std::ostream& stream, const BasePtr& ptr);
}

namespace std
{
    template<> struct hash<tsym::BasePtr>
        /* The hash is for the body of the pointer, not the pointer itself. This can be somewhat
         * misleading, but as Base subclasses are always compared by value and not reference, they
         * shall equally be treated in unordered maps. */
    {
        size_t operator () (const tsym::BasePtr& ptr) const;
    };

    template<> struct equal_to<tsym::BasePtr>
        /* Same as for the hash specialization. */
    {
        bool operator () (const tsym::BasePtr& lhs, const tsym::BasePtr& rhs) const;
    };
}

#endif
