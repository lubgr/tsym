
#include <cstdlib>
#include "logging.h"

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
