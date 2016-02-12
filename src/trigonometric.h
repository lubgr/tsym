#ifndef TSYM_TRIGONOMETRIC_H
#define TSYM_TRIGONOMETRIC_H

#include "function.h"

namespace tsym {
    class Trigonometric : public Function {
        /* Includes the implementation and simplification of sine, cosine, tangent, arcsine,
         * arccosine and arctangent. If the argument is one of these trigonometric functions, the
         * simplification procedure may return the proper trigonometric identity. Numerical
         * arguments are resolved if possible, e.g. by a double Numeric or by a Power expression
         * (e.g. sin(Pi/4) = 1/sqrt(2)). */
        public:
            static BasePtr createSin(const BasePtr& arg);
            static BasePtr createCos(const BasePtr& arg);
            static BasePtr createTan(const BasePtr& arg);
            static BasePtr createAsin(const BasePtr& arg);
            static BasePtr createAcos(const BasePtr& arg);
            static BasePtr createAtan(const BasePtr& arg);

            /* This enum is accessed from NumTrigoSimpl and is thus public: */
            enum Type { SIN, COS, TAN, ASIN, ACOS, ATAN };

            /* Implentations of pure virtual methods of Base. */
            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;

        private:
            Trigonometric(const BasePtr& arg, Type type);
            Trigonometric(const Trigonometric& other);
            Trigonometric& operator = (const Trigonometric& other);
            ~Trigonometric();

            static BasePtr create(Type type, const BasePtr& arg);
            static BasePtr createNumerically(Type type, const BasePtr& arg);
            static BasePtr createFromFunction(Type type, const BasePtr& arg);
            static const Trigonometric *tryCast(const BasePtr& arg);
            static BasePtr createFromTrigo(Type type, const BasePtr& arg);
            static bool isOtherTheInverse(Type type, Type otherType);
            static bool isThisTheInverse(Type type, Type otherType);
            static BasePtr createFromTrigoNoInverse(Type type, const BasePtr& arg);
            static std::string getStr(Type type);

            Number checkedNumericEval() const;
            BasePtr innerDiff() const;

            Type type;
    };
}

#endif
