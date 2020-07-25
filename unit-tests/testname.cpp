
#include "name.h"
#include "namefct.h"
#include "tsymtests.h"

using namespace tsym;

BOOST_AUTO_TEST_SUITE(TestName)

BOOST_AUTO_TEST_CASE(emptyName)
{
    const Name empty{""};

    BOOST_TEST(concat(empty).empty());
    BOOST_TEST(unicode(empty).empty());
    BOOST_TEST(tex(empty).empty());
}

BOOST_AUTO_TEST_CASE(simpleString)
{
    const std::string varName("abc");
    const Name a{varName};

    BOOST_CHECK_EQUAL(varName, concat(a));
    BOOST_CHECK_EQUAL(varName, tex(a));
}

BOOST_AUTO_TEST_CASE(varWithSubscriptChar)
{
    const std::string expected("U_x");
    const Name n{"U", "x"};

    BOOST_CHECK_EQUAL(expected, concat(n));
    BOOST_CHECK_EQUAL(expected, tex(n));
}

BOOST_AUTO_TEST_CASE(varWithSubscriptString)
{
    const std::string expected("U_{xyz}");
    const Name n{"U", "xyz"};

    BOOST_CHECK_EQUAL(expected, concat(n));
    BOOST_CHECK_EQUAL(expected, tex(n));
}

BOOST_AUTO_TEST_CASE(varWithSubAndSuperscriptChar)
{
    const std::string expected("ABC_d^e");
    const Name n{"ABC", "d", "e"};

    BOOST_CHECK_EQUAL(expected, concat(n));
    BOOST_CHECK_EQUAL(expected, tex(n));
}

BOOST_AUTO_TEST_CASE(varWithSubCharAndSuperString)
{
    const std::string expected("ABC_d^{efg}");
    const Name n{"ABC", "d", "efg"};

    BOOST_CHECK_EQUAL(expected, concat(n));
    BOOST_CHECK_EQUAL(expected, tex(n));
}

BOOST_AUTO_TEST_CASE(greekLowerCaseLetter)
{
    const Name gamma{"gamma"};

    BOOST_CHECK_EQUAL("gamma", concat(gamma));
    BOOST_CHECK_EQUAL("\\gamma", tex(gamma));
}

BOOST_AUTO_TEST_CASE(greekUpperCaseLetter)
{
    const Name xi{"Xi"};

    BOOST_CHECK_EQUAL("Xi", concat(xi));
    BOOST_CHECK_EQUAL("\\Xi", tex(xi));
}

BOOST_AUTO_TEST_CASE(texRepresentationOfPhi)
{
    const std::string expected("\\varphi");
    const Name phi{"phi"};

    BOOST_CHECK_EQUAL(expected, tex(phi));
}

BOOST_AUTO_TEST_CASE(comparisonOperators)
{
    const Name ab2{"A", "b", "2"};
    const Name ab3{"A", "b", "3"};
    const Name ac1{"A", "c", "1"};

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
    const Name n1{"1"};
    const Name n2{"2"};

    BOOST_CHECK_EQUAL(n1, n1);
    BOOST_TEST(n1 < n2);
    BOOST_TEST(!(n2 < n1));
}

BOOST_AUTO_TEST_CASE(accessNumericName)
{
    const Name name{"123"};

    BOOST_CHECK_EQUAL("123", concat(name));
}

BOOST_AUTO_TEST_CASE(streamOperator)
{
    const Name name{"abc", "d"};
    std::ostringstream stream;

    stream << name;

    BOOST_CHECK_EQUAL("abc_d", stream.str());
}

BOOST_AUTO_TEST_SUITE_END()
