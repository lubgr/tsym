#ifndef TSYM_PRINTER_H
#define TSYM_PRINTER_H

namespace tsym {
    class Number;
    class Base;
    class PrintEngine;
}

namespace tsym {
    void print(PrintEngine& engine, const Number& number);
    void print(PrintEngine& engine, const Base& base);
    /* Account for to the actual representation, not prettiness: */
    void printDebug(PrintEngine& engine, const Base& base);
}

#endif
