#ifndef TSYM_PLAINTEXTPRINTENGINE_H
#define TSYM_PLAINTEXTPRINTENGINE_H

#include <ostream>
#include <functional>
#include "baseptr.h"

namespace tsym {
    class Name;
    class Int;
}

namespace tsym {
    class PlaintextPrintEngine {
        public:
            enum class CharSet { ASCII, UNICODE };

            PlaintextPrintEngine(std::ostream& out, CharSet charset = CharSet::UNICODE);
            PlaintextPrintEngine(const PlaintextPrintEngine& other) = delete;
            PlaintextPrintEngine& operator = (const PlaintextPrintEngine& rhs) = delete;
            PlaintextPrintEngine(PlaintextPrintEngine&& other) = default;
            PlaintextPrintEngine& operator = (PlaintextPrintEngine&& rhs) = default;

            void registerToplevelPrintFunction(std::function<void(PlaintextPrintEngine&, const BasePtr&)> function);
            PlaintextPrintEngine& invokePrint(const BasePtr& ptr);

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
            std::ostream& out;
            const CharSet charset;
            std::function<void(PlaintextPrintEngine&, const BasePtr&)> printFunction;
    };
}

#endif
