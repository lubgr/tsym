#ifndef SGFY_H
#define SGFY_H

#include <cassert>
#include <iostream>
#include <string>
#include <cstdarg>
#include <sstream>

namespace sgfy {
    /* Format utility included from https://github.com/lubgr/sgfy */
    class Match;
    bool isAtEnd(Match *match);
    std::string getStr(Match *match);
    void append(std::ostream& stream, const char *fmt, std::va_list args);
    void macroAppend(std::ostream& stream, const char *fmt, ...);

    template<class T> typename
    std::enable_if<std::is_trivial<T>::value, const T&>::type pass(const T& arg)
    {
        return arg;
    }

    template<class T> typename
    std::enable_if<!std::is_trivial<T>::value, const char *>::type pass(const T&)
    {
        return "";
    }

    template<class ...T> void append(std::ostream& stream, const std::string& fmt, const T&... args)
    {
        macroAppend(stream, fmt.c_str(), pass(args)...);
    }

    class ArgProcessor {
        public:
            ArgProcessor(std::ostream& stream, const std::string& fmt);
            ~ArgProcessor();

            template<class T> void nextArg(const T& arg)
            {
                if (isAtEnd(match))
                    std::cerr << "Argument without format specifier: " << arg << std::endl;
                else
                    nextArgWithFmt(arg);
            }

        private:

            template<class T> void nextArgWithFmt(const T& arg)
            {
                updateCurrentMatchVars();

                if (getStr(match) == "%S") {
                    append(stream, fmt.substr(pos, partialFmtLength - 2));
                    stream << arg;
                } else if (nAsterisks > 0 && nStoredAsterisks == nAsterisks)
                    appendVarWidth(arg);
                else if (nAsterisks > 0) {
                    storeVariableFieldWidth(arg);
                    return;
                } else {
                    assert(std::is_trivial<T>::value);
                    append(stream, fmt.substr(pos, partialFmtLength), arg);
                }

                nextMatchOrFinalize();
            }

            void updateCurrentMatchVars();
            size_t numberOfAsterisksInMatch() const;

            template<class T> void appendVarWidth(const T& arg)
            {
                if (nAsterisks == 1)
                    append(stream, fmt.substr(pos, partialFmtLength), asterisksWidth[0], arg);
                else
                    append(stream, fmt.substr(pos, partialFmtLength), asterisksWidth[0],
                            asterisksWidth[1], arg);

                nStoredAsterisks = 0;
            }

            template<class T> void storeVariableFieldWidth(const T&)
            {
                assert(false);
            }

            void storeVariableFieldWidth(const int& width);
            void nextMatchOrFinalize();

            int asterisksWidth[2];
            short unsigned nStoredAsterisks;
            Match *match;
            std::ostream& stream;
            const std::string& fmt;
            size_t partialFmtLength;
            size_t nAsterisks;
            size_t pos;
    };

    template<class ...T> std::string str(const std::string& fmt, const T&... args)
    {
        std::ostringstream stream;
        ArgProcessor ap(stream, fmt);
        using expander = int[];

        (void) expander{ 0, (ap.nextArg(args), 0)... };

        return stream.str();
    }

    std::string str(const std::string& termination);
}

#endif
