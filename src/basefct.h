#ifndef TSYM_BASEFCT_H
#define TSYM_BASEFCT_H

namespace tsym {
    class Base;
}

namespace tsym {
    bool isInteger(const Base& expr);

    /* Returns true only if the argument is a Numeric type, i.e., not for function instances that
     * could be evaluated to approx. zero/one: */
    bool isOne(const Base& expr);
    bool isZero(const Base& expr);

    bool isNumericPower(const Base& expr);
    bool isFractionNumeric(const Base& arg);
    bool isRationalNumeric(const Base& expr);
    bool isDoubleNumeric(const Base& expr);
    bool isNegativeNumeric(const Base& ptr);
}

#endif
