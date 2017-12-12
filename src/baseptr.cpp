#include <cstddef>
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
    rep(undefinedBaseForNoArgCtor().rep)
{}

tsym::BasePtr::BasePtr(const Base *base) :
    rep(base)
{
    if (base == nullptr) {
        TSYM_CRITICAL("Initiate Base class with null pointer!");
        rep = undefinedBaseForNoArgCtor().rep;
    }
}

tsym::BasePtr::BasePtr(const std::shared_ptr<const Base>& rep) :
    rep(rep)
{}

const tsym::Base *tsym::BasePtr::operator -> () const
{
    return rep.get();
}

const tsym::Base& tsym::BasePtr::operator* () const
{
    return *rep.get();
}

std::ostream& tsym::operator << (std::ostream& stream, const BasePtr& ptr)
{
    Printer printer(ptr);

    printer.print(stream);

    return stream;
}

size_t std::hash<tsym::BasePtr>::operator () (const tsym::BasePtr& ptr) const
{
    return std::hash<std::string>{}(ptr->typeStr()) ^ (ptr->hash() << 1);
}

bool std::equal_to<tsym::BasePtr>::operator () (const tsym::BasePtr& lhs,
        const tsym::BasePtr& rhs) const
{
    return lhs->isEqual(rhs);
}
