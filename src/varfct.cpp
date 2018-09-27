
#include "varfct.h"
#include <boost/range/algorithm/find.hpp>
#include <chrono>
#include "basefct.h"
#include "constant.h"
#include "fraction.h"
#include "logarithm.h"
#include "logging.h"
#include "namefct.h"
#include "parser.h"
#include "power.h"
#include "symbolmap.h"
#include "trigonometric.h"

namespace tsym {
    namespace {
        std::pair<Var, Var> normalToFraction(const BasePtr& rep)
        {
            Fraction normalizedFrac;
            SymbolMap map;
            BasePtr denom;
            BasePtr num;

            normalizedFrac = rep->normal(map);

            denom = map.replaceTmpSymbolsBackFrom(normalizedFrac.denom);
            num = map.replaceTmpSymbolsBackFrom(normalizedFrac.num);

            return std::make_pair(Var(num), Var(denom));
        }

        void insertSymbolIfNotPresent(const BasePtr& symbol, std::vector<Var>& symbols)
        {
            const Var term(symbol);

            if (boost::find(symbols, term) == cend(symbols))
                symbols.push_back(term);
        }

        void collectSymbols(const BasePtr& ptr, std::vector<Var>& symbols)
        {
            if (isSymbol(*ptr))
                insertSymbolIfNotPresent(ptr, symbols);
            else
                for (const auto& operand : ptr->operands())
                    collectSymbols(operand, symbols);
        }
    }
}

tsym::Var tsym::sqrt(const Var& base)
{
    const Var half(1, 2);

    return pow(base, half);
}

tsym::Var tsym::pow(const Var& base, const Var& exp)
{
    return Var{Power::create(base.get(), exp.get())};
}

tsym::Var tsym::log(const Var& arg)
{
    return Var(Logarithm::create(arg.get()));
}

tsym::Var tsym::sin(const Var& arg)
{
    return Var(Trigonometric::createSin(arg.get()));
}

tsym::Var tsym::cos(const Var& arg)
{
    return Var(Trigonometric::createCos(arg.get()));
}

tsym::Var tsym::tan(const Var& arg)
{
    return Var(Trigonometric::createTan(arg.get()));
}

tsym::Var tsym::asin(const Var& arg)
{
    return Var(Trigonometric::createAsin(arg.get()));
}

tsym::Var tsym::acos(const Var& arg)
{
    return Var(Trigonometric::createAcos(arg.get()));
}

tsym::Var tsym::atan(const Var& arg)
{
    return Var(Trigonometric::createAtan(arg.get()));
}

tsym::Var tsym::atan2(const Var& y, const Var& x)
{
    return Var(Trigonometric::createAtan2(y.get(), x.get()));
}

const tsym::Var& tsym::pi()
{
    static const tsym::Var instance(Constant::createPi());

    return instance;
}

const tsym::Var& tsym::euler()
{
    static const tsym::Var instance(Constant::createE());

    return instance;
}

tsym::Var tsym::subst(const Var& arg, const Var& from, const Var& to)
{
    return Var(arg.get()->subst(*from.get(), to.get()));
}

tsym::Var tsym::expand(const Var& arg)
{
    return Var(arg.get()->expand());
}

tsym::Var tsym::simplify(const Var& arg)
/* Currently, only normalization and expansion is tested for the simplest representation. */
{
    using clock = std::chrono::system_clock;
    auto before = clock::now();
    const BasePtr& rep = arg.get();
    const BasePtr expanded(rep->expand());
    BasePtr normalizedLast(rep);
    BasePtr normalizedNext(rep->normal());

    if (isUndefined(*normalizedNext))
        return Var(normalizedNext);

    while (normalizedNext->isDifferent(*normalizedLast)) {
        /* Though it's probably not supposed to happen, there has been an expression that changed
         * upon a second normalization. Most of the time, the first normalization will directly
         * yield the simplest representation. */
        normalizedLast = normalizedNext;
        normalizedNext = normalizedNext->normal();
    }

    const BasePtr& result = normalizedNext->complexity() < expanded->complexity() ? normalizedNext : expanded;

    if (result->isDifferent(*rep)) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - before);

        TSYM_DEBUG("Simplified %S to %S in %.2f ms.", rep, result, static_cast<float>(duration.count()) / 1000.0);
    }

    return Var(result);
}

tsym::Var tsym::diff(const Var& arg, const Var& symbol)
{
    return Var(arg.get()->diff(*symbol.get()));
}

bool tsym::has(const Var& arg, const Var& what)
{
    return arg.get()->has(*what.get());
}

bool tsym::isPositive(const Var& arg)
{
    return arg.get()->isPositive();
}

bool tsym::isNegative(const Var& arg)
{
    return arg.get()->isNegative();
}

unsigned tsym::complexity(const Var& arg)
{
    return arg.get()->complexity();
}

tsym::Var tsym::numerator(const Var& arg)
{
    return normalToFraction(arg.get()).first;
}

tsym::Var tsym::denominator(const Var& arg)
{
    return normalToFraction(arg.get()).second;
}

std::string tsym::name(const Var& arg)
{
    return concat(arg.get()->name());
}

std::vector<tsym::Var> tsym::operands(const Var& arg)
{
    std::vector<Var> ops;

    for (const auto& operand : arg.get()->operands())
        ops.emplace_back(operand);

    return ops;
}

std::vector<tsym::Var> tsym::collectSymbols(const Var& arg)
{
    std::vector<Var> symbols;

    collectSymbols(arg.get(), symbols);

    return symbols;
}

std::optional<tsym::Var> tsym::parse(std::string_view str)
{
    ParseResult result = parseFrom(str);

    TSYM_DEBUG("Parsed '%S' with result: %S", str, result.value);

    if (result.success && result.matchedWholeString)
        return Var(result.value);

    return std::nullopt;
}
