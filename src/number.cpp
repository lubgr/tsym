
#include "number.h"
#include <boost/functional/hash.hpp>
#include <cassert>
#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include "logging.h"
#include "plaintextprintengine.h"
#include "printer.h"

tsym::Number::Number(int value)
    : Number(value, 1)
{}

tsym::Number::Number(int numerator, int denominator)
    : Number(Int(numerator), Int(denominator))
{}

tsym::Number::Number(double value)
    : dValue(value)
{
    tryDoubleToFraction();
    setDebugString();
}

tsym::Number::Number(Int value)
    : Number(std::move(value), 1)
{}

tsym::Number::Number(Int numerator, Int denominator)
    : rational(std::move(numerator), std::move(denominator))
{
    setDebugString();
}

void tsym::Number::setDebugString()
{
#ifdef TSYM_WITH_DEBUG_STRINGS
    std::ostringstream stream;
    PlaintextPrintEngine engine(stream, PlaintextPrintEngine::CharSet::ASCII);

    printer::print(engine, *this);

    prettyStr = stream.str();
#endif
}

void tsym::Number::tryDoubleToFraction()
{
    /* We don't want to have huge fractions everywhere possible, so default number of floating point
     * digits isn't set to a too large value. */
    static const int nFloatDigits = 10000;
    const double roundIncrement = dValue > 0.0 ? 0.5 : -0.5;

    if (dValue > std::numeric_limits<double>::max() / nFloatDigits - roundIncrement / nFloatDigits)
        /* The product for constructing a fraction doesn't fit into a double. */
        return;
    else if (dValue < std::numeric_limits<double>::lowest() / nFloatDigits - roundIncrement / nFloatDigits)
        return;

    auto truncated = Int(dValue * nFloatDigits + roundIncrement);

    if (std::abs(static_cast<double>(truncated) / nFloatDigits - dValue) < ZERO_TOL) {
        /* This will also catch very low double values, which turns them into a rational zero. */
        rational.assign(std::move(truncated), Int(nFloatDigits));
        dValue = 0.0;
    }
}

tsym::Number& tsym::Number::operator+=(const Number& rhs)
{
    if (isThisOrOtherDouble(rhs)) {
        dValue = toDouble() + rhs.toDouble();
        rational = 0;
        tryDoubleToFraction();
    } else
        rational += rhs.rational;

    setDebugString();

    return *this;
}

bool tsym::Number::isThisOrOtherDouble(const Number& other) const
{
    return isDouble() || other.isDouble();
}

tsym::Number& tsym::Number::operator-=(const Number& rhs)
{
    return operator+=(rhs.flipSign());
}

tsym::Number tsym::Number::flipSign() const
{
    Number copy(*this);

    if (isDouble())
        copy.dValue *= -1.0;
    else
        copy.rational *= -1;

    copy.setDebugString();

    return copy;
}

tsym::Number& tsym::Number::operator*=(const Number& rhs)
{
    if (isThisOrOtherDouble(rhs)) {
        dValue = toDouble() * rhs.toDouble();
        rational = 0;
        tryDoubleToFraction();
    } else
        rational *= rhs.rational;

    setDebugString();

    return *this;
}

tsym::Number& tsym::Number::operator/=(const Number& rhs)
{
    static const Number minusOne(-1);

    return operator*=(rhs.toThe(minusOne));
}

const tsym::Number& tsym::Number::operator+() const
{
    return *this;
}

tsym::Number tsym::Number::operator-() const
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
    else
        processRationalPowers(exponent, res);

    return res;
}

bool tsym::Number::processTrivialPowers(const Number& exponent, Number& result) const
/* If the power is evaluated within the block of trivial posssibilities, the second parameter is
 * defined and true is returned, otherwise false. */
{
    if (isZero() && exponent.numerator() < 0) {
        TSYM_ERROR("Number division by zero! Result is undefined.");
        throw std::overflow_error("Zero divisor in rational number division");
    } else if (isZero() || isOne() || exponent.isOne()) {
        result = *this;
        return true;
    } else if (exponent.isZero()) {
        result = Number(1);
        return true;
    } else if (lessThan(0) && !exponent.isInt()) {
        throw std::overflow_error("Illegal power with base zero and non-integer exponent");
    } else if (*this == -1) {
        result = computeMinusOneToThe(exponent);
        return true;
    }

    return false;
}

tsym::Number tsym::Number::computeMinusOneToThe(const Number& exponent) const
{
    assert(exponent.isInt());

    return exponent.numerator() % 2 == 0 ? 1 : -1;
}

bool tsym::Number::processNegBase(const Number& exponent, Number& result) const
{
    Number preFac(-1);

    if (*this > 0)
        return false;

    assert(exponent.isInt());

    preFac = preFac.toThe(exponent);

    result = this->abs().toThe(exponent) * preFac;

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

void tsym::Number::processRationalPowers(const Number& exponent, Number& result) const
{
    /* The base is positive and neither 1 or 0. The exponent is positive or negative. */
    computeNumPower(exponent.numerator(), result);
    computeDenomPower(exponent.denominator(), result);
}

void tsym::Number::computeNumPower(const Int& numExponent, Number& result) const
/* For e.g. (1/2)^(2/3), this does the part (1/2)^2. */
{
    assert(integer::fitsInto<unsigned>(integer::abs(numExponent)));
    const unsigned exp = static_cast<unsigned>(integer::abs(numExponent));
    const Int newDenom = integer::pow(denominator(), exp);
    const Int newNum = integer::pow(numerator(), exp);

    if (numExponent < 0)
        /* The method takes care of negative a numerator. */
        result = Number(newDenom, newNum);
    else
        result = Number(newNum, newDenom);
}

void tsym::Number::computeDenomPower(const Int& denomExponent, Number& result) const
/* For e.g. (1/2)^(2/3), this does the part (1/2)^(1/3), where an attempt is made to resolve the
 * power exactly, i.e., check for simple bases matching the numerator/denominator with the given
 * denominator exponent (e.g. 8^(1/3) = 2). */
{
    const Int numTest = tryGetBase(result.numerator(), denomExponent);
    const Int denomTest = tryGetBase(result.denominator(), denomExponent);

    if (denomExponent == 1)
        return;
    else if (numTest == 0 || denomTest == 0)
        result = Number(std::pow(result.toDouble(), 1.0 / static_cast<double>(denomExponent)));
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
    const double exact = std::pow(static_cast<double>(n), 1.0 / static_cast<double>(denomExponent));
    const int base = static_cast<int>(exact + 0.1);
    Int res(base);

    if (exact > std::numeric_limits<int>::max())
        /* The exact doesn't fit into an int. */
        return 0;
    else if (std::abs(exact - static_cast<double>(base)) > 1.e-6)
        /* We are not too strict here, because of the following check. */
        return 0;

    assert(denomExponent > 0 && integer::fitsInto<unsigned>(denomExponent));

    res = integer::pow(res, static_cast<unsigned>(denomExponent));

    return res == n ? base : 0;
}

bool tsym::Number::equal(const Number& rhs) const
{
    if (areBothRational(rhs))
        return rational == rhs.rational;
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

    return diff < TOL * max;
}

bool tsym::Number::lessThan(const Number& rhs) const
{
    if (areBothRational(rhs))
        return rational < rhs.rational;
    else
        return toDouble() < rhs.toDouble();
}

bool tsym::Number::isZero() const
{
    return rational == 0 && std::abs(dValue) < TOL;
}

bool tsym::Number::isOne() const
{
    return rational == 1;
}

bool tsym::Number::isInt() const
{
    return (numerator() != 0 && denominator() == 1) || isZero();
}

bool tsym::Number::isFrac() const
{
    return denominator() != 1;
}

bool tsym::Number::isRational() const
{
    return isFrac() || isInt();
}

bool tsym::Number::isDouble() const
{
    return rational == 0 && std::abs(dValue) > ZERO_TOL;
}

tsym::Int tsym::Number::numerator() const
{
    return rational.numerator();
}

tsym::Int tsym::Number::denominator() const
{
    return rational.denominator();
}

double tsym::Number::toDouble() const
{
    return isRational() ? boost::rational_cast<double>(rational) : dValue;
}

tsym::Number tsym::Number::abs() const
{
    return *this < 0 ? flipSign() : *this;
}

int tsym::Number::sign() const
{
    if (isZero())
        return 0;
    else
        return *this < 0 ? -1 : 1;
}

bool tsym::operator==(const Number& lhs, const Number& rhs)
{
    return lhs.equal(rhs);
}

bool tsym::operator<(const Number& lhs, const Number& rhs)
{
    return lhs.lessThan(rhs);
}

std::ostream& tsym::operator<<(std::ostream& stream, const Number& rhs)
{
    PlaintextPrintEngine engine(stream);

    printer::print(engine, rhs);

    return stream;
}

size_t std::hash<tsym::Number>::operator()(const tsym::Number& n) const
{
    size_t seed = 0;

    boost::hash_combine(seed, n.toDouble());
    boost::hash_combine(seed, n.denominator());
    boost::hash_combine(seed, n.numerator());

    return seed;
}
