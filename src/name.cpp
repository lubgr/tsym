
#include "name.h"

tsym::Name::Name() {}

tsym::Name::Name(const std::string& name) :
    name(name),
    plainText(name)
{}

tsym::Name::Name(const std::string& name, const std::string& subscript) :
    name(name),
    subscript(subscript),
    plainText(name)
{
    if (!subscript.empty())
        plainText.append("_").append(subscript);
}

tsym::Name::Name(const std::string& name, const std::string& subscript,
        const std::string& superscript) :
    name(name),
    subscript(subscript),
    superscript(superscript),
    plainText(name)
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

std::string tsym::Name::tex() const
{
    std::string result = isGreekLetter() ? getGreekTexLetter() : name;

    result.append(texAppendix(subscript, "_"));
    result.append(texAppendix(superscript, "^"));

    return result;
}

bool tsym::Name::isGreekLetter() const
{
    static const std::string alphabet[] = { "alpha", "beta", "gamma", "delta", "epsilon", "zeta",
        "eta", "theta", "iota", "kappa", "lambda", "my", "ny", "xi", "omikron", "pi", "rho",
        "sigma", "tau", "ypsilon", "phi", "chi", "psi", "omega" };
    static const size_t nLetters = sizeof(alphabet)/sizeof(alphabet[0]);

    if (name.length() <= 1)
        return false;

    for (size_t i = 0; i < nLetters; ++i)
        if (name.substr(1).compare(alphabet[i].substr(1)) != 0)
            continue;
        else if (alphabet[i][0] == name[0] || alphabet[i][0] == (char)tolower(name[0]))
            return true;

    return false;
}

std::string tsym::Name::getGreekTexLetter() const
{
    std::string result("\\");

    if (name == "phi" || name == "Phi")
        result.append("var");

    return result.append(name);
}

std::string tsym::Name::texAppendix(const std::string& term, const std::string& connection) const
{
    std::string appendix;

    if (!term.empty())
        appendix.append(connection);

    if (term.length() == 1)
        appendix.append(term);
    else if (term.length() > 1)
        appendix.append("{").append(term).append("}");

    return appendix;
}

bool tsym::Name::equal(const Name& rhs) const
{
    return plainText == rhs.plainText;
}

bool tsym::Name::lessThan(const Name& rhs) const
{
    return plainText < rhs.plainText;
}

bool tsym::operator == (const Name& lhs, const Name& rhs)
{
    return lhs.equal(rhs);
}

bool tsym::operator != (const Name& lhs, const Name& rhs)
{
    return !lhs.equal(rhs);
}

bool tsym::operator < (const Name& lhs, const Name& rhs)
{
    return lhs.lessThan(rhs);
}

bool tsym::operator <= (const Name& lhs, const Name& rhs)
{
    return !(lhs > rhs);
}

bool tsym::operator > (const Name& lhs, const Name& rhs)
{
    return rhs < lhs;
}

bool tsym::operator >= (const Name& lhs, const Name& rhs)
{
    return !(lhs < rhs);
}

std::ostream& tsym::operator << (std::ostream& stream, const Name& name)
{
    return stream << name.plain();
}
