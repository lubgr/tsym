
#include "constant.h"
#include "fixtures.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbolmap.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_FIXTURE_TEST_SUITE(TestSymbolMap, AbcFixture)

BOOST_AUTO_TEST_CASE(simpleReplacement)
{
    BasePtr replacement;
    SymbolMap map;

    replacement = map.getTmpSymbolAndStore(a);

    BOOST_TEST(replacement->isDifferent(*a));
    BOOST_CHECK_EQUAL(a, map.replaceTmpSymbolsBackFrom(replacement));
}

BOOST_AUTO_TEST_CASE(equalArguments)
{
    const BasePtr arg = Sum::create(a, pi);
    BasePtr replacement[2];
    SymbolMap map;

    replacement[0] = map.getTmpSymbolAndStore(arg);
    replacement[1] = map.getTmpSymbolAndStore(arg);

    BOOST_CHECK_EQUAL(replacement[0], replacement[1]);
    BOOST_CHECK_EQUAL(arg, map.replaceTmpSymbolsBackFrom(replacement[0]));
    BOOST_CHECK_EQUAL(arg, map.replaceTmpSymbolsBackFrom(replacement[1]));
}

BOOST_AUTO_TEST_CASE(equalArgumentsDifferentMaps)
{
    const BasePtr arg = Sum::create(four, a);
    BasePtr replacement[2];
    SymbolMap map1;
    SymbolMap map2;

    replacement[0] = map1.getTmpSymbolAndStore(arg);
    replacement[1] = map2.getTmpSymbolAndStore(arg);

    BOOST_TEST(replacement[0]->isDifferent(*replacement[1]));
}

BOOST_AUTO_TEST_CASE(differentArguments)
{
    const BasePtr arg1 = Power::create(a, b);
    const BasePtr arg2 = Product::create(ten, c);
    BasePtr replacement[2];
    SymbolMap map;

    replacement[0] = map.getTmpSymbolAndStore(arg1);
    replacement[1] = map.getTmpSymbolAndStore(arg2);

    BOOST_TEST(replacement[0]->isDifferent(*replacement[1]));
}

BOOST_AUTO_TEST_SUITE_END()
