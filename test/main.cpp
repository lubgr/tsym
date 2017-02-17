
#include "printer.h"
#include "numeric.h"
#include "logarithm.h"
#include "trigonometric.h"
#include "constant.h"
#include "matrix.h"
#include "poly.h"
#include "name.h"
#include "abc.h"
#include "plic/plic.h"
#include "CppUTest/CommandLineTestRunner.h"

void initConstructOnFirstUse()
    /* This initialization ensures that CppUTest doesn't spot a memory leak for the first test cases
     * that call the following functions employing local static variables. */
{
    const tsym::Matrix emptyMatrix;
    const tsym::Vector emptyVec;
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

    plic::debug("CppUTest", "initialize plic local static variables by %%");
}

int main(int argc, char** argv)
{
    initConstructOnFirstUse();

    tsym::Printer::disableFractions();

    plic::configFile("test/logconfig.py");

    return CommandLineTestRunner::RunAllTests(argc, argv);
}
