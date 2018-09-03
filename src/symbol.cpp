
#include "symbol.h"
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <utility>
#include "cache.h"
#include "fraction.h"
#include "logging.h"
#include "numeric.h"
#include "undefined.h"

unsigned tsym::Symbol::tmpCounter = 0;

tsym::Symbol::Symbol(Name name, bool positive, Base::CtorKey&&)
    : symbolName(std::move(name))
    , positive(positive)
{
    setDebugString();
}

tsym::Symbol::Symbol(unsigned tmpId, bool positive, Base::CtorKey&&)
    : symbolName(std::string(tmpSymbolNamePrefix) + std::to_string(tmpId))
    , positive(positive)
{
    setDebugString();
}

tsym::Symbol::~Symbol()
{
    if (symbolName.getName().find(tmpSymbolNamePrefix) == 0)
        --tmpCounter;
}

tsym::BasePtr tsym::Symbol::create(const std::string& name)
{
    return create(Name(name));
}

tsym::BasePtr tsym::Symbol::create(const Name& name)
{
    return create(name, false);
}

tsym::BasePtr tsym::Symbol::create(const Name& name, bool positive)
{
    if (name.getName().empty()) {
        TSYM_ERROR("Creating Symbol with empty name, return Undefined instead");
        return Undefined::create();
    } else if (name.getName().find(tmpSymbolNamePrefix) == 0) {
        TSYM_ERROR("Instantiation of a non-temporary Symbol containing the temporary name prefix %s,"
                   " return true temporary Symbol",
          name.getName());
        return createTmpSymbol(positive);
    }

    return createNonEmptyName(name, positive);
}

tsym::BasePtr tsym::Symbol::createNonEmptyName(const Name& name, bool positive)
{
    using Key = std::pair<Name, bool>;
    static std::unordered_map<Key, BasePtr, boost::hash<Key>> pool;
    const auto key = std::make_pair(name, positive);

    if (const auto lookup = pool.find(key); lookup != cend(pool))
        return lookup->second;

    return pool.insert({key, std::make_shared<const Symbol>(name, positive, Base::CtorKey{})}).first->second;
}

tsym::BasePtr tsym::Symbol::createPositive(const std::string& name)
{
    return createPositive(Name(name));
}

tsym::BasePtr tsym::Symbol::createPositive(const Name& name)
{
    return create(name, true);
}

tsym::BasePtr tsym::Symbol::createTmpSymbol(bool positive)
{
    ++tmpCounter;

    return std::make_shared<const Symbol>(tmpCounter, positive, Base::CtorKey{});
}

bool tsym::Symbol::isEqualDifferentBase(const Base& other) const
{
    if (other.isSymbol())
        return isEqualOtherSymbol(other);
    else
        return false;
}

bool tsym::Symbol::isEqualOtherSymbol(const Base& other) const
{
    if (symbolName == other.name())
        return positive == other.isPositive();
    else
        return false;
}

bool tsym::Symbol::sameType(const Base& other) const
{
    return other.isSymbol();
}

std::optional<tsym::Number> tsym::Symbol::numericEval() const
{
    return std::nullopt;
}

tsym::Fraction tsym::Symbol::normal(SymbolMap&) const
{
    return Fraction{clone()};
}

tsym::BasePtr tsym::Symbol::diffWrtSymbol(const Base& symbol) const
{
    return isEqual(symbol) ? Numeric::one() : Numeric::zero();
}

std::string tsym::Symbol::typeStr() const
{
    return "Symbol";
}

bool tsym::Symbol::isPositive() const
{
    return positive;
}

bool tsym::Symbol::isNegative() const
{
    return false;
}

size_t tsym::Symbol::hash() const
{
    size_t seed = 0;

    boost::hash_combine(seed, symbolName);
    boost::hash_combine(seed, positive);

    return seed;
}

unsigned tsym::Symbol::complexity() const
{
    return 5;
}

bool tsym::Symbol::isSymbol() const
{
    return true;
}

const tsym::Name& tsym::Symbol::name() const
{
    return symbolName;
}
