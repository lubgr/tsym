
#include "fraction.h"
#include "numeric.h"
#include "product.h"
#include "power.h"
#include "poly.h"
#include "logging.h"

tsym::Fraction::Fraction() :
    numerator(Undefined::create()),
    denominator(Numeric::one())
{}

tsym::Fraction::Fraction(const BasePtr& numerator) :
    numerator(numerator),
    denominator(Numeric::one())
{}

tsym::Fraction::Fraction(const BasePtr& numerator, const BasePtr& denominator) :
    numerator(numerator),
    denominator(denominator)
{}

tsym::BasePtr tsym::Fraction::eval() const
{
    return Product::create(numerator, Power::oneOver(denominator));
}

tsym::Fraction tsym::Fraction::invert() const
{
    return Fraction(denominator, numerator);
}

tsym::Fraction tsym::Fraction::cancel() const
{
    if (denominator->isOne())
        return Fraction(numerator);
    else if (numerator->isZero())
        return Fraction(numerator);
    else if (denominator->expand()->isZero()) {
        logging::warning() << "Zero denominator encountered during fraction cacellation";
        return Fraction(Undefined::create());
    } else
        return cancelNonTrivial();
}

tsym::Fraction tsym::Fraction::cancelNonTrivial() const
{
    const BasePtr gcd(poly::gcd(numerator, denominator));
    const BasePtr newDenom(poly::divide(denominator, gcd).front());
    const BasePtr newNum(poly::divide(numerator, gcd).front());

    return Fraction(newNum, newDenom);
}

const tsym::BasePtr& tsym::Fraction::num() const
{
    return numerator;
}

const tsym::BasePtr& tsym::Fraction::denom() const
{
    return denominator;
}
