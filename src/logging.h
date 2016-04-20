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

#include <cstdlib>
#include <iostream>

namespace tsym {
    namespace logging {
        class Stream {
            /* Simplistic logging stream that forwards stream operators to std::clog. */
            public:
                Stream(bool nullStream, bool exit);
                ~Stream();

                template<class T> const Stream& operator << (const T& rhs) const
                {
                    if (!isNullStream)
                        std::clog << rhs;

                    return *this;
                }

            private:
                bool isNullStream;
                bool exitAfterMessage;
        };

        Stream debug();
        Stream info();
        Stream warning();
        Stream error();
        Stream fatal();

        enum Level { FATAL = 0, ERROR, WARNING, INFO, DEBUG };
        extern Level verbosity;
    }
}

#endif

#endif
