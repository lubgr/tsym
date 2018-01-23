
#include "symbol.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Symbol) {};

TEST(Symbol, creationByString)
{
    const BasePtr ptr = Symbol::create("abc");
    const Name expected("abc");

    CHECK(ptr->isSymbol());
    CHECK_EQUAL(expected, ptr->name());
}

TEST(Symbol, numericEvaluation)
{
    const BasePtr ptr = Symbol::create("dummy");

    CHECK_FALSE(ptr->isNumericallyEvaluable());
}

TEST(Symbol, typeString)
{
    const std::string expected("Symbol");
    const BasePtr basePtr = Symbol::create("dummy");

    CHECK_EQUAL(expected, basePtr->typeStr());
}

TEST(Symbol, emptyName)
{
    disableLog();
    const BasePtr undefined = Symbol::create("");
    enableLog();

    CHECK(undefined->isUndefined());
}

TEST(Symbol, nameReservedForTemporaries)
{
    const Name name("tmp#99999999999");

    disableLog();
    const BasePtr tmp = Symbol::create(name);
    enableLog();

    CHECK(tmp->isSymbol());
    CHECK_FALSE(tmp->name() == name);
}
