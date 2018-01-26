
#include <memory>
#include "numeric.h"
#include "logarithm.h"
#include "trigonometric.h"
#include "numtrigosimpl.h"
#include "baseptrctr.h"
#include "constant.h"
#include "matrix.h"
#include "vector.h"
#include "var.h"
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
    auto logger = std::make_unique<const TestSuiteLogger>(globalSuppressLogFlag());

    tsym::Logger::setInstance(std::move(logger));

    disableLog();
    initConstructOnFirstUse();
    enableLog();

    return CommandLineTestRunner::RunAllTests(argc, argv);
}
