
#include <boost/range/numeric.hpp>
#include "int.h"
#include "simpleprimepolicy.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_AUTO_TEST_SUITE(TestSimplePrimePolicy)

BOOST_AUTO_TEST_CASE(factorizeIntegralInt)
{
    const std::vector<int> expected{3, 3, 5, 7, 13, 13, 17, 37};
    const auto input = boost::accumulate(expected, 1, std::multiplies<int>{});
    std::vector<int> result;

    SimplePrimePolicy<int>::computeAndStore(input, result);

    BOOST_CHECK_EQUAL_COLLECTIONS(cbegin(expected), cend(expected), cbegin(result), cend(result));
}

BOOST_AUTO_TEST_CASE(factorizeMultiPrecisionInt)
{
    const std::vector<Int> expected{2, 2, 5, 7, 11, 83};
    const auto input = boost::accumulate(expected, Int{1}, std::multiplies<Int>{});
    std::vector<Int> result;

    SimplePrimePolicy<Int>::computeAndStore(input, result);

    BOOST_CHECK_EQUAL_COLLECTIONS(cbegin(expected), cend(expected), cbegin(result), cend(result));
}

BOOST_AUTO_TEST_SUITE_END()
