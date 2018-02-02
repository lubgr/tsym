#ifndef TSYM_TRIGONOMETRIC_H
#define TSYM_TRIGONOMETRIC_H

#include "function.h"

namespace tsym {
    class Trigonometric : public Function {
        /* Includes the implementation and simplification of sine, cosine, tangent, arcsine,
         * arccosine and arctangent(2). If the argument is one of these trigonometric functions, the
         * simplification procedure may return the proper trigonometric identity. Numerical
         * arguments are resolved if possible, e.g. by a double Numeric or by a Power expression
         * (e.g. sin(Pi/4) = 1/sqrt(2)). In addition, inverse trigonometric functions of their
         * counterparts are resolved for numerically evaluable arguments, e.g. asin(sin(1/2)) = 1/2
         * or acos(cos(11/3*pi - sqrt(2))) = pi/3 + sqrt(2). */
        public:
            static BasePtr createSin(const BasePtr& arg);
            static BasePtr createCos(const BasePtr& arg);
            static BasePtr createTan(const BasePtr& arg);
            static BasePtr createAsin(const BasePtr& arg);
            static BasePtr createAcos(const BasePtr& arg);
            static BasePtr createAtan(const BasePtr& arg);
            static BasePtr createAtan2(const BasePtr& y, const BasePtr& x);

            /* Accessed from NumTrigoSimpl, ProductSimpl and possibly more (thus public): */
            enum class Type { SIN, COS, TAN, ASIN, ACOS, ATAN, ATAN2 };

            Trigonometric(const BasePtrList& args, Type type, Base::CtorKey&&);
            Trigonometric(const Trigonometric&) = delete;
            Trigonometric& operator = (const Trigonometric&) = delete;
            Trigonometric(Trigonometric&&) = delete;
            Trigonometric& operator = (Trigonometric&&) = delete;
            ~Trigonometric() = default;

            Number numericEval() const;
            Fraction normal(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& symbol) const;
            BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            bool isPositive() const;
            bool isNegative() const;
            unsigned complexity() const;

        private:
            static BasePtr create(Type type, const BasePtr& arg);
            static BasePtr createInstance(Type type, const BasePtrList& args);
            static bool doesSymmetryApply(const BasePtr& arg);
            static bool haveAllNegativePrefactors(const BasePtrList& operands);
            static BasePtr createBySymmetry(Type type, const BasePtr& negativeArg);
            static BasePtr createNumerically(Type type, const BasePtr& arg);
            static BasePtr createNumericallyBySymmetry(Type type, const BasePtr& arg);
            static BasePtr createFromFunction(Type type, const BasePtr& arg);
            static const Trigonometric *tryCast(const BasePtr& arg);
            static BasePtr createFromTrigo(Type type, const BasePtr& arg);
            static bool isOtherTheInverse(Type type, Type otherType);
            static bool isTanOfAtan2(Type type, Type otherType);
            static bool isThisTheInverse(Type type, Type otherType);
            static BasePtr shiftArgIntoRange(Type type, BasePtr arg);
            static void defineIntervalAndEndFactor(Type type, BasePtr *interval, BasePtr& factor);
            static BasePtr createFromTrigoNoInverse(Type type, const BasePtr& arg);
            static BasePtr createAtan2Numerically(const BasePtr& y, const BasePtr& x);
            static BasePtr simplAtan2(const BasePtr& y, const BasePtr& x, const BasePtr& increment);
            static BasePtr shiftAtanResultIntoRange(BasePtr result, BasePtr summand);
            static std::string getStr(Type type);

            Number checkedNumericEval() const;
            Fraction normalAtan2(SymbolMap& map) const;
            Fraction normalOtherThanAtan2(SymbolMap& map) const;
            BasePtr diffWrtSymbol(const BasePtr& arg, const BasePtr& symbol) const;
            BasePtr innerDiff() const;

            const BasePtr& arg1;
            const BasePtr& arg2;
            Type type;
    };
}

#endif
