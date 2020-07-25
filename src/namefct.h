#ifndef TSYM_NAMEFCT_H
#define TSYM_NAMEFCT_H

#include <string>

namespace tsym {
    struct Name;
    struct NameView;
}

namespace tsym {
    std::string unicode(NameView name);
    std::string tex(NameView name);
    std::string concat(NameView name);
}

#endif
