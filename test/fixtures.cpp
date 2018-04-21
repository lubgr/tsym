
#include <memory>
#include "fixtures.h"
#include "numeric.h"
#include "symbol.h"
#include "undefined.h"
#include "constant.h"
#include "logger.h"
#include "testsuitelogger.h"
#include "tsymtests.h"

tsym::AbcFixture::AbcFixture() :
    a{Symbol::create("a")},
    b{Symbol::create("b")},
    c{Symbol::create("c")},
    d{Symbol::create("d")},
    e{Symbol::create("e")},
    f{Symbol::create("f")},
    g{Symbol::create("g")},
    h{Symbol::create("h")},
    zero{Numeric::zero()},
    one{Numeric::one()},
    two{Numeric::two()},
    three{Numeric::three()},
    four{Numeric::four()},
    five{Numeric::create(5)},
    six{Numeric::create(6)},
    seven{Numeric::create(7)},
    eight{Numeric::create(8)},
    nine{Numeric::create(9)},
    ten{Numeric::create(10)},
    pi{Constant::createPi()},
    undefined{Undefined::create()}
{}

tsym::TestSuiteLoggingFixture::TestSuiteLoggingFixture(bool suppressLogs)
{
    auto logger = std::make_unique<const TestSuiteLogger>(suppressLogs);

    Logger::setInstance(std::move(logger));
}

tsym::TestSuiteLoggingFixture::~TestSuiteLoggingFixture()
{
    auto logger = std::make_unique<const TestSuiteLogger>(false);

    Logger::setInstance(std::move(logger));
}
