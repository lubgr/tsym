#ifndef TSYM_POLYINFO_H
#define TSYM_POLYINFO_H

#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    class PolyInfo {
        /* Utility class for operations on two multivariate polynomials. */
      public:
        PolyInfo(const Base& u, const Base& v);

        /* A valid input is the following: rational Numerics, Symbols or combinations of those
         * types, i.e., Sum, Product or Power: */
        bool isInputValid() const;
        /* The first element is the main symbol: */
        BasePtrList listOfSymbols();
        /* The main symbol is the variable with least degree contained in u and v. If there is
         * no common symbol, Undefined is returned: */
        BasePtr mainSymbol();

      private:
        static bool hasValidType(const Base& arg);
        static bool isValidPower(const Base& power);
        static bool hasValidOperands(const Base& arg);
        void addSymbols(const Base& arg);
        void addIfNotAlreadyStored(const Base& symbol);
        void addSymbolsNonScalar(const Base& arg);
        void addSymbols(const BasePtrList& list);
        bool hasCommonSymbol() const;

        const Base& u;
        const Base& v;
        BasePtrList symbolList{};
    };
}

#endif
