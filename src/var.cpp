
#include "var.h"
#include "symbol.h"
#include "numeric.h"
#include "sum.h"
#include "product.h"
#include "power.h"
#include "printer.h"
#include "logging.h"

tsym::Var::Var() :
    basePtr(Numeric::zero())
{}

tsym::Var::Var(int value) :
    basePtr(Numeric::create(value))
{}

tsym::Var::Var(double value) :
    basePtr(Numeric::create(value))
{}

tsym::Var::Var(int numerator, int denominator) :
    /* Zero denominator is checked inside of the Numeric::create method. */
    basePtr(Numeric::create(numerator, denominator))
{}

tsym::Var::Var(const Number& number) :
    /* An undefined number is again checked inside of the Numeric::create method. */
    basePtr(Numeric::create(number))
{}

tsym::Var::Var(const char *name, const char *subscript, const char *superscript) :
    basePtr(Symbol::create(Name(name, subscript, superscript)))
{}

tsym::Var::Var(const BasePtr& ptr) :
    basePtr(ptr)
{}

tsym::Var& tsym::Var::operator += (const Var& rhs)
{
    basePtr = Sum::create(basePtr, rhs.basePtr);

    return *this;
}

tsym::Var& tsym::Var::operator -= (const Var& rhs)
{
    basePtr = Sum::create(basePtr, Product::minus(rhs.basePtr));

    return *this;
}

tsym::Var& tsym::Var::operator *= (const Var& rhs)
{
    basePtr = Product::create(basePtr, rhs.basePtr);

    return *this;
}

tsym::Var& tsym::Var::operator /= (const Var& rhs)
{
    basePtr = Product::create(basePtr, Power::oneOver(rhs.basePtr));

    return *this;
}

const tsym::Var& tsym::Var::operator + () const
{
    return *this;
}

tsym::Var tsym::Var::operator - () const
{
    return Var(Product::minus(basePtr));
}

tsym::Var tsym::Var::toThe(const Var& exponent) const
{
    Var res;

    res.basePtr = Power::create(basePtr, exponent.basePtr);

    return res;
}

tsym::Var tsym::Var::subst(const Var& from, const Var& to) const
{
    return Var(basePtr->subst(from.basePtr, to.basePtr));
}

tsym::Var tsym::Var::expand() const
{
    return Var(basePtr->expand());
}

tsym::Var tsym::Var::normal() const
{
    const time_t start = time(NULL);
    const BasePtr bp(basePtr->normal());

    if (!bp->isEqual(basePtr))
        logging::info() << "Normalized " << basePtr << " to " << bp << " in " <<
            difftime(time(NULL), start) << " s.";

    return Var(bp);
}

tsym::Var tsym::Var::diff(const Var& symbol) const
{
    return Var(basePtr->diff(symbol.basePtr));
}

bool tsym::Var::equal(const Var& other) const
{
    return basePtr->isEqual(other.basePtr);
}

bool tsym::Var::has(const Var& other) const
{
    return basePtr->has(other.basePtr);
}

bool tsym::Var::isZero() const
{
    return basePtr->isZero();
}

bool tsym::Var::isNumericallyEvaluable() const
{
    return basePtr->isNumericallyEvaluable();
}

tsym::Number tsym::Var::numericEval() const
{
    /* A check for numerical evaluability is performed inside of the method called. */
    return basePtr->numericEval();
}

std::string tsym::Var::type() const
{
    return basePtr->typeStr();
}

const std::string& tsym::Var::name() const
{
    return basePtr->name().getName();
}

const std::string& tsym::Var::subscript() const
{
    return basePtr->name().getSubscript();
}

const std::string& tsym::Var::superscript() const
{
    return basePtr->name().getSuperscript();
}

std::list<tsym::Var> tsym::Var::operands() const
{
    BasePtrList::const_iterator it;
    std::list<Var> ops;

    for (it = basePtr->operands().begin(); it != basePtr->operands().end(); ++it)
        ops.push_back(Var(*it));

    return ops;
}

const tsym::BasePtr& tsym::Var::getBasePtr() const
{
    return basePtr;
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
