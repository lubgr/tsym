#ifndef TSYM_BASEPTR_H
#define TSYM_BASEPTR_H

#include <functional>
#include <memory>

namespace tsym {
    class Base;
    using BasePtr = std::shared_ptr<const Base>;

    /* Necessary to ensure that boosts hash specialisations for e.g. standard containers work as
     * expected: The hash is implemented in terms of the pointee, not the pointer itself. This can
     * be somewhat misleading, but as Base subclasses are always compared by value and not
     * reference, they shall equally be treated in unordered maps. */
    size_t hash_value(const BasePtr& ptr);

    std::ostream& operator<<(std::ostream& stream, const BasePtr& ptr);
}

namespace std {
    template <>
    struct hash<tsym::BasePtr>
    /* Implemented in terms of the above function. */
    {
        size_t operator()(const tsym::BasePtr& ptr) const;
    };

    template <> struct equal_to<tsym::BasePtr> {
        bool operator()(const tsym::BasePtr& lhs, const tsym::BasePtr& rhs) const;
    };
}

#endif
