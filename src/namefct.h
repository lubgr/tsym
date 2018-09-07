#ifndef TSYM_NAMEFCT_H
#define TSYM_NAMEFCT_H

#include <string>

namespace tsym {
    struct Name;
}

namespace tsym {
    std::string unicode(const Name& name);
    std::string tex(const Name& name);
    std::string concat(const Name& name, char delimiter = '_');
}

#endif
