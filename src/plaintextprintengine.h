#ifndef TSYM_PLAINTEXTPRINTENGINE_H
#define TSYM_PLAINTEXTPRINTENGINE_H

#include "int.h"
#include "printengine.h"

namespace tsym {
    class Name;
}

namespace tsym {
    class PlaintextPrintEngine : public PrintEngine<PlaintextPrintEngine> {
      public:
        enum class CharSet { ASCII, UNICODE };

        explicit PlaintextPrintEngine(std::ostream& out, CharSet charset = CharSet::UNICODE);
        PlaintextPrintEngine(const PlaintextPrintEngine& other) = delete;
        PlaintextPrintEngine& operator=(const PlaintextPrintEngine& rhs) = delete;
        PlaintextPrintEngine(PlaintextPrintEngine&& other) = default;
        PlaintextPrintEngine& operator=(PlaintextPrintEngine&& rhs) = default;

        PlaintextPrintEngine& symbol(const Name& name);
        PlaintextPrintEngine& positiveSymbol(const Name& name);
        PlaintextPrintEngine& functionName(const Name& name);
        PlaintextPrintEngine& number(double n);
        PlaintextPrintEngine& number(const Int& n);
        PlaintextPrintEngine& undefined();

        PlaintextPrintEngine& plusSign();
        PlaintextPrintEngine& minusSign();
        PlaintextPrintEngine& unaryMinusSign();
        PlaintextPrintEngine& timesSign();
        PlaintextPrintEngine& divisionSign();
        PlaintextPrintEngine& comma();

        PlaintextPrintEngine& openNumerator(bool numeratorIsSum = false);
        PlaintextPrintEngine& closeNumerator(bool numeratorWasSum = false);
        PlaintextPrintEngine& openDenominator(bool denominatorIsScalar = false);
        PlaintextPrintEngine& closeDenominator(bool denominatorWasScalar = false);

        PlaintextPrintEngine& openScalarExponent();
        PlaintextPrintEngine& closeScalarExponent();
        PlaintextPrintEngine& openCompositeExponent();
        PlaintextPrintEngine& closeCompositeExponent();

        PlaintextPrintEngine& openSquareRoot();
        PlaintextPrintEngine& closeSquareRoot();

        PlaintextPrintEngine& openParentheses();
        PlaintextPrintEngine& closeParentheses();

      private:
        const CharSet charset;
    };
}

#endif
