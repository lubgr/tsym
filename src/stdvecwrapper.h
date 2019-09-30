#ifndef TSYM_STDVEC_WRAPPER_H
#define TSYM_STDVEC_WRAPPER_H

#include <vector>
#include "var.h"

namespace tsym {
    template <class T = Var> struct VectorAdaptor {
        std::vector<T> data;

        T& operator()(std::size_t i)
        {
            return data[i];
        }
        const T& operator()(std::size_t i) const
        {
            return data[i];
        }
    };

    template <class T = Var> struct SquareMatrixAdaptor {
        std::vector<T> data;
        std::size_t dim;

        T& operator()(std::size_t i, std::size_t j)
        {
            return data[i * dim + j];
        }
        const T& operator()(std::size_t i, std::size_t j) const
        {
            return data[i * dim + j];
        }
    };
}

#endif
