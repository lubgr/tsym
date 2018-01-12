
#include <memory>
#include "numeric.h"
#include "logarithm.h"
#include "trigonometric.h"
#include "numtrigosimpl.h"
#include "baseptrlist.h"
#include "constant.h"
#include "matrix.h"
#include "poly.h"
#include "name.h"
#include "abc.h"
#include "logging.h"
#include "testsuitelogger.h"
#include "tsymtests.h"
#include "CppUTest/CommandLineTestRunner.h"

void initConstructOnFirstUse()
    /* This initialization ensures that CppUTest doesn't spot a memory leak for the first test cases
     * that call the following functions employing local static variables. */
{
    const tsym::BasePtrList emptyList;
    const tsym::Matrix emptyMatrix;
    const tsym::Vector emptyVec;
    tsym::NumTrigoSimpl nts;
    tsym::BasePtr undefined;
    tsym::Number n(1);

    n /= n;
    n -= n;

    tsym::Name("dummy").tex();

    tsym::Numeric::zero();
    tsym::Numeric::one();
    tsym::Numeric::mOne();

    tsym::Logarithm::create(tsym::Constant::createE());

    tsym::Trigonometric::createAtan2(zero, two);

    tsym::poly::gcd(tsym::Numeric::one(), tsym::Numeric::one());

    emptyList.front();
    emptyList.back();

    emptyMatrix(1, 1);
    emptyVec(1);

    nts.setType(tsym::Trigonometric::Type::SIN);
    nts.setArg(tsym::Numeric::zero());
    nts.compute();
    nts.setType(tsym::Trigonometric::Type::TAN);
    nts.compute();

    tsym::Var("a").type();
}

int main(int argc, char** argv)
{
    auto logger = std::make_shared<const TestSuiteLogger>("misc/test-logfiles/debug.log",
            "misc/test-logfiles/info.log");

    tsym::Logger::setInstance(logger);

    disableLog();
    initConstructOnFirstUse();
    enableLog();

    return CommandLineTestRunner::RunAllTests(argc, argv);
}
