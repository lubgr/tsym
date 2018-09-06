#ifndef TSYM_NAMEFCT_H
#define TSYM_NAMEFCT_H

#include <string>

namespace tsym {
    class Name;
}

namespace tsym {
    std::string unicode(const Name& name);
    std::string tex(const Name& name);
}

#endif
