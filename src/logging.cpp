
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
    log::Level logging::verbosity = WARNING;

    namespace {
        std::ostream& stream(logging::Level level)
        {
            static std::stringstream nullStream;

            if (level > logging::verbosity) {
                nullStream.str("");
                return nullStream;
            } else
                return std::clog;
        }
    }
}

std::ostream& tsym::logging::debug()
{
    return stream(DEBUG);
}

std::ostream& tsym::logging::info()
{
    return stream(INFO);
}

std::ostream& tsym::logging::warning()
{
    return stream(WARNING);
}

std::ostream& tsym::logging::error()
{
    return stream(ERROR);
}

std::ostream& tsym::logging::fatal()
{
    return stream(FATAL);
}

#endif
