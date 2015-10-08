
#include <limits>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include "int.h"
#include "logging.h"

/* The corresponding min() function isn't used because e.g. inverting the sign of the smallest long
 * integer leads to an overflow, e.g. in the abs() method. */
const int tsym::Int::maxInt = std::numeric_limits<int>::max();
const int tsym::Int::minInt = -maxInt;
const long tsym::Int::maxLong = std::numeric_limits<long>::max();
const long tsym::Int::minLong = -maxLong;

tsym::Int::Int() :
    overflow(false),
    rep(0)
{}

tsym::Int::Int(int n) :
    overflow(false),
    rep(static_cast<long>(n))
{}

tsym::Int::Int(long n) :
    overflow(false),
    rep(n)
{
    if (n < minLong) {
        /* Happens if instantiated with the min. long int. */
        logging::warning() << n << " doesn't fit into Int class (min. value: " << minLong <<
                "). Return overflowed Int!";
        overflow = true;
    }
}

tsym::Int::~Int() {}

tsym::Int& tsym::Int::operator += (const Int& rhs)
{
    if (warnOverflow("Addition", rhs))
        overflow = true;
    else if (rhs.rep > 0 && rep > maxLong - rhs.rep)
        overflow = true;
    else if (rhs.rep < 0 && rep < minLong - rhs.rep)
        overflow = true;
    else
        rep += rhs.rep;

    return *this;
}

bool tsym::Int::warnOverflow(const char *operationName) const
{
    return warnOverflow(operationName, Int(0));
}

bool tsym::Int::warnOverflow(const char *operationName, const Int& operand) const
{
    if (overflow)
        logging::warning() << operationName << " with overflowed Integer!";

    if (operand.overflow)
        logging::warning() << operationName << " with overflowed Integer operand!";

    return overflow || operand.overflow;
}

tsym::Int& tsym::Int::operator -= (const Int& rhs)
{
    if (warnOverflow("Subtraction", rhs))
        overflow = true;
    else if (rhs.rep > 0 && rep < minLong + rhs.rep)
        overflow = true;
    else if (rhs.rep < 0 && rep > maxLong + rhs.rep)
        overflow = true;
    else
        rep -= rhs.rep;

    return *this;
}

tsym::Int& tsym::Int::operator *= (const Int& rhs)
{
    if (warnOverflow("Multiplication", rhs))
        overflow = true;
    else if (std::abs(toDouble()) > static_cast<double>(maxLong)/std::abs(rhs.toDouble()))
        overflow = true;
    else
        rep *= rhs.rep;

    return *this;
}

tsym::Int& tsym::Int::operator /= (const Int& rhs)
{
    if (warnOverflow("Division", rhs))
        overflow = true;
    else if (rhs.rep == 0) {
        logging::error() << "Division by zero! Set integer to overflowed.";
        overflow = true;
    } else
        rep /= rhs.rep;

    return *this;
}

tsym::Int& tsym::Int::operator %= (const Int& rhs)
{
    if (warnOverflow("Modulo", rhs))
        overflow = true;
    else
        rep %= rhs.rep;

    return *this;
}

tsym::Int& tsym::Int::operator ++ ()
{
    return operator += (Int(1));
}

tsym::Int& tsym::Int::operator -- ()
{
    return operator -= (Int(1));
}

tsym::Int tsym::Int::operator ++ (int)
{
    Int result(*this);

    operator ++ ();

    return result;
}

tsym::Int tsym::Int::operator -- (int)
{
    Int result(*this);

    operator -- ();

    return result;
}

const tsym::Int& tsym::Int::operator + () const
{
    return *this;
}

tsym::Int tsym::Int::operator - () const
{
    if (warnOverflow("Unary minus "))
        return createOverflowed();
    else
        return Int(-rep);
}

tsym::Int tsym::Int::toThe(const Int& exp) const
{
    if (warnOverflow("Power", exp))
        return createOverflowed();
    else if (rep == 1)
        return *this;
    else if (exp.rep == 0)
        return 1;
    else if (exp.rep < 0) {
        logging::error() << "Request of integer power with negative exponent! Return zero.";
        return 0;
    } else
        return nonTrivialPower(exp);
}

tsym::Int tsym::Int::createOverflowed() const
{
    Int result;

    result.overflow = true;

    return result;
}

tsym::Int tsym::Int::nonTrivialPower(const Int& exp) const
{
    const long base = rep;
    long res = base;

    for (long i = 1; i < exp.rep; ++i)
        if (std::labs(res) > maxLong/std::labs(base))
            return createOverflowed();
        else
            res = res*base;

    return Int(res);
}

tsym::Int tsym::Int::abs() const
{
    if (warnOverflow("Absolute value"))
        return createOverflowed();
    else if (rep < 0)
        return Int(-rep);
    else
        return *this;
}

bool tsym::Int::equal(const Int& rhs) const
{
    if (warnOverflow("Comparison", rhs))
        return false;
    else
        return rep == rhs.rep;
}

bool tsym::Int::lessThan(const Int& rhs) const
{
    if (warnOverflow("Comparison", rhs))
        return false;
    else
        return rep < rhs.rep;
}

int tsym::Int::sign() const
{
    if (warnOverflow("Sign request"))
        return 1;
    else
        return rep >= 0 ? 1 : -1;
}

bool tsym::Int::hasOverflowed() const
{
    return overflow;
}

bool tsym::Int::fitsIntoInt() const
{
    return !hasOverflowed() && rep <= static_cast<long>(maxInt) && rep >= static_cast<long>(minInt);
}

bool tsym::Int::fitsIntoLong() const
{
    return !hasOverflowed();
}

int tsym::Int::toInt() const
{
    if (!fitsIntoInt() && rep > 0) {
        logging::error() << "Primitive int request for too large value, return max. int " << maxInt;
        return maxInt;
    } else if (!fitsIntoInt() && rep < 0) {
        logging::error() << "Primitive int request for too small value return min. int " << minInt;
        return minInt;
    }

    return static_cast<int>(rep);
}

long tsym::Int::toLong() const
{
    if (overflow) {
        logging::error() << "Long request for too large integer, return max. long " << maxLong;
        return maxLong;
    }

    return rep;
}

double tsym::Int::toDouble() const
{
    return static_cast<double>(rep);
}

void tsym::Int::print(std::ostream& stream) const
{
    stream << rep;
}

tsym::Int tsym::Int::max()
{
    return maxLong;
}

tsym::Int tsym::Int::min()
{
    return minLong;
}

bool tsym::operator == (const Int& lhs, const Int& rhs)
{
    return lhs.equal(rhs);
}

bool tsym::operator != (const Int& lhs, const Int& rhs)
{
    return !lhs.equal(rhs);
}

bool tsym::operator < (const Int& lhs, const Int& rhs)
{
    return lhs.lessThan(rhs);
}

bool tsym::operator <= (const Int& lhs, const Int& rhs)
{
    return !(lhs > rhs);
}

bool tsym::operator > (const Int& lhs, const Int& rhs)
{
    return rhs < lhs;
}

bool tsym::operator >= (const Int& lhs, const Int& rhs)
{
    return !(lhs < rhs);
}

tsym::Int tsym::operator + (Int lhs, const Int& rhs)
{
    lhs += rhs;

    return lhs;
}

tsym::Int tsym::operator - (Int lhs, const Int& rhs)
{
    lhs -= rhs;

    return lhs;
}

tsym::Int tsym::operator * (Int lhs, const Int& rhs)
{
    lhs *= rhs;

    return lhs;
}

tsym::Int tsym::operator / (Int lhs, const Int& rhs)
{
    lhs /= rhs;

    return lhs;
}

tsym::Int tsym::operator % (Int lhs, const Int& rhs)
{
    lhs %= rhs;

    return lhs;
}

std::ostream& tsym::operator << (std::ostream& stream, const Int& rhs)
{
    rhs.print(stream);

    return stream;
}
