#ifndef TSYM_PARSERADAPTER_H
#define TSYM_PARSERADAPTER_H

#ifdef __cplusplus
#include "baseptr.h"

namespace tsym {
    namespace parserAdapter {
        /* Functions that interface the C code generated for parsing and lexical analysis. They will
         * be called from C++ parts only. */
        BasePtr parse(const char *string);
        void clearErrors();
        const std::vector<std::string>& getErrors();
        /* Returns 0, if no errors are encountered: */
        unsigned getFirstErrorIndex();
    }
}

extern "C" {
#endif

    /* Functions to create and delete expressions from primitives or other expressions and to
     * register errors for use within the C++ code. These functions are called by the parser in
     * order to handle its internal stack within pure C code (thus, evil void* pointer instead of
     * the underlying C++ objects of type BasePtr). */
    void *tsym_parserAdapter_createInteger(long value);
    void *tsym_parserAdapter_createMaxInt(const char *errorMessage);
    void *tsym_parserAdapter_createDouble(double value);
    void *tsym_parserAdapter_createMaxDouble(const char *errorMessage);
    void *tsym_parserAdapter_createSymbol(const char *name);
    void *tsym_parserAdapter_createPi();
    void *tsym_parserAdapter_createEuler();
    void *tsym_parserAdapter_createSum(const void *summand1, const void *summand2);
    void *tsym_parserAdapter_createDifference(const void *summand1, const void *summand2);
    void *tsym_parserAdapter_createMinus(const void *arg);
    void *tsym_parserAdapter_createProduct(const void *factor1, const void *factor2);
    void *tsym_parserAdapter_createQuotient(const void *factor1, const void *factor2);
    void *tsym_parserAdapter_createPower(const void *base, const void *exp);
    void *tsym_parserAdapter_createSine(const void *arg);
    void *tsym_parserAdapter_createCosine(const void *arg);
    void *tsym_parserAdapter_createTangent(const void *arg);
    void *tsym_parserAdapter_createAsine(const void *arg);
    void *tsym_parserAdapter_createAcosine(const void *arg);
    void *tsym_parserAdapter_createAtangent(const void *arg);
    void *tsym_parserAdapter_createAtangent2(const void *arg1, const void *arg2);
    void *tsym_parserAdapter_createLogarithm(const void *arg);
    void *tsym_parserAdapter_createSquareRoot(const void *arg);
    void tsym_parserAdapter_deletePtrs(void *ptr1, void *ptr2);
    void tsym_parserAdapter_deletePtr(void *ptr);
    void tsym_parserAdapter_logParsingError(const char *message, char *yytext);

#ifdef __cplusplus
}
#endif

#endif
