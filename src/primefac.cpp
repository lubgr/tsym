
#include "primefac.h"
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/unique.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/range/numeric.hpp>
#include <limits>
#include "numberfct.h"

namespace tsym {
    namespace {
        template <class Container> void duplicate(Container& c, Container&& result, unsigned n)
        {
            using std::begin;
            using std::end;
            unsigned insCount = n;

            for (auto i = begin(c); i != end(c); ++i) {
                auto next = i;

                if (++next != end(c) && *next == *i) {
                    insCount += n;
                    continue;
                }

                result.insert(cend(result), insCount, *i);
                insCount = n;
            }

            c = std::move(result);
        }

        void duplicate(std::vector<Int>& primes, const Int& n)
        {
            std::vector<Int> result;

            assert(n > 0 && n < Int(std::numeric_limits<unsigned>::max()));

            const auto nUnsigned = static_cast<unsigned>(n);

            result.reserve(nUnsigned * primes.size());

            duplicate(primes, std::move(result), nUnsigned);
        }
    }
}

void tsym::PrimeFac::toThe(const Int& exponent)
{
    if (exponent == 0) {
        numPrimes.clear();
        denomPrimes.clear();
    } else {
        duplicate(numPrimes, integer::abs(exponent));
        duplicate(denomPrimes, integer::abs(exponent));
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

    merge(numPrimes, std::move(copy.numPrimes));
    merge(denomPrimes, std::move(copy.denomPrimes));
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

void tsym::PrimeFac::merge(std::vector<Int>& target, std::vector<Int>&& source)
{
    target.insert(end(target), std::make_move_iterator(begin(source)), std::make_move_iterator(end(source)));

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

                    it = std::prev(source.erase(it - static_cast<int>(expDenom - 1), it + 1));

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
    } else if (numPrimes.empty() && denomPrimes.empty())
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
            const auto firstSize = boost::size(firstEqualRange);

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
    const auto num = boost::accumulate(numPrimes, Int(1), std::multiplies<>());
    const auto denom = boost::accumulate(denomPrimes, Int(1), std::multiplies<>());

    return Number(num, denom);
}
