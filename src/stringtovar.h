#ifndef TSYM_STRINGTOVAR_H
#define TSYM_STRINGTOVAR_H

#include <vector>
#include <string>
#include "var.h"

namespace tsym {
    class StringToVar {
        /* Simple wrapper class around the parser to construct expressions from textual input.
         * Parsing is case-insensitive and accepts operators /-*+^ as well as parentheses and
         * function calls to log, sqrt, sin/cos/tan, asin/acos/atan. Symbols are constructed by
         * strings beginning with a letter (numbers are allowed in the rest of the Symbol name) and
         * may have a alphanumeric subscript, separated with an underscore and enclosed by {} in
         * case of subscripts longer than one character. The (case-insensitive) string 'Pi' is
         * reserved for the Constant Pi. Euler's constant can be constructed by the (again
         * case-insensitive) string 'Euler' to avoid confusion with symbols names 'e' or a floating
         * point number given in scientific notation.
         *
         * Error processing isn't very sophisticated, and unrecognized characters are ignored
         * wherever possible. Examples of error recovery are:
         *
         * - "1a" = 1
         * - "a_subscript" = a_s
         * - "{{12*a]ö" = 12*a
         * - "sqrt(2)*sinn(0)" = sqrt(2)*sinn (where 'sinn' is a variable!)
         *
         * This might in many cases be not very accurate, but provides a simple procedure for wrong
         * input.  */
        public:
            StringToVar(const std::string& source);
            StringToVar(const StringToVar& other) = delete;
            StringToVar& operator = (const StringToVar& rhs) = delete;

            bool success() const;
            const std::vector<std::string>& errorMessages() const;
            unsigned firstErrorIndex() const;
            const Var& get() const;

        private:
            void parse();

            const std::string source;
            std::vector<std::string> errors;
            unsigned errorIndex = 0;
            Var result;
    };
}

#endif
