
#include "var.h"
#include <map>
#include <stdexcept>
#include "base.h"
#include "basefct.h"
#include "logging.h"
#include "numberfct.h"
#include "numeric.h"
#include "parser.h"
#include "plaintextprintengine.h"
#include "power.h"
#include "printer.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"
#include "undefined.h"

namespace tsym {
    namespace {
        const std::map<std::string_view, Var::Type>& typeStringMap()
        {
            static const std::map<std::string_view, Var::Type> map{{"Sum", Var::Type::SUM},
              {"Product", Var::Type::PRODUCT}, {"Symbol", Var::Type::SYMBOL}, {"Power", Var::Type::POWER},
              {"Constant", Var::Type::CONSTANT}, {"Undefined", Var::Type::UNDEFINED}, {"Function", Var::Type::FUNCTION},
              {"Integer", Var::Type::INT}, {"Fraction", Var::Type::FRACTION}, {"Double", Var::Type::DOUBLE}};

            return map;
        }

        bool isCorrectIntOrSymbol(const ParseResult& parsed)
        {
            const bool parsingSuccess = parsed.success && parsed.matchedWholeString;
            const BasePtr& value(parsed.value);

            if (parsingSuccess)
                return value->isSymbol() || (value->isNumeric() && isInt(*value->numericEval()));

            return false;
        }

        Var::Type numericType(const Number& number)
        {
            if (isInt(number))
                return Var::Type::INT;
            else if (number.isDouble())
                return Var::Type::DOUBLE;
            else if (isFraction(number))
                return Var::Type::FRACTION;

            /* This should never happened, as the BasePtr must be Undefined in the first place. */
            TSYM_ERROR("Illegal number %S in Var!", number);

            return Var::Type::UNDEFINED;
        }
    }
}

tsym::Var::Var()
    : rep(Numeric::zero())
{}

tsym::Var::Var(int value)
    : rep(Numeric::create(value))
{}

tsym::Var::Var(double value)
    : rep(Numeric::create(value))
{}

tsym::Var::Var(int numerator, int denominator)
    : /* Zero denominator is checked inside of the Numeric::create method. */
    rep(Numeric::create(numerator, denominator))
{}

tsym::Var::Var(std::string_view str)
{
    const ParseResult parsed = parseFrom(str);

    if (isCorrectIntOrSymbol(parsed)) {
        rep = parsed.value;
        return;
    }

    TSYM_ERROR("Parsing symbol or integer from '%S' failed, result: %S (%S). "
               "Create undefined Var object.",
      str, parsed.value, Var(parsed.value).type());

    rep = Undefined::create();
}

tsym::Var::Var(std::string_view str, [[maybe_unused]] Var::Sign sign)
{
    const Var withoutSign(str);
    const Type type(withoutSign.type());

    assert(sign == Var::Sign::POSITIVE);

    if (type == Type::SYMBOL) {
        rep = Symbol::createPositive(withoutSign.rep->name());
        return;
    }

    if (type == Type::INT && *withoutSign.rep->numericEval() < 0)
        TSYM_WARNING("Ignore positive flag for negative int (%S)", withoutSign);

    rep = withoutSign.rep;
}

tsym::Var::Var(BasePtr ptr)
    : rep(std::move(ptr))
{}

tsym::Var& tsym::Var::operator+=(const Var& rhs)
{
    rep = Sum::create(rep, rhs.rep);

    return *this;
}

tsym::Var& tsym::Var::operator-=(const Var& rhs)
{
    rep = Sum::create(rep, Product::minus(rhs.rep));

    return *this;
}

tsym::Var& tsym::Var::operator*=(const Var& rhs)
{
    rep = Product::create(rep, rhs.rep);

    return *this;
}

tsym::Var& tsym::Var::operator/=(const Var& rhs)
{
    rep = Product::create(rep, Power::oneOver(rhs.rep));

    return *this;
}

const tsym::Var& tsym::Var::operator+() const
{
    return *this;
}

tsym::Var tsym::Var::operator-() const
{
    return Var(Product::minus(rep));
}

tsym::Var::Type tsym::Var::type() const
{
    if (rep->isNumeric())
        return numericType(*rep->numericEval());

    const auto lookup = typeStringMap().find(rep->typeStr());

    assert(lookup != cend(typeStringMap()));

    return lookup->second;
}

tsym::Var::operator int() const
{
    static const char* errorMessage = "Illegal integer request";
    int result = 0;

    if (!isInteger(*rep))
        throw std::domain_error(errorMessage);
    else if (!fitsInto<int>(rep->numericEval()->numerator()))
        throw std::overflow_error(errorMessage);

    try {
        result = static_cast<int>(rep->numericEval().value().numerator());
    } catch (const std::exception& e) {
        TSYM_ERROR("Conversion from %S to int failed: %s", *this, e.what());
        throw std::domain_error(errorMessage);
    }

    return result;
}

tsym::Var::operator double() const
{
    if (const auto num = rep->numericEval())
        return num->toDouble();

    throw std::domain_error("Illegal conversion to double requested");
}

const tsym::BasePtr& tsym::Var::get() const
{
    return rep;
}

bool tsym::operator==(const Var& lhs, const Var& rhs)
{
    return lhs.get()->isEqual(*rhs.get());
}

bool tsym::operator!=(const Var& lhs, const Var& rhs)
{
    return !(lhs == rhs);
}

tsym::Var tsym::operator+(Var lhs, const Var& rhs)
{
    lhs += rhs;

    return lhs;
}

tsym::Var tsym::operator-(Var lhs, const Var& rhs)
{
    lhs -= rhs;

    return lhs;
}

tsym::Var tsym::operator*(Var lhs, const Var& rhs)
{
    lhs *= rhs;

    return lhs;
}

tsym::Var tsym::operator/(Var lhs, const Var& rhs)
{
    lhs /= rhs;

    return lhs;
}

std::ostream& tsym::operator<<(std::ostream& stream, const Var& rhs)
{
    PlaintextPrintEngine engine(stream);

    print(engine, *rhs.get());

    return stream;
}

std::ostream& tsym::operator<<(std::ostream& stream, const Var::Type& rhs)
{
    for (const auto& [str, type] : typeStringMap())
        if (type == rhs)
            return stream << str;

    TSYM_ERROR("Couldn't find string representation of Var");

    return stream;
}

size_t std::hash<tsym::Var>::operator()(const tsym::Var& var) const
{
    return var.get()->hash();
}
