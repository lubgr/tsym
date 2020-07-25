
#include "namefct.h"
#include <boost/range/algorithm/find_if.hpp>
#include <cctype>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <vector>
#include "name.h"

static_assert(std::is_aggregate_v<tsym::Name>);
static_assert(std::is_aggregate_v<tsym::NameView>);

namespace tsym {
    namespace {
        bool isGreekLetter(std::string_view str);
        size_t greekAlphabetIndex(std::string_view str);
        std::string unicodeForGreekLetter(std::string_view str);
        bool startsWithCapitalLetter(std::string_view str);
        std::string getGreekTexLetter(std::string_view str);

        bool isGreekLetter(std::string_view str)
        {
            if (str.length() <= 1)
                return false;
            else
                return greekAlphabetIndex(str) != static_cast<size_t>(-1);
        }

        size_t greekAlphabetIndex(std::string_view str)
        {
            static const std::vector<std::string_view> alphabet{"alpha", "beta", "gamma", "delta", "epsilon", "zeta",
              "eta", "theta", "iota", "kappa", "lambda", "my", "ny", "xi", "omikron", "pi", "rho", "sigma", "tau",
              "ypsilon", "phi", "chi", "psi", "omega"};
            const auto lookup = boost::find_if(alphabet, [str = std::string_view(str)](const auto& letter) {
                return (letter[0] == str[0] || letter[0] == static_cast<char>(std::tolower(str[0])))
                  && str.substr(1) == letter.substr(1);
            });

            if (lookup != cend(alphabet))
                return static_cast<size_t>(std::distance(cbegin(alphabet), lookup));

            return static_cast<size_t>(-1);
        }

        std::string unicodeForGreekLetter(std::string_view str)
        {
#ifndef TSYM_ASCII_ONLY
            const size_t index = greekAlphabetIndex(str);
            const size_t shift = startsWithCapitalLetter(str) ? 0 : 24;
            static const std::vector<std::string_view> alphabet{"\u0391", "\u0392", "\u0393", "\u0394", "\u0395",
              "\u0396", "\u0397", "\u0398", "\u0399", "\u039a", "\u039b", "\u039c", "\u039d", "\u039e", "\u039f",
              "\u03a0", "\u03a1", "\u03a3", "\u03a4", "\u03a5", "\u03a6", "\u03a7", "\u03a8", "\u03a9", "\u03b1",
              "\u03b2", "\u03b3", "\u03b4", "\u03b5", "\u03b6", "\u03b7", "\u03b8", "\u03b9", "\u03ba", "\u03bb",
              "\u03bc", "\u03bd", "\u03be", "\u03bf", "\u03c0", "\u03c1", "\u03c3", "\u03c4", "\u03c5", "\u03c6",
              "\u03c7", "\u03c8", "\u03c9"};

            return std::string(alphabet[shift + index]);
#else
            return str;
#endif
        }

        bool startsWithCapitalLetter(std::string_view str)
        {
            return static_cast<char>(std::tolower(str[0])) != str[0];
        }

        std::string getGreekTexLetter(std::string_view str)
        {
            std::string result("\\");

            if (str == "phi" || str == "Phi")
                result.append("var");

            return result.append(str);
        }

        std::string delimCurlyBracesOrPlain(std::string_view term, char connection)
        {
            std::string appendix;

            if (!term.empty())
                appendix.push_back(connection);

            if (term.length() == 1)
                appendix.append(term);
            else if (term.length() > 1) {
                appendix.push_back('{');
                appendix.append(term);
                appendix.push_back('}');
            }

            return appendix;
        }
    }
}

std::string tsym::unicode(NameView name)
{
    if (!isGreekLetter(name.value))
        return concat(name);
    else
        return unicodeForGreekLetter(name.value);
}

std::string tsym::tex(NameView name)
{
    std::string result = isGreekLetter(name.value) ? getGreekTexLetter(name.value) : std::string{name.value};

    result.append(delimCurlyBracesOrPlain(name.subscript, '_'));
    result.append(delimCurlyBracesOrPlain(name.superscript, '^'));

    return result;
}

std::string tsym::concat(NameView name)
{
    std::string result{name.value};

    result.append(delimCurlyBracesOrPlain(name.subscript, '_'));
    result.append(delimCurlyBracesOrPlain(name.superscript, '^'));

    return result;
}
