#ifndef TSYM_NAME_H
#define TSYM_NAME_H

#include <string>

namespace tsym {
    class Name {
        public:
            Name();
            explicit Name(const std::string& name);
            Name(const std::string& name, const std::string& subscript);
            Name(const std::string& name, const std::string& subscript,
                    const std::string& superscript);

            const std::string& getName() const;
            const std::string& getSubscript() const;
            const std::string& getSuperscript() const;

            const std::string& plain() const;
            std::string tex() const;

            bool equal(const Name& rhs) const;
            bool lessThan(const Name& rhs) const;

        private:
            bool isGreekLetter() const;
            std::string getGreekTexLetter() const;
            std::string texAppendix(const std::string& term, const std::string& connection) const;

            std::string name;
            std::string subscript;
            std::string superscript;
            std::string plainText;
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
