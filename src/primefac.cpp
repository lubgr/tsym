
#include "primefac.h"
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/numeric.hpp>
#include <limits>

namespace tsym {
    namespace {
        void copyElementsNTimes(const Int& n, std::vector<Int>& primes)
        {
            using difference_type = std::vector<Int>::difference_type;
            using size_type = std::vector<Int>::size_type;

            assert(n > 0 && n < Int(std::numeric_limits<difference_type>::max()));
            const auto nUnsigned = static_cast<size_type>(n);

            primes.reserve(nUnsigned * primes.size());

            for (auto it = begin(primes); it != end(primes);) {
                it = primes.insert(it, nUnsigned - 1, *it);

                std::advance(it, static_cast<difference_type>(n));
            }
        }
    }
}

void tsym::PrimeFac::toThe(const Int& exponent)
{
    if (exponent == 0) {
        numPrimes.clear();
        denomPrimes.clear();
    } else {
        copyElementsNTimes(integer::abs(exponent), numPrimes);
        copyElementsNTimes(integer::abs(exponent), denomPrimes);
    }

    if (exponent < 0)
        numPrimes.swap(denomPrimes);
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
    auto it1 = begin(p1);
    auto it2 = begin(p2);

    while (it1 != end(p1) && it2 != end(p2))
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
    target.insert(end(target), cbegin(source), cend(source));

    boost::sort(target);
}

namespace tsym {
    namespace {
        void extractPrimes(std::vector<Int>& source, std::vector<Int>& target, const Int& expDenom)
        {
            Int prime(0);
            Int nPrime(0);

            for (auto it = begin(source); it != end(source); ++it)
                if (prime != *it) {
                    prime = *it;
                    nPrime = 1;
                } else if (++nPrime == expDenom) {
                    target.push_back(prime);

                    it = --source.erase(it - static_cast<int>(expDenom - 1), it + 1);

                    nPrime = 0;
                }
        }
    }
}

tsym::PrimeFac tsym::PrimeFac::extract(const Number& exponent)
{
    PrimeFac extraction;

    extractPrimes(numPrimes, extraction.numPrimes, exponent.denominator());
    extractPrimes(denomPrimes, extraction.denomPrimes, exponent.denominator());

    extraction.toThe(exponent.numerator());

    return extraction;
}

tsym::Number tsym::PrimeFac::collectToNewExp(const Number& exponent)
{
    const int count = getEqualCount();

    if (count > 1) {
        eraseDuplicates();
        return count * exponent;
    } else if (numPrimes.size() == 0 && denomPrimes.size() == 0)
        return 1;
    else
        return exponent;
}

namespace tsym {
    namespace {
        auto equalCountOrZero(const std::vector<Int>& primes, int guess)
        {
            for (auto prime = cbegin(primes) + guess; prime != cend(primes); prime += guess)
                if (*prime != *(prime + (guess - 1)))
                    return 0;

            return guess;
        }

        auto getEqualCountOf(const std::vector<Int>& primes)
        {
            const auto firstEqualRange = boost::find_if<boost::return_begin_found>(
              primes, [first = primes.front()](const auto& prime) { return prime != first; });
            const auto firstSize = size(firstEqualRange);

            if (primes.size() % firstSize != 0)
                return 0;
            else if (firstSize == 1)
                return 1;

            const auto n = static_cast<int>(firstSize);

            return equalCountOrZero(primes, n);
        }
    }
}

int tsym::PrimeFac::getEqualCount() const
{
    if (numPrimes.empty() && denomPrimes.empty())
        return 0;
    else if (numPrimes.empty())
        return getEqualCountOf(denomPrimes);
    else if (denomPrimes.empty())
        return getEqualCountOf(numPrimes);

    const auto numCount = getEqualCountOf(numPrimes);
    const auto denomCount = getEqualCountOf(denomPrimes);

    return numCount == denomCount ? numCount : 0;
}

namespace tsym {
    namespace {
        void eraseRemoveDuplicates(std::vector<Int>& primes)
        {
            boost::erase(primes, boost::unique<boost::return_found_end>(primes));
        }
    }
}

void tsym::PrimeFac::eraseDuplicates()
{
    eraseRemoveDuplicates(numPrimes);
    eraseRemoveDuplicates(denomPrimes);
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
    const auto num = boost::accumulate(numPrimes, Int(1), std::multiplies<Int>());
    const auto denom = boost::accumulate(denomPrimes, Int(1), std::multiplies<Int>());

    return Number(num, denom);
}
