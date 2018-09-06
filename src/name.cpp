
#include "name.h"

tsym::Name::Name(const std::string& name)
    : name(name)
    , plainText(name)
{}

tsym::Name::Name(const std::string& name, const std::string& subscript)
    : name(name)
    , subscript(subscript)
    , plainText(name)
{
    if (!subscript.empty())
        plainText.append("_").append(subscript);
}

tsym::Name::Name(const std::string& name, const std::string& subscript, const std::string& superscript)
    : name(name)
    , subscript(subscript)
    , superscript(superscript)
    , plainText(name)
{
    if (!subscript.empty())
        plainText.append("_").append(subscript);

    if (!superscript.empty())
        plainText.append("_").append(superscript);
}

const std::string& tsym::Name::getName() const
{
    return name;
}

const std::string& tsym::Name::getSubscript() const
{
    return subscript;
}

const std::string& tsym::Name::getSuperscript() const
{
    return superscript;
}

const std::string& tsym::Name::plain() const
{
    return plainText;
}

bool tsym::operator==(const Name& lhs, const Name& rhs)
{
    return lhs.plain() == rhs.plain();
}

bool tsym::operator<(const Name& lhs, const Name& rhs)
{
    return lhs.plain() < rhs.plain();
}

std::ostream& tsym::operator<<(std::ostream& stream, const Name& name)
{
    return stream << name.plain();
}

size_t tsym::hash_value(const Name& name)
{
    return std::hash<std::string>{}(name.plain());
}

size_t std::hash<tsym::Name>::operator()(const tsym::Name& name) const
{
    return hash_value(name);
}
