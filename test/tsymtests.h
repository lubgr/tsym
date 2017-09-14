#ifndef TEST_TSYMTESTS_H
#define TEST_TSYMTESTS_H

/* Main header to be included in each compilation unit with test cases. This file includes the main
 * header of CppUTest and provides common definitions of functions necessary to work with
 * assertions, suppress warning logs (if intended) and the like. */

#include <sstream>
#include <string>
#include "logging.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/SimpleString.h"

/* Function necessary for using the CHECK_EQUAL macro of CppUTest, i.e. for printing out error
 * messages when tests with this assertion fail: */
template<class T> SimpleString StringFrom(const T& obj)
{
    std::stringstream stream;

    stream << obj;

    return StringFrom(stream.str());
}

/* For using the CHECK_EQUAL macro on BasePtr objects: */
namespace tsym { class BasePtr; }
bool operator == (const tsym::BasePtr& lhs, const tsym::BasePtr& rhs);
bool operator != (const tsym::BasePtr& lhs, const tsym::BasePtr& rhs);

/* Functions to disable and enable logging messages except the fatal level. */
void disableLog();
void enableLog();

/* Macro to define a valid constructor for a CppUTest test group, which is after macro expansion
 * represented as a class. This macro is useful for initializing objects declared inside the
 * TEST_GROUP, which do not have a constructor without arguments: */
#define GROUP_CTOR(name) TEST_GROUP_CppUTestGroup ## name()

/* Add logging output before each test case. */

#define CPPUTEST_CONCAT(str) #str
#define CPPUTEST_QUOTE(str) CPPUTEST_CONCAT(str)

#undef TEST

#define TEST(testGroup, testName) \
class TEST_##testGroup##_##testName##_TestShell; \
extern \
TEST_##testGroup##_##testName##_TestShell TEST_##testGroup##_##testName##_TestShell_instance; \
class TEST_##testGroup##_##testName##_Test : public TEST_GROUP_##CppUTestGroup##testGroup \
{ public: TEST_##testGroup##_##testName##_Test () : TEST_GROUP_##CppUTestGroup##testGroup () \
{ TSYM_INFO("CppUTest - %s/%s", CPPUTEST_QUOTE(testGroup), CPPUTEST_QUOTE(testName) ); }\
void testBody(); }; \
class TEST_##testGroup##_##testName##_TestShell : public UtestShell \
{ virtual Utest* createTest() _override { return new TEST_##testGroup##_##testName##_Test; } } \
TEST_##testGroup##_##testName##_TestShell_instance; \
static TestInstaller TEST_##testGroup##_##testName##_Installer( \
    TEST_##testGroup##_##testName##_TestShell_instance, #testGroup, #testName, __FILE__,__LINE__); \
    void TEST_##testGroup##_##testName##_Test::testBody()
#endif
