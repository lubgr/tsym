
#include <memory>
#include "fixtures.h"
#include "numeric.h"
#include "symbol.h"
#include "constant.h"
#include "logger.h"
#include "testsuitelogger.h"
#include "tsymtests.h"

tsym::AbcFixture::AbcFixture() :
    a{tsym::Symbol::create("a")},
    b{tsym::Symbol::create("b")},
    c{tsym::Symbol::create("c")},
    d{tsym::Symbol::create("d")},
    e{tsym::Symbol::create("e")},
    f{tsym::Symbol::create("f")},
    g{tsym::Symbol::create("g")},
    h{tsym::Symbol::create("h")},
    zero{tsym::Numeric::zero()},
    one{tsym::Numeric::one()},
    two{tsym::Numeric::two()},
    three{tsym::Numeric::three()},
    four{tsym::Numeric::four()},
    five{tsym::Numeric::create(5)},
    six{tsym::Numeric::create(6)},
    seven{tsym::Numeric::create(7)},
    eight{tsym::Numeric::create(8)},
    nine{tsym::Numeric::create(9)},
    ten{tsym::Numeric::create(10)},
    pi{tsym::Constant::createPi()}
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
