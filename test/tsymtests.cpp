
#include <sstream>
#include "base.h"
#include "vector.h"
#include "var.h"
#include "matrix.h"
#include "testsuitelogger.h"
#include "tsymtests.h"

using namespace tsym;

bool operator == (const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isEqual(rhs);
}

bool operator != (const BasePtr& lhs, const BasePtr& rhs)
{
    return lhs->isDifferent(rhs);
}

template<> SimpleString StringFrom(const Matrix& m)
{
    std::ostringstream stream;

    for (size_t i = 0; i != m.rowSize(); ++i)
        for (size_t j = 0; j != m.colSize(); ++j)
            stream << "(" << i << ", " << j << "): " << m(i, j) << std::endl;

    return  stream.str().c_str();
}

template<> SimpleString StringFrom(const Vector& v)
{
    std::ostringstream stream;

    for (size_t i = 0; i != v.size(); ++i)
        stream << "(" << i << "): " << v(i) << std::endl;

    return  stream.str().c_str();
}

void disableLog()
{
    TSYM_INFO("Following warnings/errors are probably an intended part of testing");

    TestSuiteLogger::disableStdout();
}

void enableLog()
{
    TSYM_INFO("Warnings/errors aren't intended from now on!");

    TestSuiteLogger::enableStdout();
}
