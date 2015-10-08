
#include <cassert>
#include "cpputest.h"
#include "symbolregistry.h"
#include "symbol.h"

using namespace tsym;

TEST_GROUP(SymbolRegistry)
{
    Name name;

    void setup()
    {
        name = Name("someVariableName");
    }
};

TEST(SymbolRegistry, insertionOfNewSymbol)
{
    const std::string uniqueName("uniqueVariableName");
    BasePtr symbol;

    assert(SymbolRegistry::count(Name(uniqueName)) == 0);

    symbol = Symbol::create(uniqueName);

    CHECK_EQUAL(1, SymbolRegistry::count(Name(uniqueName)));
}

TEST(SymbolRegistry, addExistingSymbol)
{
    const BasePtr symbol1 = Symbol::create(name);
    const unsigned startCount = SymbolRegistry::count(name);
    BasePtr symbol2;

    symbol2 = Symbol::create(name);

    CHECK_EQUAL(startCount + 1, SymbolRegistry::count(name));
}

TEST(SymbolRegistry, assignmentSymbolToUndefined)
{
    const BasePtr orig = Symbol::create(name);
    const unsigned startCount = SymbolRegistry::count(name);
    BasePtr assigned;

    assigned = orig;

    CHECK_EQUAL(startCount + 1, SymbolRegistry::count(name));
}

TEST(SymbolRegistry, assigmentSymbolToDifferentSymbol)
{
    const BasePtr rhs = Symbol::create("a");
    BasePtr lhs = Symbol::create("b");
    const unsigned startCountA = SymbolRegistry::count(Name("a"));
    const unsigned startCountB = SymbolRegistry::count(Name("b"));

    lhs = rhs;

    CHECK_EQUAL(startCountA + 1, SymbolRegistry::count(Name("a")));
    CHECK_EQUAL(startCountB - 1, SymbolRegistry::count(Name("b")));
}

TEST(SymbolRegistry, assigmentSymbolToSameSymbol)
{
    const BasePtr rhs = Symbol::create("a");
    BasePtr lhs = Symbol::create("a");
    const unsigned startCountA = SymbolRegistry::count(Name("a"));

    lhs = rhs;

    CHECK_EQUAL(startCountA, SymbolRegistry::count(Name("a")));
}

TEST(SymbolRegistry, selfAssignmentSymbol)
{
    BasePtr s = Symbol::create(name);
    const unsigned startCount = SymbolRegistry::count(name);

    s = s;

    CHECK_EQUAL(startCount, SymbolRegistry::count(name));
}

TEST(SymbolRegistry, temporarySymbolCreation)
{
    const Name expectedName("tmp", "1");
    BasePtr tmp;

    assert(SymbolRegistry::count(expectedName) == 0);

    tmp = Symbol::createTmpSymbol();

    CHECK_EQUAL(expectedName, tmp->name());
}

TEST(SymbolRegistry, multipleTemporarySymbolCreation)
{
    const BasePtr tmp[] = { Symbol::createTmpSymbol(), Symbol::createTmpSymbol(),
        Symbol::createTmpSymbol(), Symbol::createTmpSymbol() };
    const Name expectedNames[] = { Name("tmp", "1"), Name("tmp", "2"), Name("tmp", "3"),
        Name("tmp", "4") };

    for (int i = 0; i < 4; ++i)
        CHECK_EQUAL(expectedNames[i], tmp[i]->name());
}

TEST(SymbolRegistry, temporarySymbolCreationWithExistingTemporaries)
{
    const Name names[] = { Name("tmp", "1"), Name("tmp", "2"), Name("tmp", "3"),
        Name("tmp", "4"), Name("tmp", "5") };
    const BasePtr alreadyThere[] = {
        Symbol::create(names[0]), Symbol::create(names[1]), Symbol::create(names[3]) };
    const BasePtr tmp3 = Symbol::createTmpSymbol();
    const BasePtr tmp5 = Symbol::createTmpSymbol();

    CHECK_EQUAL(names[0], alreadyThere[0]->name());
    CHECK_EQUAL(names[1], alreadyThere[1]->name());
    CHECK_EQUAL(names[2], tmp3->name());
    CHECK_EQUAL(names[3], alreadyThere[2]->name());
    CHECK_EQUAL(names[4], tmp5->name());
}
