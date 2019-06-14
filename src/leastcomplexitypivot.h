#ifndef TSYM_LEASTCOMPLEXITYPIVOT_H
#define TSYM_LEASTCOMPLEXITYPIVOT_H

#include <cstddef>
#include <unordered_set>
#include <vector>
#include "stdvecwrapper.h"

namespace tsym {
    class LeastComplexityPivot {
      public:
        LeastComplexityPivot(const MatrixAdaptor& m, std::size_t dim);

        unsigned nRowSwaps() const;
        void applyTo(MatrixAdaptor& m) const;
        void applyTo(VectorAdaptor& rhs) const;

      private:
        void collectAndSort(const MatrixAdaptor& matrix, std::vector<std::vector<std::size_t>>& indices) const;
        void collectAndSort(const MatrixAdaptor& matrix, std::size_t i, std::vector<std::size_t>& lineIndices) const;
        void selectPivots(std::vector<std::vector<std::size_t>>& indices) const;
        bool isLineMissing(const std::unordered_set<std::size_t>& pivots) const;
        unsigned swapCount(std::vector<std::vector<std::size_t>> indices) const;

      private:
        std::vector<std::vector<std::size_t>> pivotIndices;
        const std::size_t dim;
    };
}

#endif
