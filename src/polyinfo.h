#ifndef TSYM_POLYINFO_H
#define TSYM_POLYINFO_H

#include "baseptr.h"
#include "baseptrlist.h"

namespace tsym {
    /* Utility functions for operations on two multivariate polynomials. */
    namespace poly {
        /* A valid input is the following: rational Numerics, Symbols or combinations of those
         * types, i.e., Sum, Product or Power: */
        bool isInputValid(const Base& u, const Base& v);
        /* The first element is the main symbol: */
        BasePtrList listOfSymbols(const Base& u, const Base& v);
        /* The main symbol is the variable with least degree contained in u and v. If there is
         * no common symbol, Undefined is returned: */
        BasePtr mainSymbol(const BasePtrList& symbolList, const Base& u, const Base& v);
    }
}

#endif
