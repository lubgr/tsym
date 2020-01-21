#ifndef TSYM_PRINTENGINE_H
#define TSYM_PRINTENGINE_H

#include "int.h"

namespace tsym {
    struct Name;
}

namespace tsym {
    class PrintEngine {
      public:
        virtual ~PrintEngine() = default;

        virtual PrintEngine& symbol(const Name& name) = 0;
        virtual PrintEngine& positiveSymbol(const Name& name) = 0;
        virtual PrintEngine& functionName(const Name& name) = 0;
        virtual PrintEngine& number(double n) = 0;
        virtual PrintEngine& number(const Int& n) = 0;
        virtual PrintEngine& undefined() = 0;

        virtual PrintEngine& plusSign() = 0;
        virtual PrintEngine& minusSign() = 0;
        virtual PrintEngine& unaryMinusSign() = 0;
        virtual PrintEngine& timesSign() = 0;
        virtual PrintEngine& divisionSign() = 0;
        virtual PrintEngine& comma() = 0;

        virtual PrintEngine& openNumerator(bool numeratorIsSum = false) = 0;
        virtual PrintEngine& closeNumerator(bool numeratorWasSum = false) = 0;
        virtual PrintEngine& openDenominator(bool denominatorIsScalar = false) = 0;
        virtual PrintEngine& closeDenominator(bool denominatorWasScalar = false) = 0;

        virtual PrintEngine& openScalarExponent() = 0;
        virtual PrintEngine& closeScalarExponent() = 0;
        virtual PrintEngine& openCompositeExponent() = 0;
        virtual PrintEngine& closeCompositeExponent() = 0;

        virtual PrintEngine& openSquareRoot() = 0;
        virtual PrintEngine& closeSquareRoot() = 0;

        virtual PrintEngine& openParentheses() = 0;
        virtual PrintEngine& closeParentheses() = 0;
    };
}

#endif
