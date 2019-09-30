#ifndef TSYM_DIRECTSOLVE_H
#define TSYM_DIRECTSOLVE_H

#include <optional>
#include "stdvecwrapper.h"

namespace tsym {
    std::size_t firstNonZeroPivot(const SquareMatrixAdaptor<>& coeff, std::size_t row);
    std::size_t leastComplexityPivot(const SquareMatrixAdaptor<>& coeff, std::size_t row);

    using PivotStrategy = std::size_t (*)(const SquareMatrixAdaptor<>&, std::size_t);

    /* Performs partial pivoting and returns the number of row swaps: */
    unsigned eliminateGauss(SquareMatrixAdaptor<>& coeff, std::optional<VectorAdaptor<>>& rhs, PivotStrategy piv);
    void computeSolution(SquareMatrixAdaptor<>& coeff, VectorAdaptor<>& rhs, VectorAdaptor<>& x);
}

#endif
