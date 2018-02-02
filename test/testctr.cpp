
#include "baseptrlist.h"
#include "bplist.h"
#include "abc.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(BpList) {};

TEST(BpList, restOfEmptyList)
{
    const BasePtrList empty{};

    disableLog();
    auto result = bplist::rest(empty);
    enableLog();

    CHECK(result.empty());
}

TEST(BpList, restOfNonCVList)
{
    BasePtrList list{ a, b, c, d, ten };
    BasePtrList expected{ b, c, d, ten };

    bplist::rest(list);

    CHECK_EQUAL(expected, list);
}
