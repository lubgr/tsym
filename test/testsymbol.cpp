
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

    BOOST_TEST(!ptr->isNumericallyEvaluable());
}

BOOST_AUTO_TEST_CASE(typeString)
{
    const std::string expected("Symbol");
    const BasePtr basePtr = Symbol::create("dummy");

    BOOST_CHECK_EQUAL(expected, basePtr->typeStr());
}

BOOST_AUTO_TEST_CASE(emptyName)
{
    disableLog();
    const BasePtr undefined = Symbol::create("");
    enableLog();

    BOOST_TEST(undefined->isUndefined());
}

BOOST_AUTO_TEST_CASE(nameReservedForTemporaries)
{
    const Name name("tmp#99999999999");

    disableLog();
    const BasePtr tmp = Symbol::create(name);
    enableLog();

    BOOST_TEST(tmp->isSymbol());
    BOOST_TEST(tmp->name() != name);
}

BOOST_AUTO_TEST_SUITE_END()
