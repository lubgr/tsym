#ifndef TSYM_STRINGTOVAR_H
#define TSYM_STRINGTOVAR_H

#include <vector>
#include <string>
#include "var.h"

namespace tsym {
    class StringToVar {
        /* Simple wrapper class around the parser to construct expressions from textual input. */
        public:
            StringToVar(const std::string& source);

            bool success() const;
            const std::vector<std::string>& errorMessages() const;
            const Var& get() const;

        private:
            StringToVar(const StringToVar& other);
            const StringToVar& operator = (const StringToVar& rhs);

            void parse();

            const std::string source;
            std::vector<std::string> errors;
            Var result;
    };
}

#endif
