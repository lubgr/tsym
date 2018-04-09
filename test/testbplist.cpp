
#include "baseptrlist.h"
#include "bplist.h"
#include "abc.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_AUTO_TEST_SUITE(TestBpList)

BOOST_AUTO_TEST_CASE(restOfEmptyList)
{
    const BasePtrList empty{};

    disableLog();
    auto result = bplist::rest(empty);
    enableLog();

    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(restOfNonCVList)
{
    const BasePtrList expected{ b, c, d, ten };
    BasePtrList list{ a, b, c, d, ten };

    bplist::rest(list);

    BOOST_CHECK_EQUAL_COLLECTIONS(cbegin(expected), cend(expected), cbegin(list), cend(list));
}

BOOST_AUTO_TEST_SUITE_END()
