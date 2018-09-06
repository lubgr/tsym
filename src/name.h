#ifndef TSYM_NAME_H
#define TSYM_NAME_H

#include <boost/operators.hpp>
#include <limits>
#include <string>

namespace tsym {
    class Name : private boost::totally_ordered<Name> {
      public:
        /* A Name object with super- and subscript. The given string may be empty (this is what
         * the default constructor does), because it can be queried for
         * non-Symbol/-Function/-Constant types. */
        Name() = default;
        explicit Name(const std::string& name);
        Name(const std::string& name, const std::string& subscript);
        Name(const std::string& name, const std::string& subscript, const std::string& superscript);

        const std::string& getName() const;
        const std::string& getSubscript() const;
        const std::string& getSuperscript() const;

        const std::string& plain() const;

      private:
        std::string name;
        std::string subscript;
        std::string superscript;
        std::string plainText;
    };

    bool operator==(const Name& lhs, const Name& rhs);
    bool operator<(const Name& lhs, const Name& rhs);
    std::ostream& operator<<(std::ostream& stream, const Name& name);
    size_t hash_value(const Name& name);
}

namespace std {
    template <> struct hash<tsym::Name> {
        size_t operator()(const tsym::Name& name) const;
    };
}

#endif
