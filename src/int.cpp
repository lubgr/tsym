
#include <limits>
#include <cstring>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include "int.h"
#include "logging.h"

namespace tsym {
    namespace {
        static const int maxInt = std::numeric_limits<int>::max();
        static const int minInt = std::numeric_limits<int>::min();
        static const long maxLong = std::numeric_limits<long>::max();
        static const long minLong = std::numeric_limits<long>::min();
    }
}

tsym::Int::Int()
{
    mpz_init(handle);
    mpz_set_si(handle, 0);
}

tsym::Int::Int(int n)
{
    mpz_init(handle);
    mpz_set_si(handle, n);
}

tsym::Int::Int(long n)
{
    mpz_init(handle);
    mpz_set_si(handle, n);
}

tsym::Int::Int(double n)
{
    mpz_init(handle);
    mpz_set_d(handle, n);
}

tsym::Int::Int(const char *str)
{
    int flag;

    mpz_init(handle);
    flag = mpz_set_str(handle, str, 10);

    if (flag != 0)
        TSYM_ERROR("Failed to parse integer from \'%s\'", str);
}

tsym::Int::Int(const Int& other)
{
    mpz_init(handle);
    mpz_set(handle, other.handle);
}

const tsym::Int& tsym::Int::operator = (const Int& rhs)
{
    if (this != &rhs) {
        mpz_clear(handle);
        mpz_init(handle);
        mpz_set(handle, rhs.handle);
    }

    return *this;
}

tsym::Int::~Int()
{
    mpz_clear(handle);
}

tsym::Int& tsym::Int::operator += (const Int& rhs)
{
    mpz_add(handle, handle, rhs.handle);

    return *this;
}

tsym::Int& tsym::Int::operator -= (const Int& rhs)
{
    mpz_sub(handle, handle, rhs.handle);

    return *this;
}

tsym::Int& tsym::Int::operator *= (const Int& rhs)
{
    mpz_mul(handle, handle, rhs.handle);

    return *this;
}

tsym::Int& tsym::Int::operator /= (const Int& rhs)
{
    if (mpz_cmp_si(rhs.handle, 0) == 0)
        TSYM_ERROR("Division by zero!");

    mpz_fdiv_q(handle, handle, rhs.handle);

    return *this;
}

tsym::Int& tsym::Int::operator %= (const Int& rhs)
{
    mpz_tdiv_r(handle, handle, rhs.handle);

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
    Int result(*this);

    mpz_neg(result.handle, handle);

    return result;
}

tsym::Int tsym::Int::toThe(const Int& exp) const
{
    if (mpz_cmp_si(handle, 1) == 0)
        return *this;
    else if (mpz_cmp_si(exp.handle, 0) == 0)
        return 1;
    else if (mpz_cmp_si(exp.handle, 0) < 0) {
        TSYM_ERROR("Request of integer power with negative exponent! Return zero.");
        return 0;
    } else
        return nonTrivialPower(exp);
}

tsym::Int tsym::Int::nonTrivialPower(const Int& exp) const
{
    Int result;

    if (!mpz_fits_ulong_p(exp.handle))
        TSYM_ERROR("Can't evaluate integer power with huge exponent: ", exp);

    mpz_pow_ui(result.handle, handle, mpz_get_ui(exp.handle));

    return result;
}

tsym::Int tsym::Int::abs() const
{
    Int result(*this);

    mpz_abs(result.handle, handle);

    return result;
}

bool tsym::Int::equal(const Int& rhs) const
{
    return mpz_cmp(handle, rhs.handle) == 0;
}

bool tsym::Int::lessThan(const Int& rhs) const
{
    return mpz_cmp(handle, rhs.handle) < 0;
}

int tsym::Int::sign() const
{
    return mpz_cmp_si(handle, 0) >= 0 ? 1 : -1;
}

bool tsym::Int::fitsIntoInt() const
{
    return mpz_fits_sint_p(handle) != 0;
}

bool tsym::Int::fitsIntoLong() const
{
    return mpz_fits_slong_p(handle) != 0;
}

int tsym::Int::toInt() const
{
    if (fitsIntoInt())
        return static_cast<int>(toLong());
    else if (mpz_cmp_si(handle, 0) > 0) {
        TSYM_ERROR("Primitive int request for too large value, return max. int %d", maxInt);
        return maxInt;
    } else {
        assert(mpz_cmp_si(handle, 0) < 0);
        TSYM_ERROR("Primitive int request for too small value return min. int %d", minInt);
        return minInt;
    }
}

long tsym::Int::toLong() const
{
    if (fitsIntoLong())
        return mpz_get_si(handle);
    else if (mpz_cmp_si(handle, 0) > 0) {
        TSYM_ERROR("Primitive long request for too large value, return max. long %d", maxLong);
        return maxLong;
    } else {
        assert(mpz_cmp_si(handle, 0) < 0);
        TSYM_ERROR("Primitive long request for too small value return min. long %d", minLong);
        return minLong;
    }
}

double tsym::Int::toDouble() const
{
    return mpz_get_d(handle);
}

void tsym::Int::print(std::ostream& stream) const
{
    char *buffer = nullptr;
    int bufferLength = 100;
    int charsWritten;

    while (buffer == nullptr) {
        buffer = new char[bufferLength];

        charsWritten = gmp_snprintf(buffer, bufferLength, "%Zd", handle);

        if (charsWritten >= bufferLength) {
            bufferLength *= 2;

            delete[] buffer;
            buffer = nullptr;
        }
    }

    stream << buffer;

    delete[] buffer;
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

size_t std::hash<tsym::Int>::operator () (const tsym::Int& n) const
{
    if (n.fitsIntoLong())
        return std::hash<long>{}(n.toLong());
    else
        return std::hash<std::string>{}(
                static_cast<std::ostringstream&>(std::ostringstream() << n).str());
}
