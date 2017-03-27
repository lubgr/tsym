#include <cstddef>
#include <typeinfo>
#include "base.h"
#include "baseptr.h"
#include "symbolmap.h"
#include "undefined.h"
#include "printer.h"
#include "logging.h"

/* For use in the default constructor only. */
namespace tsym {
    namespace {
        const BasePtr& undefinedBaseForNoArgCtor()
        {
            static BasePtr undefined(Undefined::create());

            return undefined;
        }
    }
}

tsym::BasePtr::BasePtr() :
    bp(undefinedBaseForNoArgCtor().bp)
{
    /* Necessary to not destroy the static object above, when this particular undefined BasePtr goes
     * out of scope somewhere. */
    ++bp->refCount;
}

tsym::BasePtr::BasePtr(const Base *base) :
    bp(base)
{
    if (base == nullptr) {
        TSYM_CRITICAL("Initiate Base class with null pointer!");
        bp = undefinedBaseForNoArgCtor().bp;
    }

    ++bp->refCount;
}

tsym::BasePtr::BasePtr(const BasePtr& other) :
    bp(other.bp)
{
    ++bp->refCount;
}

tsym::BasePtr::BasePtr(BasePtr&& other) noexcept :
    bp(std::move(other.bp))
{
    other.bp = undefinedBaseForNoArgCtor().bp;
    ++other.bp->refCount;
}

const tsym::BasePtr& tsym::BasePtr::operator = (const BasePtr& other)
{
    const Base* const old = bp;

    bp = other.bp;

    ++bp->refCount;

    if (--old->refCount == 0)
        delete old;

    return *this;
}

const tsym::BasePtr& tsym::BasePtr::operator = (BasePtr&& other)
{
    const Base* const old = bp;

    bp = other.bp;

    other.bp = undefinedBaseForNoArgCtor().bp;
    ++other.bp->refCount;

    if (--old->refCount == 0)
        delete old;

    return *this;
}

tsym::BasePtr::~BasePtr()
{
    --bp->refCount;

    if (bp->refCount == 0)
        delete bp;
}

const tsym::Base *tsym::BasePtr::operator -> () const
{
    return bp;
}

const tsym::Base& tsym::BasePtr::operator* () const
{
    return *bp;
}

std::ostream& tsym::operator << (std::ostream& stream, const BasePtr& ptr)
{
    Printer printer(ptr);

    printer.print(stream);

    return stream;
}

size_t std::hash<tsym::BasePtr>::operator () (const tsym::BasePtr& ptr) const
{
    return typeid(*ptr).hash_code() ^ (ptr->hash() << 1);
}

bool std::equal_to<tsym::BasePtr>::operator () (const tsym::BasePtr& lhs,
        const tsym::BasePtr& rhs) const
{
    return lhs->isEqual(rhs);
}
