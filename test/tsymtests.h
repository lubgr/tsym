#ifndef TEST_TSYMTESTS_H
#define TEST_TSYMTESTS_H

/* Main header to be included in each compilation unit with test cases. This file includes the main
 * header of CppUTest and provides common definitions of functions necessary to work with
 * assertions, suppress warning logs (if intended) and the like. */

#include <sstream>
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


/* For using the CHECK_EQUAL macro on BasePtr objects and the appropiate containers: */
#include "baseptr.h"
#include "baseptrlist.h"
bool operator == (const tsym::BasePtr& lhs, const tsym::BasePtr& rhs);
bool operator != (const tsym::BasePtr& lhs, const tsym::BasePtr& rhs);
bool operator == (const tsym::BasePtrList& lhs, const tsym::BasePtrList& rhs);
bool operator != (const tsym::BasePtrList& lhs, const tsym::BasePtrList& rhs);

/* Functions to disable and enable logging messages except the fatal level. */
void disableLog();
void enableLog();
bool& globalSuppressLogFlag();

#endif
