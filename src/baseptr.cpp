
#include <cstddef>
#include "base.h"
#include "baseptr.h"
#include "symbolregistry.h"
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
    /* This sets the reference count to 2, which is necessary to not destroy the static object
     * above, when this particular undefined BasePtr goes out of scope somewhere. */
    ++bp->refCount;

    #ifndef NDEBUG
    prettyStr = "Undefined";
    #endif
}

tsym::BasePtr::BasePtr(const Base *base) :
    bp(base)
{
    if (base == NULL)
        logging::fatal() << "Initiate Base class with NULL pointer!";

    ++bp->refCount;

    #ifndef NDEBUG
    prettyStr = (Printer(*this)).getStr();
    #endif

    if (bp->isSymbol())
        SymbolRegistry::add(*this);
}

tsym::BasePtr::BasePtr(const BasePtr& other) :
    bp(other.bp)
{
    if (other.bp == NULL)
        logging::fatal() << "Initiate Base class with NULL pointer!";

    #ifndef NDEBUG
    prettyStr = other.prettyStr;
    #endif

    ++bp->refCount;

    if (bp->isSymbol())
        SymbolRegistry::add(*this);
}

const tsym::BasePtr& tsym::BasePtr::operator = (const BasePtr& other)
{
    const Base* const old = bp;

    if (bp->isSymbol())
        SymbolRegistry::remove(*this);
    if (other->isSymbol())
        SymbolRegistry::add(other);

    bp = other.bp;

    #ifndef NDEBUG
    prettyStr = other.prettyStr;
    #endif

    ++bp->refCount;

    if (--old->refCount == 0)
        delete old;

    return *this;
}

tsym::BasePtr::~BasePtr()
{
    --bp->refCount;

    if (bp->isSymbol())
        SymbolRegistry::remove(*this);

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

const tsym::Base *tsym::BasePtr::base() const
{
    return bp;
}

std::ostream& tsym::operator << (std::ostream& stream, const BasePtr& ptr)
{
    Printer printer(ptr);

    printer.print(stream);

    return stream;
}
