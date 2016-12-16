
#include <algorithm>
#include <cstring>
#include "var.h"
#include "symbol.h"
#include "numeric.h"
#include "sum.h"
#include "product.h"
#include "power.h"
#include "printer.h"
#include "fraction.h"
#include "symbolmap.h"
#include "logging.h"
#include "globals.h"

namespace tsym {
    namespace {
        Name parseNonEmptySymbolName(const char *origName)
        {
            Var tmp;
            const bool success = stringToVar(origName, tmp);

            if (success && tmp.getBasePtr()->isSymbol())
                return tmp.getBasePtr()->name();

            logging::error() << "Parsing a symbol from '" << origName << "' failed, result: "
                << tmp << " (" << tmp.type() << "). Use '" << origName << "' as a symbol name";

            return Name(origName);
        }

        Name parseSymbolName(const char *origName)
        {
            if (strlen(origName) != 0)
                return parseNonEmptySymbolName(origName);

            logging::error() << "A symbol can't be instantiated with an empty string!";

            return Name("");
        }
    }
}

tsym::Var::Var() :
    rep(new BasePtr(Numeric::zero()))
{}

tsym::Var::Var(int value) :
    rep(new BasePtr(Numeric::create(value)))
{}

tsym::Var::Var(double value) :
    rep(new BasePtr(Numeric::create(value)))
{}

tsym::Var::Var(int numerator, int denominator) :
    /* Zero denominator is checked inside of the Numeric::create method. */
    rep(new BasePtr(Numeric::create(numerator, denominator)))
{}

tsym::Var::Var(const char *name)
{
    rep = new BasePtr(Symbol::create(parseSymbolName(name)));
}

tsym::Var::Var(const char *name, Var::Sign sign)
{
    assert(sign == Var::POSITIVE);

    /* To avoid an unused variable warning: */
    (void)sign;

    rep = new BasePtr(Symbol::createPositive(parseSymbolName(name)));
}

tsym::Var::Var(const BasePtr& ptr) :
    rep(new BasePtr(ptr))
{}

tsym::Var::Var(const Var& other) :
    rep(new BasePtr(*other.rep))
{}

const tsym::Var& tsym::Var::operator = (const Var& rhs)
{
    if (this == &rhs)
        return *this;

    delete rep;

    rep = new BasePtr(*rhs.rep);

    return *this;
}

tsym::Var::~Var()
{
    delete rep;
}

tsym::Var& tsym::Var::operator += (const Var& rhs)
{
    const BasePtr *ref(rep);

    rep = new BasePtr(Sum::create(*rep, *rhs.rep));

    delete ref;

    return *this;
}

tsym::Var& tsym::Var::operator -= (const Var& rhs)
{
    const BasePtr *ref(rep);

    rep = new BasePtr(Sum::create(*rep, Product::minus(*rhs.rep)));

    delete ref;

    return *this;
}

tsym::Var& tsym::Var::operator *= (const Var& rhs)
{
    const BasePtr *ref(rep);

    rep = new BasePtr(Product::create(*rep, *rhs.rep));

    delete ref;

    return *this;
}

tsym::Var& tsym::Var::operator /= (const Var& rhs)
{
    const BasePtr *ref(rep);

    rep = new BasePtr(Product::create(*rep, Power::oneOver(*rhs.rep)));

    delete ref;

    return *this;
}

const tsym::Var& tsym::Var::operator + () const
{
    return *this;
}

tsym::Var tsym::Var::operator - () const
{
    return Var(Product::minus(*rep));
}

tsym::Var tsym::Var::toThe(const Var& exponent) const
{
    return Var(Power::create(*rep, *exponent.rep));
}

tsym::Var tsym::Var::subst(const Var& from, const Var& to) const
{
    return Var((*rep)->subst(*from.rep, *to.rep));
}

tsym::Var tsym::Var::expand() const
{
    return Var((*rep)->expand());
}

tsym::Var tsym::Var::normal() const
{
    const time_t start = time(NULL);
    const BasePtr normalized((*rep)->normal());

    if (!normalized->isEqual(*rep))
        logging::debug() << "Normalized " << *rep << " to " << normalized << " in " <<
            difftime(time(NULL), start) << " s.";

    return Var(normalized);
}

tsym::Var tsym::Var::diff(const Var& symbol) const
{
    return Var((*rep)->diff(*symbol.rep));
}

bool tsym::Var::equal(const Var& other) const
{
    return (*rep)->isEqual(*other.rep);
}

bool tsym::Var::has(const Var& other) const
{
    return (*rep)->has(*other.rep);
}

bool tsym::Var::isZero() const
{
    return (*rep)->isZero();
}

bool tsym::Var::isPositive() const
{
    return (*rep)->isPositive();
}

bool tsym::Var::isNegative() const
{
    return (*rep)->isNegative();
}

std::string tsym::Var::type() const
{
    if ((*rep)->isNumeric())
        return numericType();
    else
        return (*rep)->typeStr();
}

std::string tsym::Var::numericType() const
{
    const Number number((*rep)->numericEval());

    if (number.isInt())
        return "Integer";
    else if (number.isDouble())
        return "Double";
    else if (number.isFrac())
        return "Fraction";

    /* This should never happened, as the BasePtr must be Undefined in the first place. */
    logging::error() << "Illegal number " << number << " in Var!";

    return "Undefined";
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

    normalizedFrac = (*rep)->normal(map);

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
        return (*rep)->numericEval().numerator().fitsIntoInt();
    else
        return false;
}

bool tsym::Var::isInteger() const
{
    return (*rep)->isNumeric() && (*rep)->numericEval().isInt();
}

int tsym::Var::toInt() const
{
    if (!isInteger())
        logging::error() << "Requesting integer from " << type();

    return (*rep)->numericEval().numerator().toInt();
}

double tsym::Var::toDouble() const
{
    assert((*rep)->isNumeric());

    return (*rep)->numericEval().toDouble();
}

const std::string& tsym::Var::name() const
{
    return (*rep)->name().plain();
}

std::vector<tsym::Var> tsym::Var::operands() const
{
    BasePtrList::const_iterator it;
    std::vector<Var> ops;

    for (it = (*rep)->operands().begin(); it != (*rep)->operands().end(); ++it)
        ops.push_back(Var(*it));

    return ops;
}

std::vector<tsym::Var> tsym::Var::collectSymbols() const
{
    std::vector<Var> symbols;

    collectSymbols(*rep, symbols);

    return symbols;
}

void tsym::Var::collectSymbols(const BasePtr& ptr, std::vector<Var>& symbols) const
{
    BasePtrList::const_iterator it;

    if (ptr->isSymbol())
        insertSymbolIfNotPresent(ptr, symbols);
    else
        for (it = ptr->operands().begin(); it != ptr->operands().end(); ++it)
            collectSymbols(*it, symbols);
}

void tsym::Var::insertSymbolIfNotPresent(const BasePtr& symbol, std::vector<Var>& symbols) const
{
    const Var term(symbol);

    if (std::find(symbols.begin(), symbols.end(), term) == symbols.end())
        symbols.push_back(term);
}

const tsym::BasePtr& tsym::Var::getBasePtr() const
{
    return *rep;
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
    Printer printer(var);

    printer.print(stream);

    return stream;
}
