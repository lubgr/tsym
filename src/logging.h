#ifndef TSYM_LOGGING_H
#define TSYM_LOGGING_H

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

#endif
