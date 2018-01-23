
#include "name.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Name) {};

TEST(Name, emptyName)
{
    const Name empty("");

    CHECK(empty.plain().empty());
    CHECK(empty.unicode().empty());
    CHECK(empty.tex().empty());
}

TEST(Name, simpleString)
{
    const std::string varName("abc");
    const Name a(varName);

    CHECK_EQUAL(varName, a.plain());
    CHECK_EQUAL(varName, a.tex());
}

TEST(Name, varWithSubscriptChar)
{
    const std::string expected("U_x");
    const Name n("U", "x");

    CHECK_EQUAL(expected, n.plain());
    CHECK_EQUAL(expected, n.tex());
}

TEST(Name, varWithSubscriptString)
{
    const std::string expectedPlain("U_xyz");
    const std::string expectedTex("U_{xyz}");
    const Name n("U", "xyz");

    CHECK_EQUAL(expectedPlain, n.plain());
    CHECK_EQUAL(expectedTex, n.tex());
}

TEST(Name, varWithSubAndSuperscriptChar)
{
    const std::string expectedPlain("ABC_d_e");
    const std::string expectedTex("ABC_d^e");
    const Name n("ABC", "d", "e");

    CHECK_EQUAL(expectedPlain, n.plain());
    CHECK_EQUAL(expectedTex, n.tex());
}

TEST(Name, varWithSubCharAndSuperString)
{
    const std::string expectedPlain("ABC_d_efg");
    const std::string expectedTex("ABC_d^{efg}");
    const Name n("ABC", "d", "efg");

    CHECK_EQUAL(expectedPlain, n.plain());
    CHECK_EQUAL(expectedTex, n.tex());
}

TEST(Name, greekLowerCaseLetter)
{
    const Name gamma("gamma");

    CHECK_EQUAL("gamma", gamma.plain());
    CHECK_EQUAL("\\gamma", gamma.tex());
}

TEST(Name, greekUpperCaseLetter)
{
    const Name xi("Xi");

    CHECK_EQUAL("Xi", xi.plain());
    CHECK_EQUAL("\\Xi", xi.tex());
}

TEST(Name, texRepresentationOfPhi)
{
    const std::string expected("\\varphi");
    const Name phi("phi");

    CHECK_EQUAL(expected, phi.tex());
}

TEST(Name, comparisonOperators)
{
    const Name ab2("A", "b", "2");
    const Name ab3("A", "b", "3");
    const Name ac1("A", "c", "1");

    CHECK(ac1 == ac1);
    CHECK(ab2 != ab3);

    CHECK(ab2 < ac1);
    CHECK(ab2 <= ab3);
    CHECK(ab2 <= ab2);

    CHECK(ab3 > ab2);
    CHECK(ac1 >= ab3);
}

TEST(Name, twoNumeric)
{
    const Name n1("1");
    const Name n2("2");

    CHECK_EQUAL(n1, n1);
    CHECK(n1 < n2);
    CHECK_FALSE((n2 < n1));
}

TEST(Name, accessNumericName)
{
    const Name name("123");

    CHECK_EQUAL("123", name.plain());
}
