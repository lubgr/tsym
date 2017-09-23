
#include <map>
#include "options.h"

namespace tsym {
    namespace {
        bool isInitiated = false;

        template<class T> std::map<Option, T>& getMap()
        {
            static std::map<Option, T> map;

            return map;
        }

        const char *optionName(Option key)
        {
            static std::map<Option, std::string> names;

            if (names.empty()) {
#define STRINGIFY(option) names[Option::option] = #option
                STRINGIFY(USE_OPTIONAL_CACHING);
                STRINGIFY(MAX_PRIME_INTEGER);
                STRINGIFY(PRINT_FRACTIONS);
                STRINGIFY(PRINT_UTF8);
#undef STRINGIFY
            }

            return names[key].c_str();
        }
    }
}

bool tsym::options::areOptionsInitiated()
{
    return isInitiated;
}

void tsym::options::setDefaultOptions()
{
    isInitiated = true;

    setOption<bool>(Option::USE_OPTIONAL_CACHING, true);
    setOption<bool>(Option::PRINT_UTF8, false);
    setOption<bool>(Option::PRINT_FRACTIONS, true);
    setOption<int>(Option::MAX_PRIME_INTEGER, 1000);
}

template<class T> const T& tsym::options::get(Option key)
{
    if (!areOptionsInitiated())
        setDefaultOptions();

    const auto lookup = getMap<T>().find(key);

    if (lookup == getMap<T>().end())
        TSYM_ERROR("Option %s not found!", optionName(key));

    return lookup->second;
}

/* This function is declared directly inside of the tsym namespace to make it available in the
 * public API, but must be implemented here such that the correct functions can be deduced from the
 * implementation of the functions in the options namespace. */
template<class T> void tsym::setOption(tsym::Option key, const T& value)
{
    if (!options::areOptionsInitiated())
        options::setDefaultOptions();

    getMap<T>()[key] = value;

    /* Make sure the function with this signature exists: */
    (void) options::get<T>(key);

    TSYM_INFO("Set option %s to %S", optionName(key), value);
}
