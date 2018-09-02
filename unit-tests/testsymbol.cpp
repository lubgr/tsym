
#include "fixtures.h"
#include "number.h"
#include "symbol.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_AUTO_TEST_SUITE(TestSymbol)

BOOST_AUTO_TEST_CASE(creationByString)
{
    const BasePtr ptr = Symbol::create("abc");
    const Name expected("abc");

    BOOST_TEST(ptr->isSymbol());
    BOOST_CHECK_EQUAL(expected, ptr->name());
}

BOOST_AUTO_TEST_CASE(numericEvaluation)
{
    const BasePtr ptr = Symbol::create("dummy");

    BOOST_TEST(!ptr->numericEval());
}

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Symbol");
    const BasePtr basePtr = Symbol::create("dummy");

    BOOST_CHECK_EQUAL(expected, basePtr->typeStr());
}

BOOST_AUTO_TEST_CASE(emptyName, noLogs())
{
    const BasePtr undefined = Symbol::create("");

    BOOST_TEST(undefined->isUndefined());
}

BOOST_AUTO_TEST_CASE(nameReservedForTemporaries, noLogs())
{
    const Name name("tmp#99999999999");

    const BasePtr tmp = Symbol::create(name);

    BOOST_TEST(tmp->isSymbol());
    BOOST_TEST(tmp->name() != name);
}

BOOST_AUTO_TEST_SUITE_END()
