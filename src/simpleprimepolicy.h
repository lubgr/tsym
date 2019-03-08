#ifndef TSYM_SIMPLEPRIMEPOLICY_H
#define TSYM_SIMPLEPRIMEPOLICY_H

#include <cmath>
#include <vector>

namespace tsym {
    template <class Integer> struct SimplePrimePolicy {
        static void computeAndStore(Integer n, std::vector<Integer>& primes)
        {
            static const Integer zero{0};
            static const Integer two{2};

            primes.clear();

            while (n % two == zero) {
                primes.emplace_back(2);
                n /= two;
            }

            const auto upperLimit = std::sqrt(static_cast<double>(n)) + 0.5;

            for (Integer i(3); static_cast<double>(i) < upperLimit; i = i + two)
                while (n % i == zero) {
                    primes.push_back(i);
                    n /= i;
                }

            if (n > two)
                primes.push_back(n);
        }
    };
}

#endif
