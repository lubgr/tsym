#ifndef TSYM_PRINTER_H
#define TSYM_PRINTER_H

#include "baseptr.h"

namespace tsym {
    class Number;
}

namespace tsym {
    namespace printer {
        template <class Engine> void print(Engine& engine, const Number& number);
        template <class Engine> void print(Engine& engine, const Base& base);
        template <class Engine> void print(Engine& engine, const BasePtr& ptr);
        /* Account for to the actual representation, not prettiness: */
        template <class Engine> void printDebug(Engine& engine, const Base& base);
    }
}

#endif
