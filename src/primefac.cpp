
#include <cmath>
#include <algorithm>
#include "primefac.h"
#include "number.h"
#include "logging.h"

tsym::PrimeFac::PrimeFac(const Number& n)
{
    checkAndFactorize(n);
}

void tsym::PrimeFac::checkAndFactorize(const Number& n)
{
    if (n.isDouble() || n < 0)
        return;
    else
        factorize(n);
}

void tsym::PrimeFac::factorize(const Number& n)
{
    defPrimes(n.numerator(), numPrimes);
    defPrimes(n.denominator(), denomPrimes);
}

void tsym::PrimeFac::defPrimes(Int n, std::vector<Int>& primes)
    /* Saves the prime factorization of n into the given vector. This is done without any special
     * algorithm, meaning that the speed of this operation is poor. However, this method will mostly
     * been called for moderate values, and even for high numbers the factorization didn't cause a
     * remarkable break of the program, so for now, this is supposed to be sufficient. */
{
    static const Int zero(0);
    static const Int two(2);
    double upperLimit;

    primes.clear();

    while (n % two == zero) {
        primes.push_back(two);
        n /= two;
    }

    upperLimit = std::sqrt(static_cast<double>(n)) + 0.5;

    for (Int i(3); static_cast<double>(i) < upperLimit; i = i + two)
        while (n % i == zero) {
            primes.push_back(i);
            n /= i;
        }

    if (n > two)
        primes.push_back(n);
}

void tsym::PrimeFac::set(const Number& n)
{
    checkAndFactorize(n);
}

void tsym::PrimeFac::toThe(const Int& exponent)
{
    if (exponent == 0) {
        numPrimes.clear();
        denomPrimes.clear();
    }

    copyElementsNTimes(integer::abs(exponent), numPrimes);
    copyElementsNTimes(integer::abs(exponent), denomPrimes);

    if (exponent < 0)
        numPrimes.swap(denomPrimes);
}

void tsym::PrimeFac::copyElementsNTimes(const Int& n, std::vector<Int>& primes)
{
    std::vector<Int>::iterator it(primes.begin());
    Int i;

    while (it != primes.end()) {
        for (i = 0; i < n - 1; ++i)
            it = primes.insert(it, *it);

        for (i = 0; i < n; ++i)
            ++it;
    }
}

void tsym::PrimeFac::multiply(const Number& n)
{
    const PrimeFac other(n);

    multiply(other);
}

void tsym::PrimeFac::multiply(const PrimeFac& other)
{
    PrimeFac copy(other);

    if (copy.numPrimes.empty() && copy.denomPrimes.empty())
        return;

    cancelPrimes(numPrimes, copy.denomPrimes);
    cancelPrimes(denomPrimes, copy.numPrimes);

    merge(numPrimes, copy.numPrimes);
    merge(denomPrimes, copy.denomPrimes);
}

void tsym::PrimeFac::cancelPrimes(std::vector<Int>& p1, std::vector<Int>& p2)
{
    std::vector<Int>::iterator it1(p1.begin());
    std::vector<Int>::iterator it2(p2.begin());

    while (it1 != p1.end() && it2 != p2.end())
        if (*it1 < *it2)
            ++it1;
        else if (*it1 > *it2)
            ++it2;
        else {
            it1 = p1.erase(it1);
            it2 = p2.erase(it2);
        }
}

void tsym::PrimeFac::merge(std::vector<Int>& target, const std::vector<Int>& source)
{
    target.insert(target.end(), source.begin(), source.end());

    std::sort(target.begin(), target.end());
}

tsym::PrimeFac tsym::PrimeFac::extract(const Number& exponent)
{
    PrimeFac extraction;

    extract(numPrimes, extraction.numPrimes, exponent.denominator());
    extract(denomPrimes, extraction.denomPrimes, exponent.denominator());

    extraction.toThe(exponent.numerator());

    return extraction;
}

void tsym::PrimeFac::extract(std::vector<Int>& source, std::vector<Int>& target,
        const Int& expDenom)
{
    Int prime(0);
    Int nPrime(0);

    for (auto it = source.begin(); it != source.end(); ++it)
        if (prime != *it) {
            prime = *it;
            nPrime = 1;
        } else if (++nPrime == expDenom) {
            target.push_back(prime);

            for (Int i(0); i < expDenom; ++i)
                it = --source.erase(it);

            nPrime = 0;
        }
}

tsym::Number tsym::PrimeFac::collectToNewExp(const Number& exponent)
{
    const int count = getEqualCount();

    if (count > 1) {
        deleteElements(count - 1);
        return count*exponent;
    } else if (numPrimes.size() == 0 && denomPrimes.size() == 0)
        return 1;
    else
        return exponent;
}

int tsym::PrimeFac::getEqualCount() const
{
    int denomCount;
    int numCount;

    if (numPrimes.empty() && denomPrimes.empty())
        return 0;
    else if (numPrimes.empty())
        return getEqualCount(denomPrimes);
    else if (denomPrimes.empty())
        return getEqualCount(numPrimes);

    numCount = getEqualCount(numPrimes);
    denomCount = getEqualCount(denomPrimes);

    return numCount == denomCount ? numCount : 0;
}

int tsym::PrimeFac::getEqualCount(const std::vector<Int>& primes) const
{
    int lastCount = 0;
    int count = 0;
    Int oldPrime;

    /* The vector must not be empty at this point! */
    for (const auto& prime : primes)
        if (prime == oldPrime)
            ++count;
        else if (lastCount != 0 && lastCount != count)
            return 0;
        else {
            lastCount = count;
            count = 1;
            oldPrime = prime;
        }

    return count == lastCount || lastCount == 0 ? count : 0;
}

void tsym::PrimeFac::deleteElements(int nToDelete)
{
    deleteElements(nToDelete, numPrimes);
    deleteElements(nToDelete, denomPrimes);
}

void tsym::PrimeFac::deleteElements(int nToDelete, std::vector<Int>& primes)
    /* Removes duplicate elements from the given vector of primes. It is supposed, that primes have
     * been checked before, such that this operation is safe. */
{
    for (auto it = primes.begin(); it != primes.end(); ++it)
        for (int i = 0; i < nToDelete; ++i)
            it = primes.erase(it);
}

const std::vector<tsym::Int>& tsym::PrimeFac::getNumPrimes() const
{
    return numPrimes;
}

const std::vector<tsym::Int>& tsym::PrimeFac::getDenomPrimes() const
{
    return denomPrimes;
}

tsym::Number tsym::PrimeFac::eval() const
{
    std::vector<Int>::const_iterator nIt(numPrimes.begin());
    std::vector<Int>::const_iterator dIt(denomPrimes.begin());
    Number res(1);
    Int denom;
    Int num;

    while (nIt != numPrimes.end() || dIt != denomPrimes.end()) {
        num = nIt == numPrimes.end() ? 1 : *nIt++;
        denom = dIt == denomPrimes.end() ? 1 : *dIt++;
        res *= Number(num, denom);
    }

    return res;
}
