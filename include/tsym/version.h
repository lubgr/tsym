#ifndef TSYM_VERSION_H
#define TSYM_VERSION_H

namespace tsym {
    namespace version {
        constexpr int major();
        constexpr int minor();
        constexpr int patch();
        constexpr const char* compiler();
        constexpr const char* os();
        constexpr const char* configureTime();
    }
}

#endif
