
#include "name.h"

tsym::Name::Name() :
    numeric(0)
{}

tsym::Name::Name(const std::string& name) :
    name(name),
    plainText(name),
    numeric(0)
{}

tsym::Name::Name(const std::string& name, const std::string& subscript) :
    name(name),
    subscript(subscript),
    plainText(name),
    numeric(0)
{
    if (!subscript.empty())
        plainText.append("_").append(subscript);
}

tsym::Name::Name(const std::string& name, const std::string& subscript,
        const std::string& superscript) :
    name(name),
    subscript(subscript),
    superscript(superscript),
    plainText(name),
    numeric(0)
{
    if (!subscript.empty())
        plainText.append("_").append(subscript);

    if (!superscript.empty())
        plainText.append("_").append(superscript);
}

tsym::Name::Name(unsigned n) :
    plainText("[tmp]"),
    numeric(n)
{}

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

std::string tsym::Name::unicode() const
{
    if (!isGreekLetter())
        return plain();
    else
        return unicodeForGreekLetter();
}

bool tsym::Name::isGreekLetter() const
{
    if (name.length() <= 1)
        return false;
    else
        return greekAlphabetIndex() != (size_t) -1;
}

size_t tsym::Name::greekAlphabetIndex() const
{
    static const std::string alphabet[] = { "alpha", "beta", "gamma", "delta", "epsilon", "zeta",
        "eta", "theta", "iota", "kappa", "lambda", "my", "ny", "xi", "omikron", "pi", "rho",
        "sigma", "tau", "ypsilon", "phi", "chi", "psi", "omega" };
    static const size_t nLetters = sizeof(alphabet)/sizeof(alphabet[0]);

    for (size_t i = 0; i < nLetters; ++i)
        if (name.substr(1).compare(alphabet[i].substr(1)) != 0)
            continue;
        else if (alphabet[i][0] == name[0] || alphabet[i][0] == (char)tolower(name[0]))
            return i;

    return (size_t) -1;
}

std::string tsym::Name::unicodeForGreekLetter() const
{
    const size_t index = greekAlphabetIndex();
    const size_t shift = startsWithCapitalLetter() ? 0 : 24;
    static const std::string alphabet[] = { "\u0391", "\u0392", "\u0393", "\u0394", "\u0395",
        "\u0396", "\u0397", "\u0398", "\u0399", "\u039a", "\u039b", "\u039c", "\u039d", "\u039e",
        "\u039f", "\u03a0", "\u03a1", "\u03a3", "\u03a4", "\u03a5", "\u03a6", "\u03a7", "\u03a8",
        "\u03a9", "\u03b1", "\u03b2", "\u03b3", "\u03b4", "\u03b5", "\u03b6", "\u03b7", "\u03b8",
        "\u03b9", "\u03ba", "\u03bb", "\u03bc", "\u03bd", "\u03be", "\u03bf", "\u03c0", "\u03c1",
        "\u03c3", "\u03c4", "\u03c5", "\u03c6", "\u03c7", "\u03c8", "\u03c9" };

    return alphabet[shift + index];
}

bool tsym::Name::startsWithCapitalLetter() const
{
    return (char)tolower(name[0]) != name[0];
}

std::string tsym::Name::tex() const
{
    std::string result = isGreekLetter() ? getGreekTexLetter() : name;

    result.append(texAppendix(subscript, "_"));
    result.append(texAppendix(superscript, "^"));

    return result;
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
    if (numeric != 0 && rhs.numeric != 0)
        return numeric == rhs.numeric;
    else if (numeric != 0 || rhs.numeric != 0)
        return false;
    else
        return plainText == rhs.plainText;
}

bool tsym::Name::lessThan(const Name& rhs) const
{
    if (numeric != 0 && rhs.numeric != 0)
        return numeric < rhs.numeric;
    else if (numeric != 0)
        return true;
    else if (rhs.numeric != 0)
        return false;
    else
        return plainText < rhs.plainText;
}

bool tsym::Name::isNumericId() const
{
    return numeric != 0;
}

unsigned tsym::Name::getNumericId() const
{
    return numeric;
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

size_t std::hash<tsym::Name>::operator () (const tsym::Name& name) const
{
    return std::hash<std::string>{}(name.plain()) ^
        (std::hash<unsigned>{}(name.getNumericId()) << 1);
}
