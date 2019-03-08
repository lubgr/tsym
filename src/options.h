#ifndef TSYM_OPTIONS_H
#define TSYM_OPTIONS_H

#include "int.h"

namespace tsym {
    namespace options {
        const Int& getMaxPrimeResolution();
        void setMaxPrimeResolution(Int max);
    }
}

#endif
