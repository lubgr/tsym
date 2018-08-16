
#include "number.h"
#include <boost/functional/hash.hpp>
#include <cassert>
#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include "logging.h"
#include "numberfct.h"
#include "plaintextprintengine.h"
#include "printer.h"

tsym::Number::Number(int value)
    : Number(value, 1)
{}

tsym::Number::Number(int numerator, int denominator)
    : Number(Int(numerator), Int(denominator))
{}

tsym::Number::Number(double value)
    : rep(value)
{
    tryDoubleToFraction();
    setDebugString();
}

tsym::Number::Number(const Int& value)
    : Number(value, 1)
{}

tsym::Number::Number(const Int& numerator, const Int& denominator)
    /* The implementation doesn't move from input rvalues, hence const references are fine here: */
    : rep(std::in_place_type_t<Rational>(), denominator < 0 ? -numerator : numerator, integer::abs(denominator))
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
    assert(std::holds_alternative<double>(rep));
    /* We don't want to have huge fractions everywhere possible, so default number of floating point
     * digits isn't set to a too large value. */
    static const int nFloatDigits = 10000;
    const auto value = std::get<double>(rep);
    const double roundIncrement = value > 0.0 ? 0.5 : -0.5;

    if (value > std::numeric_limits<double>::max() / nFloatDigits - roundIncrement / nFloatDigits)
        /* The product for constructing a fraction doesn't fit into a double. */
        return;
    else if (value < std::numeric_limits<double>::lowest() / nFloatDigits - roundIncrement / nFloatDigits)
        return;

    auto truncated = Int(value * nFloatDigits + roundIncrement);

    if (std::abs(static_cast<double>(truncated) / nFloatDigits - value) < std::numeric_limits<double>::epsilon())
        /* This will also catch very low double values, which turns them into a rational zero. */
        rep.emplace<Rational>(truncated, Int(nFloatDigits));
}

tsym::Number& tsym::Number::operator+=(const Number& rhs)
{
    if (isThisOrOtherDouble(rhs)) {
        rep.emplace<double>(toDouble() + rhs.toDouble());

        tryDoubleToFraction();
    } else
        rep.emplace<Rational>(std::get<Rational>(rep) + std::get<Rational>(rhs.rep));

    setDebugString();

    return *this;
}

bool tsym::Number::isThisOrOtherDouble(const Number& other) const
{
    return isDouble(*this) || isDouble(other);
}

tsym::Number& tsym::Number::operator-=(const Number& rhs)
{
    return operator+=(-rhs);
}

tsym::Number& tsym::Number::operator*=(const Number& rhs)
{
    if (isThisOrOtherDouble(rhs)) {
        rep.emplace<double>(toDouble() * rhs.toDouble());

        tryDoubleToFraction();
    } else
        rep.emplace<Rational>(std::get<Rational>(rep) * std::get<Rational>(rhs.rep));

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
    if (isRational(*this))
        return {-numerator(), denominator()};
    else
        return {-toDouble()};
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
    static const Number zero(0);

    if (*this == zero && exponent.numerator() < 0) {
        TSYM_ERROR("Number division by zero! Result is undefined.");
        throw std::overflow_error("Zero divisor in rational number division");
    } else if (*this == zero || *this == 1 || exponent == 1) {
        result = *this;
        return true;
    } else if (exponent == zero) {
        result = 1;
        return true;
    } else if (*this < 0 && !isInt(exponent)) {
        throw std::overflow_error("Illegal power with base zero and non-integer exponent");
    } else if (*this == -1) {
        result = computeMinusOneToThe(exponent);
        return true;
    }

    return false;
}

tsym::Number tsym::Number::computeMinusOneToThe(const Number& exponent) const
{
    assert(isInt(exponent));

    return exponent.numerator() % 2 == 0 ? 1 : -1;
}

bool tsym::Number::processNegBase(const Number& exponent, Number& result) const
{
    Number preFac(-1);

    if (*this > 0)
        return false;

    assert(isInt(exponent));

    preFac = preFac.toThe(exponent);

    result = abs(*this).toThe(exponent) * preFac;

    return true;
}

bool tsym::Number::processIrrationalPowers(const Number& exponent, Number& result) const
{
    if (isThisOrOtherDouble(exponent)) {
        result = {std::pow(toDouble(), exponent.toDouble())};
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
    const auto exp = static_cast<unsigned>(integer::abs(numExponent));
    const Int newDenom = integer::pow(denominator(), exp);
    const Int newNum = integer::pow(numerator(), exp);

    if (numExponent < 0)
        /* The method takes care of negative a numerator. */
        result = {newDenom, newNum};
    else
        result = {newNum, newDenom};
}

namespace tsym {
    namespace {
        Int tryGetBase(const Int& n, const Int& denomExponent)
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

            if (exact > std::numeric_limits<int>::max())
                /* Result wouldn't fit into an int. */
                return 0;

            const auto base = static_cast<int>(exact + 0.1);

            if (std::abs(exact - static_cast<double>(base)) > 1.e-6)
                /* We are not too strict here, because of the following check. */
                return 0;

            assert(denomExponent > 0 && integer::fitsInto<unsigned>(denomExponent));

            return integer::pow(Int(base), static_cast<unsigned>(denomExponent)) == n ? base : 0;
        }
    }
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
        result = {std::pow(result.toDouble(), 1.0 / static_cast<double>(denomExponent))};
    else
        result = {numTest, denomTest};
}

tsym::Int tsym::Number::numerator() const
{
    if (std::holds_alternative<Rational>(rep))
        return std::get<Rational>(rep).numerator();
    else
        return 0;
}

tsym::Int tsym::Number::denominator() const
{
    if (std::holds_alternative<Rational>(rep))
        return std::get<Rational>(rep).denominator();
    else
        return 1;
}

double tsym::Number::toDouble() const
{
    if (std::holds_alternative<Rational>(rep))
        return boost::rational_cast<double>(std::get<Rational>(rep));
    else
        return std::get<double>(rep);
}

namespace tsym {
    namespace {
        bool areEqual(double lhs, double rhs)
        {
            const double diff = std::abs(lhs - rhs);
            double max = 1.0;

            lhs = std::abs(lhs);
            rhs = std::abs(rhs);

            max = lhs > max ? lhs : max;
            max = rhs > max ? rhs : max;

            return diff < 100. * std::numeric_limits<double>::epsilon() * max;
        }
    }
}

bool tsym::operator==(const Number& lhs, const Number& rhs)
{
    if (isRational(lhs) && isRational(rhs))
        return lhs.numerator() == rhs.numerator() && lhs.denominator() == rhs.denominator();
    else
        return areEqual(lhs.toDouble(), rhs.toDouble());
}

bool tsym::operator<(const Number& lhs, const Number& rhs)
{
    return lhs.toDouble() < rhs.toDouble();
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
