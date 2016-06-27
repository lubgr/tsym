
#include "abc.h"
#include "symbolmap.h"
#include "sum.h"
#include "power.h"
#include "product.h"
#include "constant.h"
#include "cpputest.h"

using namespace tsym;

TEST_GROUP(SymbolMap) {};

TEST(SymbolMap, simpleReplacement)
{
    BasePtr replacement;
    SymbolMap map;

    replacement = map.getTmpSymbolAndStore(a);

    CHECK(replacement->isDifferent(a));
    CHECK_EQUAL(a, map.replaceTmpSymbolsBackFrom(replacement));
}

TEST(SymbolMap, equalArguments)
{
    const BasePtr arg = Sum::create(a, Constant::createPi());
    BasePtr replacement[2];
    SymbolMap map;

    replacement[0] = map.getTmpSymbolAndStore(arg);
    replacement[1] = map.getTmpSymbolAndStore(arg);

    CHECK_EQUAL(replacement[0], replacement[1]);
    CHECK_EQUAL(arg, map.replaceTmpSymbolsBackFrom(replacement[0]));
    CHECK_EQUAL(arg, map.replaceTmpSymbolsBackFrom(replacement[1]));
}

TEST(SymbolMap, equalArgumentsDifferentMaps)
{
    const BasePtr arg = Sum::create(four, a);
    BasePtr replacement[2];
    SymbolMap map1;
    SymbolMap map2;

    replacement[0] = map1.getTmpSymbolAndStore(arg);
    replacement[1] = map2.getTmpSymbolAndStore(arg);

    CHECK(replacement[0]->isDifferent(replacement[1]));
}

TEST(SymbolMap, differentArguments)
{
    const BasePtr arg1 = Power::create(a, b);
    const BasePtr arg2 = Product::create(ten, c);
    BasePtr replacement[2];
    SymbolMap map;

    replacement[0] = map.getTmpSymbolAndStore(arg1);
    replacement[1] = map.getTmpSymbolAndStore(arg2);

    CHECK(replacement[0]->isDifferent(replacement[1]));
}
