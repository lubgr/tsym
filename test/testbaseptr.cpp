
#include "abc.h"
#include "numeric.h"
#include "symbol.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(BasePtr) {};

TEST(BasePtr, noArgumentCtor)
{
    BasePtr ptr;

    CHECK(ptr->isUndefined());
}

TEST(BasePtr, initWithNullPointer)
{
    disableLog();
    const BasePtr ptr(nullptr);
    enableLog();

    CHECK(ptr->isUndefined());
}

TEST(BasePtr, pointerAccess)
{
    const Base *base = &*ten;

    POINTERS_EQUAL(base, &*ten);
}

TEST(BasePtr, assignmentOperator)
{
    BasePtr assigned = Symbol::create("dummy");

    assigned = ten;

    CHECK_EQUAL(ten, assigned);
}

TEST(BasePtr, selfAssignment)
{
    BasePtr dummy = Numeric::create(12345);

    dummy = dummy;

    CHECK_EQUAL(dummy, dummy);
}
