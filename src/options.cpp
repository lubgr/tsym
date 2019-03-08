
#include "options.h"

namespace tsym {
    namespace {
        Int& maxPrimeResolution()
        {
            static Int maxPrimeResolution(1000);

            return maxPrimeResolution;
        }
    }
}

const tsym::Int& tsym::options::getMaxPrimeResolution()
{
    return maxPrimeResolution();
}

void tsym::options::setMaxPrimeResolution(Int max)
{
    maxPrimeResolution() = std::move(max);
}
