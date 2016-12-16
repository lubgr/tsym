#ifndef TSYM_SYMBOLREGISTRY_H
#define TSYM_SYMBOLREGISTRY_H

#include "name.h"

namespace tsym {
    class SymbolRegistry {
        /* All symbol names are registered here, mainly for a safe creation of unique temporary
         * symbols. As Symbol objects are passed around as Base pointer wrapped inside the BasePtr
         * class, the registry can only be correctly used from within BasePtr (copy/assignment)
         * constructor and its destructor. Thus, inserting and removing symbols from the registry is
         * made private and accessible only to BasePtr. */
        friend class BasePtr;

        private:
            static void add(const Name& symbolName);
            static void remove(const Name& symbolName);

        public:
            static unsigned count(const Name& symbolName);
    };
}

#endif
