#ifndef TSYM_PRINTENGINE_H
#define TSYM_PRINTENGINE_H

#include <string_view>

namespace tsym {
    class PrintEngine {
      public:
        virtual ~PrintEngine() = default;

        struct Name {
            std::string_view value;
            std::string_view subscript;
            std::string_view superscript;
        };

        virtual PrintEngine& symbol(std::string_view name, std::string_view sub, std::string_view super) = 0;
        virtual PrintEngine& positiveSymbol(std::string_view name, std::string_view sub, std::string_view super) = 0;
        virtual PrintEngine& functionName(std::string_view name) = 0;
        virtual PrintEngine& floatingPoint(double n) = 0;
        virtual PrintEngine& integer(long long n) = 0;
        virtual PrintEngine& integer(std::string_view n) = 0;
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
