
#include "numeric.h"
#include "symbolmap.h"
#include "logging.h"

tsym::Numeric::Numeric(const Number& number) :
    number(number)
{
    setDebugString();
}

tsym::BasePtr tsym::Numeric::create(int value)
{
    return create(Number(value));
}

tsym::BasePtr tsym::Numeric::create(int numerator, int denominator)
{
    return create(Int(numerator), Int(denominator));
}

tsym::BasePtr tsym::Numeric::create(double value)
{
    return create(Number(value));
}

tsym::BasePtr tsym::Numeric::create(const Int& value)
{
    return create(Number(value));
}

tsym::BasePtr tsym::Numeric::create(const Int& numerator, const Int& denominator)
{
    if (denominator == 0) {
        TSYM_ERROR("Attempt to create a Numeric with zero denominator, result is Undefined");
        return Undefined::create();
    } else
        return create(Number(numerator, denominator));
}

tsym::BasePtr tsym::Numeric::create(const Number& number)
{
    return instantiate([&number]() { return new Numeric(number); });
}

namespace tsym {
    namespace {
        template<int num, int denom = 1> const tsym::BasePtr& refToLocalStatic()
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

bool tsym::Numeric::isEqualDifferentBase(const BasePtr& other) const
{
    if (other->isNumeric())
        return number == other->numericEval();
    else
        return false;
}

bool tsym::Numeric::sameType(const BasePtr& other) const
{
    return other->isNumeric();
}

tsym::Number tsym::Numeric::numericEval() const
{
    return number;
}

tsym::Fraction tsym::Numeric::normal(SymbolMap& map) const
{
    if (number.isRational())
        return Fraction(Numeric::create(number.numerator()), Numeric::create(number.denominator()));
    else
        return Fraction(map.getTmpSymbolAndStore(clone()));
}

tsym::BasePtr tsym::Numeric::diffWrtSymbol(const BasePtr&) const
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
    /* A numeric object must not be instantiated from an undefined Number in the first place: */
    assert(!number.isUndefined());

    if (number.isInt())
        return 1;
    else if(number.isFrac())
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
    return number.isZero();
}

bool tsym::Numeric::isOne() const
{
    return number.isOne();
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

tsym::BasePtr tsym::Numeric::coeff(const BasePtr&, int exp) const
{
    if (exp == 0)
        return clone();
    else
        return Numeric::zero();
}

int tsym::Numeric::degree(const BasePtr&) const
{
    return 0;
}
