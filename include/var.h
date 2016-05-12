#ifndef TSYM_VAR_H
#define TSYM_VAR_H

#include <vector>
#include "baseptr.h"
#include "number.h"

namespace tsym {
    class Var {
        public:
            Var();
            Var(int value);
            Var(double value);
            Var(int numerator, int denominator);
            Var(const Number& number);
            explicit Var(const char *name, const char *subscript = "",
                    const char *superscript = "");
            explicit Var(const BasePtr& ptr);

            Var& operator += (const Var& rhs);
            Var& operator -= (const Var& rhs);
            Var& operator *= (const Var& rhs);
            Var& operator /= (const Var& rhs);

            const Var& operator + () const;
            Var operator - () const;

            Var toThe(const Var& exponent) const;

            Var subst(const Var& from, const Var& to) const;
            Var expand() const;
            Var normal() const;
            /* The argument must be a Symbol: */
            Var diff(const Var& symbol) const;

            bool equal(const Var& other) const;
            bool has(const Var& other) const;
            bool isZero() const;
            bool isNumericallyEvaluable() const;
            Number numericEval() const;
            /* Returns "Symbol", "Numeric", "Constant", "Undefined", "Function", "Sum", "Product" or
             * "Power": */
            std::string type() const;
            /* Returns names for Symbols, Functions and Constants, an empty string otherwise: */
            const std::string& name() const;
            /* Return the sub-/superscript of a Symbol or an empty string: */
            const std::string& subscript() const;
            const std::string& superscript() const;
            std::vector<Var> operands() const;
            std::vector<Var> collectSymbols() const;
            const BasePtr& getBasePtr() const;

        private:
            void collectSymbols(const BasePtr& ptr, std::vector<Var>& symbols) const;
            void insertSymbolIfNotPresent(const BasePtr& symbol, std::vector<Var>& symbols) const;

            BasePtr basePtr;
    };

    bool operator == (const Var& lhs, const Var& rhs);
    bool operator != (const Var& lhs, const Var& rhs);

    Var operator + (Var lhs, const Var& rhs);
    Var operator - (Var lhs, const Var& rhs);
    Var operator * (Var lhs, const Var& rhs);
    Var operator / (Var lhs, const Var& rhs);

    std::ostream& operator << (std::ostream& stream, const Var& rhs);
}

#endif
