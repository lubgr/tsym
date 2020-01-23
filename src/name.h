#ifndef TSYM_NAME_H
#define TSYM_NAME_H

#include <string>
#include <string_view>

namespace tsym {
    struct NameView {
        std::string_view value;
        std::string_view subscript{};
        std::string_view superscript{};
    };

    struct Name {
        /* This struct shall be an aggregate, hence not boost operator CTRP base class. Until the
         * three-way comparison is available, all operators must be implemented manually. */
        std::string value;
        std::string subscript{};
        std::string superscript{};

        operator NameView() const;
    };

    bool operator==(const Name& lhs, const Name& rhs);
    bool operator!=(const Name& lhs, const Name& rhs);
    bool operator<(const Name& lhs, const Name& rhs);
    bool operator<=(const Name& lhs, const Name& rhs);
    bool operator>(const Name& lhs, const Name& rhs);
    bool operator>=(const Name& lhs, const Name& rhs);
    std::ostream& operator<<(std::ostream& stream, const Name& name);
    size_t hash_value(const Name& name);
}

namespace std {
    template <> struct hash<tsym::Name> {
        size_t operator()(const tsym::Name& name) const;
    };
}

#endif
