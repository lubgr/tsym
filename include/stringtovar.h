#ifndef TSYM_STRINGTOVAR_H
#define TSYM_STRINGTOVAR_H

#include <vector>
#include <string>
#include "var.h"

namespace tsym {
    class StringToVar {
        /* Simple wrapper class around the parser to construct expressions from textual input. Error
         * processing isn't very sophisticated: in case of a syntax error, everything starting from
         * that error is rejected. Unrecognized characters are ignored wherever possible. Examples
         * of error recovery are:
         *
         * - "1a" = 1
         * - "a_subscript" = a_s
         * - "{{12*a]ö" = 12*a
         * - "sqrt(2)*sinn(0)" = sqrt(2)*sinn (where 'sinn' is a variable!)
         *
         * This might in many cases be not very accurate, but provides a simple procedure for wrong
         * input. */
        public:
            StringToVar(const std::string& source);

            bool success() const;
            const std::vector<std::string>& errorMessages() const;
            unsigned firstErrorIndex() const;
            const Var& get() const;

        private:
            StringToVar(const StringToVar& other);
            const StringToVar& operator = (const StringToVar& rhs);

            void parse();

            const std::string source;
            std::vector<std::string> errors;
            unsigned errorIndex;
            Var result;
    };
}

#endif
