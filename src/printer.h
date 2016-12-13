#ifndef TSYM_PRINTER_H
#define TSYM_PRINTER_H

#include <sstream>
#include <vector>
#include <typeinfo>
#include "baseptr.h"
#include "baseptrlist.h"
#include "number.h"
#include "var.h"
#include "vector.h"
#include "matrix.h"

namespace tsym {
    class Printer {
        /* Generates a simple text form of the given argument, that can be obtained via the getStr()
         * method.
         *
         * If desired, the use of fractions can be disabled (globally for alle Printer instances via
         * a static method/member), per default, it is enabled. Using fractions means converting a
         * product of powers into a fraction, if some power expressions have numeric, negative
         * exponents, e.g. a*b^(-1) can be printed as a/b. Collection of powers with the same
         * exponent isn't provided. Even if sqrt(a*b) may be more intuitive than sqrt(a)*sqrt(b), it
         * obfuscates the actual data representation too much and causes other internal problems
         * with the use of fractions.
         *
         * There is a UTF8-encoded subscript plus-sign for positive symbols. Enabling or disabling
         * this character can be achieved via the corresponding static method, and whether it's
         * enabled by default depends on the configuration. */
        public:
            Printer();
            explicit Printer(const Var& var);
            explicit Printer(const Number& number);
            explicit Printer(const BasePtr& ptr);
            explicit Printer(const Vector& vector);
            explicit Printer(const Matrix& matrix);

            void set(const Var& var);
            void set(const Number& number);
            void set(const BasePtr& ptr);
            void set(const Vector& vec);
            void set(const Matrix& matrix);

            static void enableFractions();
            static void disableFractions();
            static void enableUtf8();
            static void disableUtf8();

            std::string getStr() const;
            void print(std::ostream& stream);

        private:
            void setDefaults();
            void clearStream();
            void print(const BasePtr& ptr);
            void printSymbol(const BasePtr& ptr);
            void printNumeric(const BasePtr& ptr);
            void printNumber(const Number& n);
            void printPower(const BasePtr& ptr);
            void printPower(const BasePtr& base, const BasePtr& exp);
            bool isNegativeNumeric(const BasePtr& ptr) const;
            BasePtr toggleSign(const BasePtr& numeric) const;
            void printBase(const BasePtr& base);
            bool needsBaseParentheses(const BasePtr& base) const;
            bool isPositiveInt(const BasePtr& ptr) const;
            void printExponent(const BasePtr& exp);
            bool needsExpParentheses(const BasePtr& ptr) const;
            unsigned prec(const BasePtr& ptr) const;
            unsigned prec(const std::type_info& type) const;
            void printInParentheses(const BasePtr& ptr);
            void printSum(const BasePtr& ptr);
            bool isProductWithNegativeNumeric(const BasePtr& ptr);
            void printProduct(const BasePtr& ptr);
            void printProductFrac(const BasePtrList& factors);
            std::pair<BasePtrList, BasePtrList> getProductFrac(const BasePtrList& origFactors);
            void printProduct(const BasePtrList& factors);
            void printFunction(const BasePtr& ptr);
            void printConstant(const BasePtr& ptr);
            void print(const Vector& vector);
            int getMaxCharacters(const Vector& vector) const;
            void print(const Matrix& matrix);
            void defMaxCharsPerColumn(const Matrix& matrix, std::vector<int>& maxChars) const;

            std::stringstream stream;
            static bool convertToFrac;
            static bool withUtf8;
    };
}

#endif
