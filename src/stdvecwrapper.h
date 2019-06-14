#ifndef TSYM_STDVEC_WRAPPER_H
#define TSYM_STDVEC_WRAPPER_H

#include <vector>
#include "var.h"

namespace tsym {
    struct VectorAdaptor {
        std::vector<Var> data;

        Var& operator()(std::size_t i)
        {
            return data[i];
        }
        const Var& operator()(std::size_t i) const
        {
            return data[i];
        }
    };

    struct MatrixAdaptor {
        std::vector<Var> data;
        std::size_t nRows;

        Var& operator()(std::size_t i, std::size_t j)
        {
            return data[i * nRows + j];
        }
        const Var& operator()(std::size_t i, std::size_t j) const
        {
            return data[i * nRows + j];
        }
    };
}

#endif
