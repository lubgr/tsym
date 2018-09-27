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

    bool isConstant(const Base& expr);
    bool isFunction(const Base& expr);
    bool isNumeric(const Base& expr);
    bool isPower(const Base& expr);
    bool isProduct(const Base& expr);
    bool isSum(const Base& expr);
    bool isSymbol(const Base& expr);
    bool isUndefined(const Base& expr);

    bool isNumericPower(const Base& expr);
    bool isFractionNumeric(const Base& arg);
    bool isRationalNumeric(const Base& expr);
    bool isDoubleNumeric(const Base& expr);
    bool isNegativeNumeric(const Base& ptr);
}

#endif
