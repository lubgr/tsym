
#include <cmath>
#include <iostream>
#include <limits>
#include "number.h"
#include "printer.h"
#include "logging.h"

const double tsym::Number::ZERO_TOL = std::numeric_limits<double>::epsilon();
const double tsym::Number::TOL = 100.0*ZERO_TOL;

tsym::Number::Number() :
    undefined(false)
{
    setAndSimplify(0, 1, 0.0);
}

tsym::Number::Number(int value) :
    undefined(false)
{
    setAndSimplify(value, 1, 0.0);
}

tsym::Number::Number(int numerator, int denominator) :
    undefined(false)
{
    setAndSimplify(numerator, denominator, 0.0);
}

tsym::Number::Number(double value) :
    undefined(false)
{
    setAndSimplify(0, 1, value);
}

tsym::Number::Number(const Int& value) :
    undefined(false)
{
    setAndSimplify(value, 1, 0.0);
}

tsym::Number::Number(const Int& numerator, const Int& denominator) :
    undefined(false)
{
    setAndSimplify(numerator, denominator, 0.0);
}

tsym::Number tsym::Number::createUndefined()
{
    Number undefined;

    undefined.setUndefined();

    return undefined;
}

void tsym::Number::setAndSimplify(int num, int denom, double dValue)
{
    setAndSimplify(Int(num), Int(denom), dValue);
}

void tsym::Number::setAndSimplify(const Int& num, const Int& denom, double dValue)
{
    set(num, denom, dValue);

    if (denom == 0)
        logging::error() << "Try to set fraction with zero denominator. Number is undefined.";
    else if (num.hasOverflowed() || denom.hasOverflowed())
        logging::error() << "Initiate Number with overflowed Int arguments! Number is undefined.";
    else {
        simplify();
        return;
    }

    setUndefined();
}

void tsym::Number::set(const Int& num, const Int& denom, double dValue)
{
    if (denom > 0) {
        this->num = num;
        this->denom = denom;
    } else {
        this->num = -num;
        this->denom = -denom;
    }

    this->dValue = dValue;
}

void tsym::Number::setUndefined()
{
    setAndSimplify(0, 1, 0.0);
    undefined = true;
}

void tsym::Number::simplify()
{
    if (isDouble())
        tryDoubleToFraction();

    if (isFrac())
        cancel();

    #ifdef TSYM_DEBUG_STRINGS
    prettyStr = Printer(*this).getStr();
    #endif
}

void tsym::Number::tryDoubleToFraction()
{
    /* We don't want to have huge fractions everywhere possible, so default number of floating point
     * digits isn't set to a too large value. */
    const int nFloatDigits = 10000;
    int truncated;

    if (dValue*nFloatDigits >= std::numeric_limits<int>::max())
        /* Conversion to int would lead to an integer overflow. */
        return;

    truncated = (int)(dValue*nFloatDigits + 0.5);

    if (std::abs((double)truncated/nFloatDigits - dValue) < ZERO_TOL)
        /* This will also catch very low double values, which turns them into a rational zero. */
        setAndSimplify(truncated, nFloatDigits, 0.0);
}

void tsym::Number::cancel()
{
    /* The overload of std::abs doesn't seem to work correctly here, compiler warnings for possibly
     * dangerous implicit conversions demand for the implicit cast to int. */
    const Int divisor(gcd(num, denom).abs());

    if (num == 0)
        denom = 1;

    if (divisor == 0 || divisor > num.abs() || divisor > denom)
        return;

    num /= divisor;
    denom /= divisor;
}

tsym::Int tsym::Number::gcd(const Int& a, const Int& b) const
{
    if (b == 0)
        return a;

    return gcd(b, a % b);
}

tsym::Number& tsym::Number::operator += (const Number& rhs)
{
    if (isThisOrOtherUndefined(rhs))
        setUndefined();
    else if (isThisOrOtherDouble(rhs))
        setAndSimplify(0, 1, toDouble() + rhs.toDouble());
    else
        addRational(rhs);

    return *this;
}

bool tsym::Number::isThisOrOtherUndefined(const Number& other) const
{
    return isUndefined() || other.isUndefined();
}

bool tsym::Number::isThisOrOtherDouble(const Number& other) const
{
    return isDouble() || other.isDouble();
}

void tsym::Number::addRational(const Number& other)
{
    const Int multiple(lcm(denom, other.denom));
    Int newDenom;
    Int newNum;

    if (num == 0 || other.num == 0) {
        newNum = num + other.num;
        newDenom = denom + other.denom - 1;
    } else {
        /* If the lcm is overflowed, the result of this evaluation will be, too. */
        newNum = num*multiple/denom + other.num*multiple/other.denom;
        newDenom = multiple;
    }

    if (newNum.hasOverflowed() || newDenom.hasOverflowed()) {
        printOverflowWarning();
        setAndSimplify(0, 1, toDouble() + other.toDouble());
        return;
    }

    setAndSimplify(newNum, newDenom, 0.0);
}

tsym::Int tsym::Number::lcm(const Int& a, const Int& b) const
    /* Returns the smallest common multiple of the given two integers, This method is only called
     * during addition, the arguments are denominators of two numbers and thus positive. If an
     * integer overflow occurs, this state is ported to the return value. */
{
    const Int divisor(gcd(a, b));

    return a/divisor*b;
}

void tsym::Number::printOverflowWarning() const
{
    logging::warning() << "Integer overflow! Number is converted to double.";
}

tsym::Number& tsym::Number::operator -= (const Number& rhs)
{
    return operator += (rhs.flipSign());
}

tsym::Number tsym::Number::flipSign() const
{
    if (isDouble())
        return Number(-dValue);
    else if (isRational())
        return Number(-num, denom);
    else
        return *this;
}

tsym::Number& tsym::Number::operator *= (const Number& rhs)
{
    if (isThisOrOtherUndefined(rhs))
        setUndefined();
    else if (isThisOrOtherDouble(rhs))
        setAndSimplify(0, 1, toDouble()*rhs.toDouble());
    else
        timesRational(rhs);

    return *this;
}

void tsym::Number::timesRational(const Number& other)
{
    const Int newNum(num*other.num);
    const Int newDenom(denom*other.denom);

    if (newNum.hasOverflowed() || newDenom.hasOverflowed()) {
        printOverflowWarning();
        setAndSimplify(0, 1, toDouble()*other.toDouble());
    } else
        setAndSimplify(newNum, newDenom, 0.0);
}

tsym::Number& tsym::Number::operator /= (const Number& rhs)
{
    static const Number minusOne(-1);

    return operator *= (rhs.toThe(minusOne));
}

const tsym::Number& tsym::Number::operator + () const
{
    return *this;
}

tsym::Number tsym::Number::operator - () const
{
    return flipSign();
}

tsym::Number tsym::Number::toThe(const Number& exponent) const
{
    Number res;

    if (processTrivialPowers(exponent, res))
        return res;
    else if (processNegBase(exponent, res))
        return res;
    else if (processIrrationalPowers(exponent, res))
        return res;
    else if (processRationalPowers(exponent, res))
        return res;

    res = *this;

    res.setAndSimplify(0, 1, std::pow(toDouble(), exponent.toDouble()));

    return res;
}

bool tsym::Number::processTrivialPowers(const Number& exponent, Number& result) const
    /* If the power is evaluated within the block of trivial posssibilities, the second parameter is
     * defined and true is returned, otherwise false. */
{
    if (isThisOrOtherUndefined(exponent)) {
        result = createUndefined();
        return true;
    } else if (isZero() && exponent.num < 0) {
        logging::warning() << "Number division by zero! Result is undefined.";
        result = createUndefined();
        return true;
    } else if (isZero() || isOne() || exponent.isOne()) {
        result = *this;
        return true;
    } else if (exponent.isZero()) {
        result = Number(1);
        return true;
    } else if (*this == -1) {
        result = computeMinusOneToThe(exponent);
        return true;
    }

    return false;
}

tsym::Number tsym::Number::computeMinusOneToThe(const Number& exponent) const
{
    const Int two(2);

    if (exponent.isDouble())
        return createUndefined();
    else if (exponent.num % two == 0)
        return Number(1);
    else if (exponent.denom % two == 0 && num == -1)
        return createUndefined();
    else
        return Number(-1);
}

bool tsym::Number::processNegBase(const Number& exponent, Number& result) const
{
    Number preFac(-1);

    if (*this > 0)
        return false;

    preFac = preFac.toThe(exponent);

    if (preFac.isUndefined()) {
        result = createUndefined();
        return true;
    }

    result = this->abs().toThe(exponent)*preFac;

    return true;
}

bool tsym::Number::processIrrationalPowers(const Number& exponent, Number& result) const
{
    if (isThisOrOtherDouble(exponent)) {
        result = Number(std::pow(toDouble(), exponent.toDouble()));
        return true;
    } else
        return false;
}

bool tsym::Number::processRationalPowers(const Number& exponent, Number& result) const
{
    /* The base is positive and neither 1 or 0. The exponent is positive or negative. */
    if (!computeNumPower(exponent.num, result))
        return false;

    /* We get here only if no integer overflow happened before. computeDenomPower can't produce
     * an integer overflow, so no additional check is required. */
    computeDenomPower(exponent.denom, result);

    return true;
}

bool tsym::Number::computeNumPower(const Int& numExponent, Number& result) const
    /* For e.g. (1/2)^(2/3), this does the part (1/2)^2. This may produce an integer overflow - if
     * it does, the method stops and returns false, otherwise true. */
{
    const Int newDenom(denom.toThe(numExponent.abs()));
    const Int newNum(num.toThe(numExponent.abs()));

    if (newNum.hasOverflowed() || newDenom.hasOverflowed()) {
        printOverflowWarning();
        return false;
    } else if (numExponent < 0)
        /* The method takes care of negative a numerator. */
        result = Number(newDenom, newNum);
    else
        result = Number(newNum, newDenom);

    return true;
}

void tsym::Number::computeDenomPower(const Int& denomExponent, Number& result) const
    /* For e.g. (1/2)^(2/3), this does the part (1/2)^(1/3), where an attempt is made to resolve the
     * power exactly, i.e., check for simple bases matching the numerator/denominator with the given
     * denominator exponent (e.g. 8^(1/3) = 2). */
{
    const Int numTest = tryGetBase(result.num, denomExponent);
    const Int denomTest = tryGetBase(result.denom, denomExponent);

    if (denomExponent == 1)
        return;
    else if (numTest == 0 || denomTest == 0)
        result = Number(std::pow(result.toDouble(), 1.0/denomExponent.toDouble()));
    else
        result = Number(numTest, denomTest);
}

tsym::Int tsym::Number::tryGetBase(const Int& n, const Int& denomExponent) const
    /* Returns a in a^(1/denomExponent) = n, if that's an exact solution. Otherwise, returns 0. n
     * and denomExponent are both positive. */
{
    /* The following integer base can only be a solution, if the double resulting from the std::pow
     * call is more or less exactly an integer. That's why 0.1 is added instead of the usual 0.5 for
     * rounding. The purpose is to avoid truncation errors due to e.g. (int)(6.000001 + 0.5) = 7. On
     * the other hand, (int)(5.50001 + 0.5) = 6 isn't a desired result, because 5.50001 will never
     * lead to an exact integer solution of the power. Thus, it is save to cast the double power
     * result to an integer after only adding 0.1 (which is somewhat arbitrary, could be any value
     * less than 0.5). */
    const double exact = std::pow(n.toDouble(), 1.0/denomExponent.toDouble());
    const int base = (int)(exact + 0.1);
    Int res(base);

    if (exact > std::numeric_limits<int>::max())
        /* The exact doesn't fit into an int. */
        return 0;
    else if (std::abs(exact - (double)base) > 1.e-6)
        /* We are not too strict here, because of the following check. */
        return 0;

    res = res.toThe(denomExponent);

    if (res.hasOverflowed())
        /* This shouldn't happen, it should have been overflowed before. */
        return 0;
    else
        return res == n ? base : 0;
}

bool tsym::Number::equal(const Number& rhs) const
{
    if (areBothRational(rhs))
        return num == rhs.num && denom == rhs.denom;
    else if (isThisOrOtherUndefined(rhs))
        return false;
    else
        return equalViaDouble(rhs);
}

bool tsym::Number::areBothRational(const Number& other) const
{
    return isRational() && other.isRational();
}

bool tsym::Number::equalViaDouble(const Number& rhs) const
{
    double dLhs = toDouble();
    double dRhs = rhs.toDouble();
    const double diff = std::abs(dLhs - dRhs);
    double max = 1.0;

    dLhs = std::abs(dLhs);
    dRhs = std::abs(dRhs);

    max = dLhs > max ? dLhs : max;
    max = dRhs > max ? dRhs : max;

    return diff < TOL*max;
}

bool tsym::Number::lessThan(const Number& rhs) const
{
    if (isThisOrOtherUndefined(rhs))
        return false;
    else if (areBothRational(rhs))
        if (num < rhs.num && denom >= rhs.denom)
            return true;

    return toDouble() < rhs.toDouble();
}

bool tsym::Number::isZero() const
{
    if (isInt() || isFrac())
        return num == 0;
    else if (isDouble())
        return std::abs(dValue) < TOL;
    else if (isUndefined())
        return false;

    logging::error() << "Unknown number type " << *this << " during zero request.";

    return false;
}

bool tsym::Number::isOne() const
{
    return isInt() && num == 1 && !undefined;
}

bool tsym::Number::isInt() const
{
    return denom == 1 && std::abs(dValue) < ZERO_TOL && !undefined;
}

bool tsym::Number::isFrac() const
{
    return denom != 1 && !undefined;
}

bool tsym::Number::isRational() const
{
    return isInt() || isFrac();
}

bool tsym::Number::isDouble() const
{
    return num == 0 && std::abs(dValue) > ZERO_TOL && !undefined;
}

bool tsym::Number::isUndefined() const
{
    return undefined;
}

const tsym::Int& tsym::Number::numerator() const
{
    return num;
}

const tsym::Int& tsym::Number::denominator() const
{
    return denom;
}

double tsym::Number::toDouble() const
{
    if (isInt())
        return num.toDouble();
    else if (isFrac())
        return num.toDouble()/denom.toDouble();
    else
        return dValue;
}

tsym::Number tsym::Number::abs() const
{
    return *this < 0 ? flipSign() : *this;
}

int tsym::Number::sign() const
{
    return *this < 0 ? -1 : 1;
}

tsym::Number tsym::Number::Sqrt(const Number& base)
{
    const Number half(1, 2);

    return Pow(base, half);
}

tsym::Number tsym::Number::Pow(const Number& base, const Number& exp)
{
    return base.toThe(exp);
}

bool tsym::operator == (const Number& lhs, const Number& rhs)
{
    return lhs.equal(rhs);
}

bool tsym::operator != (const Number& lhs, const Number& rhs)
{
    return !lhs.equal(rhs);
}

bool tsym::operator < (const Number& lhs, const Number& rhs)
{
    return lhs.lessThan(rhs);
}

bool tsym::operator <= (const Number& lhs, const Number& rhs)
{
    return !(lhs > rhs);
}

bool tsym::operator > (const Number& lhs, const Number& rhs)
{
    return rhs < lhs;
}

bool tsym::operator >= (const Number& lhs, const Number& rhs)
{
    return !(lhs < rhs);
}

tsym::Number tsym::operator + (Number lhs, const Number& rhs)
{
    lhs += rhs;

    return lhs;
}

tsym::Number tsym::operator - (Number lhs, const Number& rhs)
{
    lhs -= rhs;

    return lhs;
}

tsym::Number tsym::operator * (Number lhs, const Number& rhs)
{
    lhs *= rhs;

    return lhs;
}

tsym::Number tsym::operator / (Number lhs, const Number& rhs)
{
    lhs /= rhs;

    return lhs;
}

std::ostream& tsym::operator << (std::ostream& stream, const Number& rhs)
{
    Printer printer(rhs);

    printer.print(stream);

    return stream;
}
