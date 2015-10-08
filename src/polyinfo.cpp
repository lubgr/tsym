
#include <cassert>
#include "polyinfo.h"
#include "baseptrlist.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "undefined.h"
#include "order.h"
#include "logging.h"
#include "poly.h"

namespace tsym {
    namespace {
        /* Local Functor that determines the order or symbols to be used for multivariate gcd
         * computation. The object has state, i.e., the arguments u and v, for which the gcd shall
         * be computed. Thus, ordering will be different for different u/v, which is inevitable
         * here. The order is such that variables with least degree, but contained in both u and v
         * are first. Variables occuring only in one of u or v are placed at the end. */
        class ComparePolyVariables {
            public:
                ComparePolyVariables(const BasePtr& u, const BasePtr& v) : u(u), v(v) {}

                bool operator () (const BasePtr& lhs, const BasePtr& rhs) const
                {
                    const int lhsMinDegree = std::min(u->degree(lhs), v->degree(lhs));
                    const int rhsMinDegree = std::min(u->degree(rhs), v->degree(rhs));

                    if (lhsMinDegree != 0 && rhsMinDegree != 0)
                        return lhsMinDegree < rhsMinDegree;
                    else if (lhsMinDegree == 0 && rhsMinDegree == 0)
                        return lhs->name() < rhs->name();
                    else if (lhsMinDegree != 0)
                        return true;
                    else
                        return false;
                }

            private:
                const BasePtr& u;
                const BasePtr& v;
        };
    }
}

tsym::PolyInfo::PolyInfo() :
    needsUpdate(true)
{}

tsym::PolyInfo::PolyInfo(const BasePtr& u, const BasePtr& v) :
    u(u),
    v(v),
    needsUpdate(true)
{}

void tsym::PolyInfo::set(const BasePtr& u, const BasePtr& v)
{
    this->u = u;
    this->v = v;

    needsUpdate = true;

    symbolList.clear();
}

bool tsym::PolyInfo::isInputValid() const
{
    if (u->isZero() && v->isZero())
        return false;
    else
        return hasValidType(u) && hasValidType(v);
}

bool tsym::PolyInfo::hasValidType(const BasePtr& ptr)
    /* Only symbols, rational Numerics, sums, products or powers with primitive int exponents are
     * allowed. */
{
    if (ptr->isSymbol())
        return true;
    else if (ptr->isNumeric())
        return ptr->numericEval().isRational();
    else if (ptr->isPower())
        return isValidPower(ptr);
    else if (ptr->isSum() || ptr->isProduct())
        return hasValidOperands(ptr);
    else
        return false;
}

bool tsym::PolyInfo::isValidPower(const tsym::BasePtr& power)
{
    Number exp;

    if (hasValidType(power->base()) && power->exp()->isNumericallyEvaluable()) {
        exp = power->exp()->numericEval();
        return exp.isInt() && exp.numerator().fitsIntoInt() && exp > 0;
    }

    return false;
}

bool tsym::PolyInfo::hasValidOperands(const BasePtr& ptr)
{
    BasePtrList::const_iterator it;

    for (it = ptr->operands().begin(); it != ptr->operands().end(); ++it)
        if (!hasValidType(*it))
            return false;

    return true;
}

const tsym::BasePtrList& tsym::PolyInfo::listOfSymbols()
{
    defineSymbolListIfRequired();

    return symbolList;
}

void tsym::PolyInfo::defineSymbolListIfRequired()
{
    if (!needsUpdate)
        return;

    symbolList.clear();

    addSymbols(u);
    addSymbols(v);

    symbolList.sort(ComparePolyVariables(u, v));

    needsUpdate = false;
}

void tsym::PolyInfo::addSymbols(const BasePtr& ptr)
{
    if (ptr->isSymbol())
        addIfNotAlreadyStored(ptr);
    else if (ptr->isNumeric())
        return;
    else
        addSymbolsNonScalar(ptr);
}

void tsym::PolyInfo::addIfNotAlreadyStored(const BasePtr& symbol)
{
    BasePtrList::const_iterator it;

    for (it = symbolList.begin(); it != symbolList.end(); ++it)
        if ((*it)->isEqual(symbol))
            return;

    symbolList.push_back(symbol);
}

void tsym::PolyInfo::addSymbolsNonScalar(const BasePtr& ptr)
{
    if (ptr->isSum())
        addSymbols(ptr->operands());
    else if (ptr->isProduct())
        addSymbols(ptr->operands());
    else if (ptr->isPower())
        addSymbols(ptr->base());
}

void tsym::PolyInfo::addSymbols(const BasePtrList& list)
{
    for (BasePtrList::const_iterator it = list.begin(); it != list.end(); ++it)
        addSymbols(*it);
}

tsym::BasePtr tsym::PolyInfo::mainSymbol()
{
    defineSymbolListIfRequired();

    if (hasCommonSymbol())
        return symbolList.front();
    else
        return Undefined::create();
}

bool tsym::PolyInfo::hasCommonSymbol() const
{
    BasePtrList::const_iterator it;

    for (it = symbolList.begin(); it != symbolList.end(); ++it)
        if (u->has(*it) && v->has(*it))
            return true;

    return false;
}
