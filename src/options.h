#ifndef TSYM_OPTION_H
#define TSYM_OPTION_H

#include "globals.h"
#include "logging.h"

namespace tsym {
    namespace options {
        bool areOptionsInitiated();
        void setDefaultOptions();
        template<class T> const T& get(Option key);
    }
}

#endif
