#ifndef TSYM_PLAINTEXTPRINTENGINE_H
#define TSYM_PLAINTEXTPRINTENGINE_H

#include <iosfwd>
#include "printengine.h"

namespace tsym {
    class PlaintextPrintEngine : public PrintEngine {
      public:
        enum class CharSet { ASCII, UNICODE };

        explicit PlaintextPrintEngine(std::ostream& out, CharSet charset = CharSet::UNICODE);

        PrintEngine& symbol(std::string_view name, std::string_view sub, std::string_view super) override;
        PrintEngine& positiveSymbol(std::string_view name, std::string_view sub, std::string_view super) override;
        PrintEngine& functionName(std::string_view name) override;
        PrintEngine& floatingPoint(double n) override;
        PrintEngine& integer(long long n) override;
        PrintEngine& integer(std::string_view n) override;
        PrintEngine& undefined() override;

        PrintEngine& plusSign() override;
        PrintEngine& minusSign() override;
        PrintEngine& unaryMinusSign() override;
        PrintEngine& timesSign() override;
        PrintEngine& divisionSign() override;
        PrintEngine& comma() override;

        PrintEngine& openNumerator(bool numeratorIsSum = false) override;
        PrintEngine& closeNumerator(bool numeratorWasSum = false) override;
        PrintEngine& openDenominator(bool denominatorIsScalar = false) override;
        PrintEngine& closeDenominator(bool denominatorWasScalar = false) override;

        PrintEngine& openScalarExponent() override;
        PrintEngine& closeScalarExponent() override;
        PrintEngine& openCompositeExponent() override;
        PrintEngine& closeCompositeExponent() override;

        PrintEngine& openSquareRoot() override;
        PrintEngine& closeSquareRoot() override;

        PrintEngine& openParentheses() override;
        PrintEngine& closeParentheses() override;

      protected:
        std::ostream& out;

      private:
        const CharSet charset;
    };
}

#endif
