#ifndef TSYM_BASEPTR_H
#define TSYM_BASEPTR_H

#include <functional>

namespace tsym { class Base; }

namespace tsym {
    class BasePtr {
        /* Intrusive reference counting custodian of the Base math object. This class is to be
         * passed around, used as an operand etc. As the object it is pointing to, instances of this
         * class are immutable. */
        public:
            /* Creates an Undefined Base class: */
            BasePtr();
            explicit BasePtr(const Base *base);
            BasePtr(const BasePtr& other);
            BasePtr(BasePtr&& other) noexcept;
            const BasePtr& operator = (const BasePtr& other);
            const BasePtr& operator = (BasePtr&& other);
            ~BasePtr();

            const Base *operator -> () const;
            const Base& operator * () const;

        private:
            const Base* bp;
    };

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
