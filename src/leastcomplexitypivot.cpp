
#include "leastcomplexitypivot.h"
#include <algorithm>
#include <cassert>
#include "functions.h"
#include "stdvecwrapper.h"

tsym::LeastComplexityPivot::LeastComplexityPivot(const MatrixAdaptor& m, std::size_t dim)
    : pivotIndices(dim)
    , dim(dim)
{
    collectAndSort(m, pivotIndices);
    selectPivots(pivotIndices);
}

unsigned tsym::LeastComplexityPivot::nRowSwaps() const
{
    return swapCount(pivotIndices);
}

void tsym::LeastComplexityPivot::applyTo(MatrixAdaptor& m) const
{
    const MatrixAdaptor origMatrix = m;

    for (std::size_t i = 0; i < dim; ++i)
        for (std::size_t j = 0; j < dim; ++j)
            m(pivotIndices[i].front(), j) = origMatrix(i, j);
}

void tsym::LeastComplexityPivot::applyTo(VectorAdaptor& rhs) const
{
    VectorAdaptor origRhs = rhs;

    for (std::size_t i = 0; i < dim; ++i)
        rhs(pivotIndices[i].front()) = origRhs(i);
}

void tsym::LeastComplexityPivot::collectAndSort(
  const MatrixAdaptor& matrix, std::vector<std::vector<std::size_t>>& indices) const
{
    for (std::size_t i = 0; i < dim; ++i)
        collectAndSort(matrix, i, indices[i]);
}

void tsym::LeastComplexityPivot::collectAndSort(
  const MatrixAdaptor& matrix, std::size_t i, std::vector<std::size_t>& lineIndices) const
{
    auto sortByComplexity = [i, &matrix](std::size_t pivotA, std::size_t pivotB) {
        return complexity(matrix(i, pivotA)) < complexity(matrix(i, pivotB));
    };

    for (std::size_t j = 0; j < dim; ++j)
        if (matrix(i, j) != 0)
            lineIndices.push_back(j);

    assert(!lineIndices.empty());

    std::sort(begin(lineIndices), end(lineIndices), sortByComplexity);
}

void tsym::LeastComplexityPivot::selectPivots(std::vector<std::vector<std::size_t>>& indices) const
/* Decides how lines are reordered based on pivot, i.e., divisor complexity. As conflicts might
 * appear, permutation and recursion continue until a pivot distribution is found. This is a
 * trial-and-error logic, and there is no guarantee that the selected reordering yields the lowest
 * pivot complexity. However, as long as the coefficient matrix is not singular, an order is
 * provided that ensures no division by zero will happen during LU decomposition. */
{
    std::vector<std::size_t> selectedPivots(dim);
    std::unordered_set<std::size_t> targetLineIndices;

    for (std::size_t i = 0; i < dim; ++i)
        for (const auto& pivotIndex : indices[i])
            if (targetLineIndices.count(pivotIndex) == 0) {
                targetLineIndices.insert(pivotIndex);
                selectedPivots[i] = pivotIndex;
                break;
            }

    if (isLineMissing(targetLineIndices)) {
        std::rotate(begin(indices), begin(indices) + 1, end(indices));
        selectPivots(indices);
        std::rotate(rbegin(indices), rbegin(indices) + 1, rend(indices));
    } else
        for (std::size_t i = 0; i < dim; ++i)
            indices[i] = {selectedPivots[i]};
}

bool tsym::LeastComplexityPivot::isLineMissing(const std::unordered_set<std::size_t>& pivots) const
{
    for (std::size_t i = 0; i < dim; ++i)
        if (pivots.find(i) == cend(pivots))
            return true;

    return false;
}

unsigned tsym::LeastComplexityPivot::swapCount(std::vector<std::vector<std::size_t>> indices) const
/* Uses the selected indices as a sparse representation of a perturbed unit matrix and swaps rows
 * until the original distribution of diagonal elements is retained. */
{
    unsigned count = 0;

    for (std::size_t i = 0; i < dim; ++i) {
        if (indices[i][0] == i)
            continue;

        for (std::size_t n = i + 1; n < dim; ++n)
            if (i == indices[n][0]) {
                std::swap(indices[n], indices[i]);
                ++count;
                break;
            }
    }

    return count;
}
