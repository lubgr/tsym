
#include <numeric>
#include "int.h"
#include "simpleprimepolicy.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(SimplePrimePolicy) {};

TEST(SimplePrimePolicy, factorizeIntegralInt)
{
    const std::vector<int> expected { 3, 3, 5, 7, 13, 13, 17, 37 };
    const auto input = std::accumulate(cbegin(expected), cend(expected), 1, std::multiplies<int>{});
    std::vector<int> result;

    SimplePrimePolicy<int>::computeAndStore(input, result);

    CHECK_EQUAL(expected.size(), result.size());

    for (decltype(result)::size_type i = 0; i < result.size(); ++i) {
        CHECK_EQUAL(expected[i], result[i]);
    }
}

TEST(SimplePrimePolicy, factorizeMultiPrecisionInt)
{
    const std::vector<Int> expected { 2, 2, 5, 7, 11, 83 };
    const auto input = std::accumulate(cbegin(expected), cend(expected), Int{1}, std::multiplies<Int>{});
    std::vector<Int> result;

    SimplePrimePolicy<Int>::computeAndStore(input, result);

    CHECK_EQUAL(expected.size(), result.size());

    for (decltype(result)::size_type i = 0; i < result.size(); ++i) {
        CHECK_EQUAL(expected[i], result[i]);
    }
}
