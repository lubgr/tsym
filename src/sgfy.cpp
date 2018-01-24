
#include <regex>
#include "sgfy.h"

namespace sgfy {
    class Match {
        /* Wrapper class to be forward declared and moved to a compilation unit to eliminate the
         * need for the inclusion of the regex header (see comment below). */
        public:
            explicit Match(const std::string& fmt)
            {
                static const std::regex fmtPattern(
                        "%"
                        "("
                        "("
                        "[-+ #0]*"
                        "("
                        "([0-9*]*(\\.[0-9*]*)?)"
                        "|"
                        "(\\*(\\.[0-9*]*)?)"
                        "|"
                        "([0-9*]*\\.\\*)"
                        ")"
                        "(h|hh|l|ll|L|z|j|t)?[csdioxXufFeEaAgGnp]"
                        ")"
                        "|"
                        "S"
                        ")"
                        );

                rep = std::sregex_iterator(fmt.begin(), fmt.end(), fmtPattern);
            }

            std::iterator_traits<std::string::const_iterator>::difference_type length() const
            {
                return rep->length();
            }

            std::iterator_traits<std::string::const_iterator>::difference_type position() const
            {
                return rep->position();
            }

            bool isAtEnd() const
            {
                return rep == std::sregex_iterator();
            }

            void increment()
            {
                ++rep;
            }

            std::string str() const
            {
                return rep->str();
            }

        private:
            std::sregex_iterator rep;
    };
}

bool sgfy::isAtEnd(Match *match)
{
    return match->isAtEnd();
}

std::string sgfy::getStr(Match *match)
{
    return match->str();
}

void sgfy::append(std::ostream& stream, const char *fmt, std::va_list args)
{
    std::va_list argsCopy;
    std::size_t bufferSize;
    int charsWritten;
    char *buffer;

    va_copy(argsCopy, args);

    charsWritten = std::vsnprintf(NULL, 0, fmt, args);

    if (charsWritten < 0) {
        std::cerr << "Couldn't build string via format specifier" << std::endl;
        bufferSize = 10000;
    } else
        bufferSize = static_cast<size_t>(charsWritten + 1);

    buffer = new char[bufferSize];

    std::vsnprintf(buffer, bufferSize, fmt, argsCopy);

    stream << buffer;

    delete[] buffer;
}

void sgfy::macroAppend(std::ostream& stream, const char *fmt, ...)
{
    std::va_list args;

    va_start(args, fmt);

    append(stream, fmt, args);

    va_end(args);
}

sgfy::ArgProcessor::ArgProcessor(std::ostream& stream, const std::string& fmt) :
    nStoredAsterisks(0),
    match(new Match(fmt)),
    stream(stream),
    fmt(fmt),
    partialFmtLength(0),
    nAsterisks(0),
    pos(0)
{}

sgfy::ArgProcessor::~ArgProcessor()
{
    delete match;
}

void sgfy::ArgProcessor::updateCurrentMatchVars()
{
    partialFmtLength = static_cast<size_t>(match->position() + match->length()) - pos;
    nAsterisks = numberOfAsterisksInMatch();
}

size_t sgfy::ArgProcessor::numberOfAsterisksInMatch() const
{
    const size_t first = getStr(match).find('*');

    if (first == std::string::npos)
        return 0;
    else if (getStr(match).rfind('*') == first)
        return 1;
    else
        return 2;
}

void sgfy::ArgProcessor::storeVariableFieldWidth(const int& width)
{
    assert(nStoredAsterisks <= 1);

    asterisksWidth[nStoredAsterisks++] = width;
}

void sgfy::ArgProcessor::nextMatchOrFinalize()
{
    pos += partialFmtLength;

    match->increment();

    if (isAtEnd(match) && pos < fmt.length())
        append(stream, fmt.substr(pos));
}

std::string sgfy::str(const std::string& termination)
{
    return termination;
}
