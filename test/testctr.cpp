
#include "baseptrctr.h"
#include "ctr.h"
#include "abc.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Ctr) {};

TEST(Ctr, restOfEmptyList)
{
    const BasePtrCtr empty{};

    disableLog();
    auto result = ctr::rest(empty);
    enableLog();

    CHECK(result.empty());
}

TEST(Ctr, restOfNonCVList)
{
    BasePtrCtr list{ a, b, c, d, ten };
    BasePtrCtr expected{ b, c, d, ten };

    ctr::rest(list);

    CHECK_EQUAL(expected, list);
}
