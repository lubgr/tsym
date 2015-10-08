#ifndef TSYM_LOGGING_H
#define TSYM_LOGGING_H

#ifdef USE_TRLOG

#include "trlog/trlog.h"

namespace tsym {
    namespace logging {
        trlog::Stream debug();
        trlog::Stream info();
        trlog::Stream warning();
        trlog::Stream error();
        /* Note that trlog calls std::exit(1) after printing the message: */
        trlog::Stream fatal();
    }
}

#else

#include <sstream>
#include <iostream>

namespace tsym {
    namespace logging {
        std::ostream& debug();
        std::ostream& info();
        std::ostream& warning();
        std::ostream& error();
        /* The program doesn't exit within this function: */
        std::ostream& fatal();

        enum Level { FATAL = 0, ERROR, WARNING, INFO, DEBUG };
        extern Level verbosity;
    }
}

#endif

#endif
