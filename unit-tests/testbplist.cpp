
#include "baseptrlist.h"
#include "bplist.h"
#include "fixtures.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestBpList, AbcFixture)

BOOST_AUTO_TEST_CASE(restOfEmptyList, noLogs())
{
    const BasePtrList empty{};

    auto result = bplist::rest(empty);

    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(restOfRvalueArg)
{
    const BasePtrList expected{b, c, d, ten};
    const BasePtrList list = bplist::rest({a, b, c, d, ten});

    BOOST_CHECK_EQUAL_COLLECTIONS(cbegin(expected), cend(expected), cbegin(list), cend(list));
}

BOOST_AUTO_TEST_SUITE_END()
