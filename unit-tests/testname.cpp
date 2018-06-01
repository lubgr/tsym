
#include "name.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_AUTO_TEST_SUITE(TestName)

BOOST_AUTO_TEST_CASE(emptyName)
{
    const Name empty("");

    BOOST_TEST(empty.plain().empty());
    BOOST_TEST(empty.unicode().empty());
    BOOST_TEST(empty.tex().empty());
}

BOOST_AUTO_TEST_CASE(simpleString)
{
    const std::string varName("abc");
    const Name a(varName);

    BOOST_CHECK_EQUAL(varName, a.plain());
    BOOST_CHECK_EQUAL(varName, a.tex());
}

BOOST_AUTO_TEST_CASE(varWithSubscriptChar)
{
    const std::string expected("U_x");
    const Name n("U", "x");

    BOOST_CHECK_EQUAL(expected, n.plain());
    BOOST_CHECK_EQUAL(expected, n.tex());
}

BOOST_AUTO_TEST_CASE(varWithSubscriptString)
{
    const std::string expectedPlain("U_xyz");
    const std::string expectedTex("U_{xyz}");
    const Name n("U", "xyz");

    BOOST_CHECK_EQUAL(expectedPlain, n.plain());
    BOOST_CHECK_EQUAL(expectedTex, n.tex());
}

BOOST_AUTO_TEST_CASE(varWithSubAndSuperscriptChar)
{
    const std::string expectedPlain("ABC_d_e");
    const std::string expectedTex("ABC_d^e");
    const Name n("ABC", "d", "e");

    BOOST_CHECK_EQUAL(expectedPlain, n.plain());
    BOOST_CHECK_EQUAL(expectedTex, n.tex());
}

BOOST_AUTO_TEST_CASE(varWithSubCharAndSuperString)
{
    const std::string expectedPlain("ABC_d_efg");
    const std::string expectedTex("ABC_d^{efg}");
    const Name n("ABC", "d", "efg");

    BOOST_CHECK_EQUAL(expectedPlain, n.plain());
    BOOST_CHECK_EQUAL(expectedTex, n.tex());
}

BOOST_AUTO_TEST_CASE(greekLowerCaseLetter)
{
    const Name gamma("gamma");

    BOOST_CHECK_EQUAL("gamma", gamma.plain());
    BOOST_CHECK_EQUAL("\\gamma", gamma.tex());
}

BOOST_AUTO_TEST_CASE(greekUpperCaseLetter)
{
    const Name xi("Xi");

    BOOST_CHECK_EQUAL("Xi", xi.plain());
    BOOST_CHECK_EQUAL("\\Xi", xi.tex());
}

BOOST_AUTO_TEST_CASE(texRepresentationOfPhi)
{
    const std::string expected("\\varphi");
    const Name phi("phi");

    BOOST_CHECK_EQUAL(expected, phi.tex());
}

BOOST_AUTO_TEST_CASE(comparisonOperators)
{
    const Name ab2("A", "b", "2");
    const Name ab3("A", "b", "3");
    const Name ac1("A", "c", "1");

    BOOST_TEST(ac1 == ac1);
    BOOST_TEST(ab2 != ab3);

    BOOST_TEST(ab2 < ac1);
    BOOST_TEST(ab2 <= ab3);
    BOOST_TEST(ab2 <= ab2);

    BOOST_TEST(ab3 > ab2);
    BOOST_TEST(ac1 >= ab3);
}

BOOST_AUTO_TEST_CASE(twoNumeric)
{
    const Name n1("1");
    const Name n2("2");

    BOOST_CHECK_EQUAL(n1, n1);
    BOOST_TEST(n1 < n2);
    BOOST_TEST(!(n2 < n1));
}

BOOST_AUTO_TEST_CASE(accessNumericName)
{
    const Name name("123");

    BOOST_CHECK_EQUAL("123", name.plain());
}

BOOST_AUTO_TEST_CASE(streamOperator)
{
    const Name name("abc", "d");
    std::ostringstream stream;

    stream << name;

    BOOST_CHECK_EQUAL("abc_d", stream.str());
}

BOOST_AUTO_TEST_SUITE_END()
