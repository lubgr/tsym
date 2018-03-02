
#include <algorithm>
#include <chrono>
#include <map>
#include <cstring>
#include "var.h"
#include "symbol.h"
#include "numeric.h"
#include "sum.h"
#include "undefined.h"
#include "product.h"
#include "power.h"
#include "printer.h"
#include "plaintextprintengine.h"
#include "fraction.h"
#include "symbolmap.h"
#include "logging.h"
#include "globals.h"

namespace tsym {
    namespace {
        const std::map<std::string, Var::Type>& typeStringMap()
        {
            static const std::map<std::string, Var::Type> map {{ "Sum", Var::Type::SUM },
                { "Product", Var::Type::PRODUCT }, { "Symbol", Var::Type::SYMBOL },
                { "Power", Var::Type::POWER }, { "Constant", Var::Type::CONSTANT },
                { "Undefined", Var::Type::UNDEFINED}, { "Function", Var::Type::FUNCTION },
                { "Integer", Var::Type::INT }, { "Fraction", Var::Type::FRACTION },
                { "Double", Var::Type::DOUBLE }};

            return map;
        }
    }
}

tsym::Var::Var() :
    rep(Numeric::zero())
{}

tsym::Var::Var(int value) :
    rep(Numeric::create(value))
{}

tsym::Var::Var(double value) :
    rep(Numeric::create(value))
{}

tsym::Var::Var(int numerator, int denominator) :
    /* Zero denominator is checked inside of the Numeric::create method. */
    rep(Numeric::create(numerator, denominator))
{}

tsym::Var::Var(const char *str)
{
    bool success;
    const Var tmp(parse(str, &success));

    if (success && (tmp.type() == Type::SYMBOL || tmp.type() == Type::INT)) {
        rep = tmp.rep;
        return;
    }

    TSYM_ERROR("Parsing symbol or integer from '%s' failed, result: %S (%S). "
            "Create undefined Var object.", str, tmp, tmp.type());

    rep = Undefined::create();
}

tsym::Var::Var(const char *str, Var::Sign sign)
{
    const Var withoutSign(str);
    const Type type(withoutSign.type());

    assert(sign == Var::Sign::POSITIVE);

    /* To avoid an unused variable warning: */
    (void) sign;

    if (type == Type::SYMBOL) {
        rep = Symbol::createPositive(withoutSign.rep->name());
        return;
    }

    if (type == Type::INT && withoutSign.rep->numericEval() < 0)
        TSYM_WARNING("Ignore positive flag for negative int (%S)", withoutSign);

    rep = withoutSign.rep;
}

tsym::Var::Var(const BasePtr& ptr) :
    rep(ptr)
{}

tsym::Var& tsym::Var::operator += (const Var& rhs)
{
    rep = Sum::create(rep, rhs.rep);

    return *this;
}

tsym::Var& tsym::Var::operator -= (const Var& rhs)
{
    rep = Sum::create(rep, Product::minus(rhs.rep));

    return *this;
}

tsym::Var& tsym::Var::operator *= (const Var& rhs)
{
    rep = Product::create(rep, rhs.rep);

    return *this;
}

tsym::Var& tsym::Var::operator /= (const Var& rhs)
{
    rep = Product::create(rep, Power::oneOver(rhs.rep));

    return *this;
}

const tsym::Var& tsym::Var::operator + () const
{
    return *this;
}

tsym::Var tsym::Var::operator - () const
{
    return Var(Product::minus(rep));
}

tsym::Var tsym::Var::toThe(const Var& exponent) const
{
    return Var(Power::create(rep, exponent.rep));
}

tsym::Var tsym::Var::subst(const Var& from, const Var& to) const
{
    return Var(rep->subst(from.rep, to.rep));
}

tsym::Var tsym::Var::expand() const
{
    return Var(rep->expand());
}

tsym::Var tsym::Var::normal() const
{
    auto ts = std::chrono::high_resolution_clock::now();
    const auto normalized(rep->normal());
    std::chrono::microseconds ms;
    decltype(ts) te;

    te = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::microseconds>(te - ts);

    if (!normalized->isEqual(rep))
        TSYM_DEBUG("Normalized %S to %S in %.2f ms.", rep, normalized,
                static_cast<float>(ms.count())/1000.0);

    return Var(normalized);
}

tsym::Var tsym::Var::simplify() const
/* Currently, only normalization and expansion is tested for the simplest representation. */
{
    const BasePtr expanded(rep->expand());
    BasePtr normalizedLast(rep);
    BasePtr normalizedNext(rep->normal());

    if (normalizedNext->isUndefined())
        return Var(normalizedNext);

    while (normalizedNext->isDifferent(normalizedLast)) {
        /* Though it's probably not supposed to happen, there has been an expression that changed
         * upon a second normalization. Most of the time, the first normalization will directly
         * yield the simplest representation. */
        normalizedLast = normalizedNext;
        normalizedNext = normalizedNext->normal();
    }

    return normalizedNext->complexity() < expanded->complexity() ? Var(normalizedNext) : Var(expanded);
}

tsym::Var tsym::Var::diff(const Var& symbol) const
{
    return Var(rep->diff(symbol.rep));
}

bool tsym::Var::equal(const Var& other) const
{
    return rep->isEqual(other.rep);
}

bool tsym::Var::has(const Var& other) const
{
    return rep->has(other.rep);
}

bool tsym::Var::isZero() const
{
    return rep->isZero();
}

bool tsym::Var::isPositive() const
{
    return rep->isPositive();
}

bool tsym::Var::isNegative() const
{
    return rep->isNegative();
}

tsym::Var::Type tsym::Var::type() const
{
    if (rep->isNumeric())
        return numericType();

    const auto lookup = typeStringMap().find(rep->typeStr());

    assert(lookup != cend(typeStringMap()));

    return lookup->second;
}

unsigned tsym::Var::complexity() const
{
    return rep->complexity();
}

tsym::Var::Type tsym::Var::numericType() const
{
    const Number number(rep->numericEval());

    if (number.isInt())
        return Type::INT;
    else if (number.isDouble())
        return Type::DOUBLE;
    else if (number.isFrac())
        return Type::FRACTION;

    /* This should never happened, as the BasePtr must be Undefined in the first place. */
    TSYM_ERROR("Illegal number %S in Var!", number);

    return Type::UNDEFINED;
}

tsym::Var tsym::Var::numerator() const
{
    return normalToFraction().first;
}

std::pair<tsym::Var, tsym::Var> tsym::Var::normalToFraction() const
{
    Fraction normalizedFrac;
    SymbolMap map;
    BasePtr denom;
    BasePtr num;

    normalizedFrac = rep->normal(map);

    denom = map.replaceTmpSymbolsBackFrom(normalizedFrac.denom());
    num = map.replaceTmpSymbolsBackFrom(normalizedFrac.num());

    return std::make_pair(Var(num), Var(denom));
}

tsym::Var tsym::Var::denominator() const
{
    return normalToFraction().second;
}

bool tsym::Var::fitsIntoInt() const
{
    if (isInteger())
        return integer::fitsInto<int>(rep->numericEval().numerator());
    else
        return false;
}

bool tsym::Var::isInteger() const
{
    return rep->isNumeric() && rep->numericEval().isInt();
}

int tsym::Var::toInt() const
{
    int result = 0;

    if (!isInteger())
        TSYM_ERROR("Requesting integer from %S", type());

    try {
        result = static_cast<int>(rep->numericEval().numerator());
    } catch (const std::exception& e){
        TSYM_ERROR("Conversion from %S to int failed: %s", *this, e.what());
    }

    return result;
}

double tsym::Var::toDouble() const
{
    assert(rep->isNumeric());

    return rep->numericEval().toDouble();
}

const std::string& tsym::Var::name() const
{
    return rep->name().plain();
}

std::vector<tsym::Var> tsym::Var::operands() const
{
    std::vector<Var> ops;

    for (const auto& operand : rep->operands())
        ops.push_back(Var(operand));

    return ops;
}

std::vector<tsym::Var> tsym::Var::collectSymbols() const
{
    std::vector<Var> symbols;

    collectSymbols(rep, symbols);

    return symbols;
}

void tsym::Var::collectSymbols(const BasePtr& ptr, std::vector<Var>& symbols) const
{
    if (ptr->isSymbol())
        insertSymbolIfNotPresent(ptr, symbols);
    else
        for (const auto& operand : ptr->operands())
            collectSymbols(operand, symbols);
}

void tsym::Var::insertSymbolIfNotPresent(const BasePtr& symbol, std::vector<Var>& symbols) const
{
    const Var term(symbol);

    if (std::find(cbegin(symbols), cend(symbols), term) == cend(symbols))
        symbols.push_back(term);
}

const tsym::BasePtr& tsym::Var::getBasePtr() const
{
    return rep;
}

bool tsym::operator == (const Var& lhs, const Var& rhs)
{
    return lhs.equal(rhs);
}

bool tsym::operator != (const Var& lhs, const Var& rhs)
{
    return !lhs.equal(rhs);
}

tsym::Var tsym::operator + (Var lhs, const Var& rhs)
{
    lhs += rhs;

    return lhs;
}

tsym::Var tsym::operator - (Var lhs, const Var& rhs)
{
    lhs -= rhs;

    return lhs;
}

tsym::Var tsym::operator * (Var lhs, const Var& rhs)
{
    lhs *= rhs;

    return lhs;
}

tsym::Var tsym::operator / (Var lhs, const Var& rhs)
{
    lhs /= rhs;

    return lhs;
}

std::ostream& tsym::operator << (std::ostream& stream, const Var& var)
{
    PlaintextPrintEngine engine(stream);

    printer::print(engine, var.getBasePtr());

    return stream;
}

std::ostream& tsym::operator << (std::ostream& stream, const Var::Type& type)
{
    for (const auto& it : typeStringMap())
        if (it.second == type) {
            stream << it.first;
            return stream;
        }

    TSYM_ERROR("Couldn't find string representation of Var");

    return stream;
}

size_t std::hash<tsym::Var>::operator () (const tsym::Var& var) const
{
    return var.getBasePtr()->hash();
}
