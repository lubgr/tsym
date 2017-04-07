
#include "numeric.h"
#include "symbolmap.h"

tsym::Numeric::Numeric(const Number& number) :
    number(number)
{
    setDebugString();
}

tsym::Numeric::~Numeric() {}

tsym::BasePtr tsym::Numeric::create(int value)
{
    return create(Number(value));
}

tsym::BasePtr tsym::Numeric::create(int numerator, int denominator)
{
    return create(Number(numerator, denominator));
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
    return create(Number(numerator, denominator));
}

tsym::BasePtr tsym::Numeric::create(const Number& number)
{
    if (number.isUndefined())
        return Undefined::create();
    else
        return BasePtr(new Numeric(number));
}

const tsym::BasePtr& tsym::Numeric::zero()
{
    static const BasePtr n(create(0));

    return n;
}

const tsym::BasePtr& tsym::Numeric::one()
{
    static const BasePtr n(create(1));

    return n;
}

const tsym::BasePtr& tsym::Numeric::mOne()
{
    static const BasePtr n(create(-1));

    return n;
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

unsigned int tsym::Numeric::complexity() const
{
    if (number.isInt())
    {
         return 1;
    }else if(number.isFrac())
    {
        return 2;
    }else if(number.isDouble())
    {
        return 3;
    }
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
