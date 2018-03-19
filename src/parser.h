#ifndef TSYM_PARSER_H
#define TSYM_PARSER_H

#include "baseptr.h"

namespace tsym {
    namespace parser {
        struct Result {
            BasePtr value;
            bool success;
            bool matchedWholeString;
        };

        /* Parsing is case-insensitive and accepts operators /-*+^ as well as parentheses and
         * function calls to log, sqrt, sin/cos/tan, asin/acos/atan. Symbols are constructed by
         * strings beginning with a letter (numbers are allowed in the rest of the Symbol name) and
         * may have an alphanumeric subscript, separated with an underscore and enclosed by {} in
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
         * - "{{12*a]ö" = Undefined (no recovery possible)
         * - "sqrt(2)*sinn(0)" = sqrt(2)*sinn (where 'sinn' is a variable!)
         *
         * This might in many cases be not very accurate, but provides a simple procedure for wrong
         * input.  */
        Result parse(const std::string& inputStr);
    }
}

#endif
