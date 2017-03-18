#ifndef TSYM_NAME_H
#define TSYM_NAME_H

#include <string>

namespace tsym {
    class Name {
        public:
            /* A Name object with super- and subscript. The given string may be empty (this is what
             * the default constructor does), because it can be queried for
             * non-Symbol/-Function/-Constant types. It may be an unsigned integer for the
             * construction of temporary symbols. */
            Name();
            explicit Name(const std::string& name);
            Name(const std::string& name, const std::string& subscript);
            Name(const std::string& name, const std::string& subscript,
                    const std::string& superscript);
            explicit Name(unsigned n);

            const std::string& getName() const;
            const std::string& getSubscript() const;
            const std::string& getSuperscript() const;

            const std::string& plain() const;
            std::string unicode() const;
            std::string tex() const;

            bool equal(const Name& rhs) const;
            bool lessThan(const Name& rhs) const;

            bool isNumericId() const;

        private:
            bool isGreekLetter() const;
            size_t greekAlphabetIndex() const;
            std::string unicodeForGreekLetter() const;
            bool startsWithCapitalLetter() const;
            std::string getGreekTexLetter() const;
            std::string texAppendix(const std::string& term, const std::string& connection) const;

            std::string name;
            std::string subscript;
            std::string superscript;
            std::string plainText;
            unsigned numeric;
    };

    bool operator == (const Name& lhs, const Name& rhs);
    bool operator != (const Name& lhs, const Name& rhs);
    bool operator < (const Name& lhs, const Name& rhs);
    bool operator <= (const Name& lhs, const Name& rhs);
    bool operator > (const Name& lhs, const Name& rhs);
    bool operator >= (const Name& lhs, const Name& rhs);
    std::ostream& operator << (std::ostream& stream, const Name& name);
}

#endif
