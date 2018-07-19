
#include "polyinfo.h"
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <cassert>
#include "logging.h"
#include "name.h"
#include "number.h"
#include "order.h"
#include "poly.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "undefined.h"

namespace tsym {
    namespace {
        /* Local Functor that determines the order or symbols to be used for multivariate gcd
         * computation. The object has state, i.e., the arguments u and v, for which the gcd shall
         * be computed. Thus, ordering will be different for different u/v, which is inevitable
         * here. The order is such that variables with least degree, but contained in both u and v
         * are first. Variables occuring only in one of u or v are placed at the end. */
        class ComparePolyVariables {
          public:
            ComparePolyVariables(const Base& u, const Base& v)
                : u(u)
                , v(v)
            {}

            bool operator()(const BasePtr& lhs, const BasePtr& rhs) const
            {
                const int lhsMinDegree = std::min(u.degree(*lhs), v.degree(*lhs));
                const int rhsMinDegree = std::min(u.degree(*rhs), v.degree(*rhs));

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
            const Base& u;
            const Base& v;
        };
    }
}

tsym::PolyInfo::PolyInfo(const Base& u, const Base& v)
    : u(u)
    , v(v)
{}

namespace tsym {
    namespace {
        bool isValidPower(const Base& power);
        bool hasValidOperands(const Base& arg);

        bool hasValidType(const Base& arg)
        /* Only symbols, rational Numerics, sums, products or powers with primitive int exponents are
         * allowed. */
        {
            if (arg.isSymbol())
                return true;
            else if (arg.isNumeric())
                return arg.numericEval().isRational();
            else if (arg.isPower())
                return isValidPower(arg);
            else if (arg.isSum() || arg.isProduct())
                return hasValidOperands(arg);
            else
                return false;
        }

        bool isValidPower(const Base& power)
        {
            if (hasValidType(*power.base()) && power.exp()->isNumericallyEvaluable()) {
                auto exp = power.exp()->numericEval();
                return exp.isInt() && integer::fitsInto<int>(exp.numerator()) && exp > 0;
            }

            return false;
        }

        bool hasValidOperands(const Base& arg)
        {
            using boost::adaptors::indirected;
            const auto valid = [](const auto& op) { return hasValidType(op); };

            return boost::algorithm::all_of(arg.operands() | indirected, valid);
        }
    }
}

bool tsym::PolyInfo::isInputValid() const
{
    if (u.isZero() && v.isZero())
        return false;
    else
        return hasValidType(u) && hasValidType(v);
}

tsym::BasePtrList tsym::PolyInfo::listOfSymbols()
{
    symbolList.clear();

    addSymbols(u);
    addSymbols(v);

    symbolList.sort(ComparePolyVariables(u, v));

    return symbolList;
}

void tsym::PolyInfo::addSymbols(const Base& arg)
{
    if (arg.isSymbol())
        addIfNotAlreadyStored(arg);
    else if (arg.isNumeric())
        return;
    else
        addSymbolsNonScalar(arg);
}

void tsym::PolyInfo::addIfNotAlreadyStored(const Base& symbol)
{
    const auto existing = boost::find_if(symbolList, [&symbol](const auto& other) { return symbol.isEqual(*other); });

    if (existing == cend(symbolList))
        symbolList.push_back(symbol.clone());
}

void tsym::PolyInfo::addSymbolsNonScalar(const Base& arg)
{
    if (arg.isSum())
        addSymbols(arg.operands());
    else if (arg.isProduct())
        addSymbols(arg.operands());
    else if (arg.isPower())
        addSymbols(*arg.base());
}

void tsym::PolyInfo::addSymbols(const BasePtrList& list)
{
    using boost::adaptors::indirected;

    for (const auto& item : list | indirected)
        addSymbols(item);
}

tsym::BasePtr tsym::PolyInfo::mainSymbol()
{
    listOfSymbols();

    if (hasCommonSymbol())
        return symbolList.front();
    else
        return Undefined::create();
}

bool tsym::PolyInfo::hasCommonSymbol() const
{
    using boost::adaptors::indirected;

    return boost::algorithm::any_of(
      symbolList | indirected, [this](const auto& symbol) { return u.has(symbol) && v.has(symbol); });
}
