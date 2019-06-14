#ifndef TSYM_FIRSTNONZEROPIVOT_H
#define TSYM_FIRSTNONZEROPIVOT_H

#include <cstddef>
#include <vector>

namespace tsym {
    template <class MatrixView> class FirstNonZeroPivot {
      public:
        FirstNonZeroPivot(const MatrixView& m, std::size_t dim)
            : newRowIndices(dim)
            , dim(dim)
        {
            for (std::size_t j = 0; j + 1 < dim; ++j) {
                if (m(j, j) != 0) {
                    newRowIndices[j] = j;
                    continue;
                }

                for (std::size_t i = j + 1; i < dim; ++i)
                    if (m(i, j) != 0) {
                        newRowIndices[j] = i;

                        ++swapCount;

                        break;
                    }
            }
        }
        FirstNonZeroPivot(const FirstNonZeroPivot&) = delete;
        FirstNonZeroPivot(FirstNonZeroPivot&&) = delete;
        FirstNonZeroPivot& operator=(const FirstNonZeroPivot&) = delete;
        FirstNonZeroPivot& operator=(FirstNonZeroPivot&&) = delete;
        ~FirstNonZeroPivot() = default;

        unsigned nRowSwaps() const
        {
            return swapCount;
        }

        void applyTo(MatrixView& m) const
        {
            auto newIndices(newRowIndices);

            for (std::size_t i = 0; i + 1 < dim; ++i) {
                auto targetRow = newRowIndices[i];

                if (i != targetRow) {
                    swapRows(m, i, targetRow);

                    /* Make sure the rows are swapped only once: */
                    newIndices[targetRow] = targetRow;
                }
            }
        }

        template <class RhsVectorProxy> void applyTo(RhsVectorProxy& rhs) const
        {
            std::vector<std::size_t> newIndices = newRowIndices;

            for (std::size_t i = 0; i + 1 < dim; ++i) {
                auto targetRow = newRowIndices[i];

                if (i != targetRow) {
                    std::swap(rhs(i), rhs(targetRow));

                    /* See above. */
                    newIndices[targetRow] = targetRow;
                }
            }
        }

      private:
        void swapRows(MatrixView& m, std::size_t index1, std::size_t index2) const
        {
            for (std::size_t j = 0; j < dim; ++j)
                std::swap(m(index1, j), m(index2, j));
        }

        std::vector<std::size_t> newRowIndices;
        unsigned swapCount = 0;
        const std::size_t dim;
    };
}

#endif
