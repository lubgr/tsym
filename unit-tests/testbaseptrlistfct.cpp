
#include "baseptrlist.h"
#include "baseptrlistfct.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestBpList, AbcFixture)

BOOST_AUTO_TEST_CASE(restOfEmptyList, noLogs())
{
    const BasePtrList empty{};

    auto result = rest(empty);

    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(restOfRvalueArg)
{
    const BasePtrList expected{b, c, d, ten};
    const BasePtrList list = rest({a, b, c, d, ten});

    BOOST_TEST(expected == list, per_element());
}

BOOST_AUTO_TEST_SUITE_END()
