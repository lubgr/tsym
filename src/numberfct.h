#ifndef TSYM_NUMBERFCT_H
#define TSYM_NUMBERFCT_H

namespace tsym {
    class Number;
}

namespace tsym {
    bool isInt(const Number& n);
    bool isOddInt(const Number& n);
    bool isEvenInt(const Number& n);
    bool isFraction(const Number& n);
    Number abs(const Number& n);
    /* Returns 0 in case of a zero number, otherwise -1 or 1: */
    int sign(const Number& n);
}

#endif
