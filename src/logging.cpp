
#include <cstdlib>
#include "logging.h"

#ifdef USE_TRLOG

trlog::Stream tsym::logging::debug()
{
    return trlog::debug("tsym");
}

trlog::Stream tsym::logging::info()
{
    return trlog::info("tsym");
}

trlog::Stream tsym::logging::warning()
{
    return trlog::warning("tsym");
}

trlog::Stream tsym::logging::error()
{
    return trlog::error("tsym");
}

trlog::Stream tsym::logging::fatal()
{
    return trlog::fatal("tsym");
}

#else

namespace tsym {
    /* Default behavior; don't show info or debug messages: */
    logging::Level logging::verbosity = WARNING;

    namespace {
        tsym::logging::Stream stream(logging::Level level)
        {
            if (level == logging::FATAL)
                return logging::Stream(false, true);
            else if (level > logging::verbosity)
                return logging::Stream(true, false);
            else
                return logging::Stream(false, false);
        }
    }
}

tsym::logging::Stream::Stream(bool nullStream, bool exit) :
    isNullStream(nullStream),
    exitAfterMessage(exit)
{}

tsym::logging::Stream::~Stream()
{
    if (!isNullStream)
        std::clog << std::endl;

    if (exitAfterMessage)
        std::exit(1);
}

tsym::logging::Stream tsym::logging::debug()
{
    return stream(DEBUG);
}

tsym::logging::Stream tsym::logging::info()
{
    return stream(INFO);
}

tsym::logging::Stream tsym::logging::warning()
{
    return stream(WARNING);
}

tsym::logging::Stream tsym::logging::error()
{
    return stream(ERROR);
}

tsym::logging::Stream tsym::logging::fatal()
{
    return stream(FATAL);
}

#endif
