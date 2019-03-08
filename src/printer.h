#ifndef TSYM_PRINTER_H
#define TSYM_PRINTER_H

#include "baseptr.h"

namespace tsym {
    class Number;
}

namespace tsym {
    template <class Engine> void print(Engine& engine, const Number& number);
    template <class Engine> void print(Engine& engine, const Base& base);
    /* Account for to the actual representation, not prettiness: */
    template <class Engine> void printDebug(Engine& engine, const Base& base);
}

#endif
