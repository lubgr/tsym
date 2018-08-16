
#include "numeric.h"
#include "fraction.h"
#include "numberfct.h"
#include "symbolmap.h"

tsym::Numeric::Numeric(Number&& number, Base::CtorKey&&)
    : number(std::move(number))
{
    setDebugString();
}

tsym::BasePtr tsym::Numeric::create(Number number)
{
    return std::make_shared<const Numeric>(std::move(number), Base::CtorKey{});
}

namespace tsym {
    namespace {
        template <int num, int denom = 1> const tsym::BasePtr& refToLocalStatic()
        {
            static const auto n = Numeric::create(num, denom);

            return n;
        }
    }
}

const tsym::BasePtr& tsym::Numeric::zero()
{
    return refToLocalStatic<0>();
}

const tsym::BasePtr& tsym::Numeric::one()
{
    return refToLocalStatic<1>();
}

const tsym::BasePtr& tsym::Numeric::two()
{
    return refToLocalStatic<2>();
}

const tsym::BasePtr& tsym::Numeric::three()
{
    return refToLocalStatic<3>();
}

const tsym::BasePtr& tsym::Numeric::four()
{
    return refToLocalStatic<4>();
}

const tsym::BasePtr& tsym::Numeric::half()
{
    return refToLocalStatic<1, 2>();
}

const tsym::BasePtr& tsym::Numeric::third()
{
    return refToLocalStatic<1, 3>();
}

const tsym::BasePtr& tsym::Numeric::fourth()
{
    return refToLocalStatic<1, 4>();
}

const tsym::BasePtr& tsym::Numeric::mOne()
{
    return refToLocalStatic<-1>();
}

bool tsym::Numeric::isEqualDifferentBase(const Base& other) const
{
    if (other.isNumeric())
        return number == other.numericEval();
    else
        return false;
}

bool tsym::Numeric::sameType(const Base& other) const
{
    return other.isNumeric();
}

tsym::Number tsym::Numeric::numericEval() const
{
    return number;
}

tsym::Fraction tsym::Numeric::normal(SymbolMap& map) const
{
    if (isRational(number))
        return Fraction{Numeric::create(number.numerator()), Numeric::create(number.denominator())};
    else
        return Fraction{map.getTmpSymbolAndStore(clone())};
}

tsym::BasePtr tsym::Numeric::diffWrtSymbol(const Base&) const
{
    return zero();
}

std::string tsym::Numeric::typeStr() const
{
    return "Numeric";
}

bool tsym::Numeric::isPositive() const
{
    return number > 0;
}

bool tsym::Numeric::isNegative() const
{
    return number < 0;
}

size_t tsym::Numeric::hash() const
{
    return std::hash<Number>{}(number);
}

unsigned tsym::Numeric::complexity() const
{
    if (isInt(number))
        return 1;
    else if (isFraction(number))
        return 2;
    else
        return 3;
}

bool tsym::Numeric::isNumericallyEvaluable() const
{
    return true;
}

bool tsym::Numeric::isNumeric() const
{
    return true;
}

bool tsym::Numeric::isZero() const
{
    return number == 0;
}

bool tsym::Numeric::isOne() const
{
    return number == 1;
}

bool tsym::Numeric::isConst() const
{
    return true;
}

tsym::BasePtr tsym::Numeric::numericTerm() const
{
    return clone();
}

tsym::BasePtr tsym::Numeric::nonNumericTerm() const
{
    return create(1);
}

tsym::BasePtr tsym::Numeric::constTerm() const
{
    return clone();
}

tsym::BasePtr tsym::Numeric::nonConstTerm() const
{
    return create(1);
}

tsym::BasePtr tsym::Numeric::coeff(const Base&, int exp) const
{
    if (exp == 0)
        return clone();
    else
        return Numeric::zero();
}

int tsym::Numeric::degree(const Base&) const
{
    return 0;
}
