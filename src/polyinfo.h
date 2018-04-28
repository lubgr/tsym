#ifndef TSYM_POLYINFO_H
#define TSYM_POLYINFO_H

#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    class PolyInfo {
        /* Utility class for operations on two multivariate polynomials. */
      public:
        PolyInfo() = default;
        PolyInfo(const BasePtr& u, const BasePtr& v);

        void set(const BasePtr& u, const BasePtr& v);

        /* A valid input is the following: rational Numerics, Symbols or combinations of those
         * types, i.e., Sum, Product or Power: */
        bool isInputValid() const;
        /* The first element is the main symbol: */
        const BasePtrList& listOfSymbols();
        /* The main symbol is the variable with least degree contained in u and v. If there is
         * no common symbol, Undefined is returned: */
        BasePtr mainSymbol();

      private:
        static bool hasValidType(const BasePtr& ptr);
        static bool isValidPower(const BasePtr& power);
        static bool hasValidOperands(const BasePtr& ptr);
        void defineSymbolListIfRequired();
        void addSymbols(const BasePtr& ptr);
        void addIfNotAlreadyStored(const BasePtr& symbol);
        void addSymbolsNonScalar(const BasePtr& ptr);
        void addSymbols(const BasePtrList& list);
        bool hasCommonSymbol() const;

        BasePtr u{};
        BasePtr v{};
        BasePtrList symbolList{};
        bool needsUpdate = true;
    };
}

#endif
