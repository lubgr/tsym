
#include <algorithm>
#include <cassert>
#include <vector>
#include <chrono>
#include "matrix.h"
#include "var.h"
#include "vector.h"
#include "numeric.h"
#include "logging.h"
#include "printer.h"

namespace tsym {
    namespace {
        const Var& constZero()
        {
            static const Var zero(Numeric::zero());

            return zero;
        }

        Var& zero()
        {
            static Var nonConstZero(Numeric::zero());

            return nonConstZero;
        }

#ifndef NDEBUG
        bool areAllItemsZero(const Vector& vec)
        {
            for (size_t i = 0; i < vec.size(); ++i)
                if (!vec(i).isZero())
                    return false;

            return true;
        }
#endif
    }
}

tsym::Matrix::Matrix(size_t nRow, size_t nCol) :
    nRow(nRow),
    nCol(nCol)
{
    allocateMem();
}

tsym::Matrix::Matrix(std::initializer_list<std::initializer_list<Var>> data) :
    nRow(data.size())
{
    size_t columnCount = data.begin()->size();
    size_t i = 0;

    for (const auto& row : data) {
        if (columnCount != row.size()) {
            TSYM_ERROR("Matrix initialization with varying column number");
            columnCount = row.size() >= columnCount ? row.size() : columnCount;
        }
    }

    nCol = columnCount;

    allocateMem();

    for (const auto& row : data) {
        size_t j = 0;
        for (const auto& item : row)
            (this->data)[i][j++] = item;
        ++i;
    }
}

tsym::Matrix::Matrix(const Matrix& other) :
    nRow(other.nRow),
    nCol(other.nCol)
{
    allocateMem();
    copyValuesFromMatrix(other);
}

tsym::Matrix::Matrix(Matrix&& other) :
    data(other.data),
    nRow(other.nRow),
    nCol(other.nCol)
{
    other.nRow = 0;
    other.nCol = 0;
    other.data = nullptr;
}

tsym::Matrix& tsym::Matrix::operator = (const Matrix& rhs)
{
    if (this == &rhs)
        return *this;

    if (nRow != rhs.nRow || nCol != rhs.nCol) {
        deleteMem();
        nRow = rhs.nRow;
        nCol = rhs.nCol;
        allocateMem();
    }

    copyValuesFromMatrix(rhs);

    return *this;
}

tsym::Matrix& tsym::Matrix::operator = (Matrix&& rhs)
{
    deleteMem();

    nRow = rhs.nRow;
    nCol = rhs.nCol;
    data = rhs.data;

    rhs.nRow = 0;
    rhs.nCol = 0;
    rhs.data = nullptr;

    return *this;
}

tsym::Matrix::~Matrix()
{
    deleteMem();
}

void tsym::Matrix::allocateMem()
{
    data = new Var*[nRow];

    for (size_t i = 0; i < nRow; ++i)
        data[i] = new Var[nCol];
}

void tsym::Matrix::copyValuesFromMatrix(const Matrix& other)
{
    /* nCol and nRow of both matrices have been checked, they are equal. */
    for (size_t i = 0; i < nRow; ++i)
        for (size_t j = 0; j < nCol; ++j)
            data[i][j] = other.data[i][j];
}

void tsym::Matrix::deleteMem()
{
    if (nRow == 0 && nCol == 0)
        return;

    for (size_t i = 0; i < nRow; i++)
        delete[] data[i];

    delete[] data;
}

tsym::Var& tsym::Matrix::operator () (size_t i, size_t j)
{
    if (nRow == 0 && nCol == 0)
        TSYM_ERROR("Matrix has zero column/row size! Return zero.");
    else if (i > nRow || j > nCol)
        TSYM_ERROR("Matric indices (%zu, %zu) out of bounds! Return zero.", i, j);
    else
        return data[i][j];

    /* A non-const reference to a static object is returned, it could possibly be modified from
     * the outside. So initialize it to zero before returning is necessary. */
    zero() = constZero();

    return zero();
}

const tsym::Var& tsym::Matrix::operator () (size_t i, size_t j) const
{
    if (nRow == 0 && nCol == 0)
        TSYM_ERROR("Matrix has zero column/row size! Return zero.");
    else if (i > nRow || j > nCol)
        TSYM_ERROR("Matric indices (%zu, %zu) out of bounds! Return zero.", i, j);
    else
        return data[i][j];

    return constZero();
}

tsym::Matrix& tsym::Matrix::operator += (const Matrix& rhs)
{
    if (nRow == rhs.nRow && nCol == rhs.nCol) {
        for (size_t i = 0; i < nRow; ++i)
            for (size_t j = 0; j < nCol; ++j)
                data[i][j] += rhs.data[i][j];
    } else
        TSYM_ERROR("Matrix dimensions %zu and %zu don't match! Return unmodified left hand side.",
                nRow, rhs.nRow);

    return *this;
}

tsym::Matrix& tsym::Matrix::operator -= (const Matrix& rhs)
{
    if (nRow == rhs.nRow && nCol == rhs.nCol) {
        for (size_t i = 0; i < nRow; ++i)
            for (size_t j = 0; j < nCol; ++j)
                data[i][j] -= rhs.data[i][j];
    } else
        TSYM_ERROR("Matrix dimensions %zu and %zu don't match! Return unmodified left hand side.",
                nRow, rhs.nRow);

    return *this;
}

tsym::Matrix& tsym::Matrix::operator *= (const Matrix& rhs)
{
    if (nCol == rhs.nRow)
        multiplyChecked(rhs);
    else
        TSYM_ERROR("Matrix dimensions %zu and %zu don't match! Return matrix with zero entries.",
                nRow, rhs.nRow);

    return *this;
}

void tsym::Matrix::multiplyChecked(const Matrix& rhs)
{
    const Matrix copy(*this);

    deleteMem();
    nCol = rhs.nCol;
    allocateMem();

    for (size_t i = 0; i < copy.nRow; ++i)
        for (size_t j = 0; j < rhs.nCol; ++j)
            for (size_t k = 0; k < copy.nCol; ++k)
                data[i][j] += copy.data[i][k]*rhs.data[k][j];
}

tsym::Matrix& tsym::Matrix::operator *= (const Var& rhs)
{
    for (size_t i = 0; i < nRow; ++i)
        for (size_t j = 0; j < nCol; ++j)
            data[i][j] *= rhs;

    return *this;
}

tsym::Vector tsym::Matrix::operator * (const Vector& rhs) const
{
    Vector result(nRow);

    if (nCol == rhs.dim)
        for (size_t i = 0; i < nRow; ++i)
            for (size_t j = 0; j < nCol; ++j)
                result.data[i] += data[i][j]*rhs.data[j];
    else
        TSYM_ERROR("%zu matrix columns don't match vector size (%zu)! Return empty vector.", nCol,
                rhs.dim);

    return result;
}

const tsym::Matrix& tsym::Matrix::operator + () const
{
    return *this;
}

tsym::Matrix tsym::Matrix::operator - () const
{
    Matrix res(nRow, nCol);

    for (size_t i = 0; i < nRow; ++i)
        for (size_t j = 0; j < nCol; ++j)
            res.data[i][j] = -data[i][j];

    return res;
}

tsym::Matrix tsym::Matrix::transpose() const
{
    Matrix res(nCol, nRow);

    for (size_t i = 0; i < nCol; ++i)
        for (size_t j = 0; j < nRow; ++j)
            res.data[i][j] = data[j][i];

    return res;
}

tsym::Vector tsym::Matrix::solve(const Vector& rhs, Pivoting option) const
{
    if (!isSquare())
        TSYM_ERROR("Matrix (%zu, %zu) isn't square!", nRow , nCol);
    else if (rhs.size() != nRow)
        TSYM_ERROR("Matrix dimension %zu doesn't match vector size %zu", nRow, rhs.dim);
    else if (nRow == 0)
        TSYM_ERROR("Matrix and vector with zero dimension can't be solved!");
    else
        return solveChecked(rhs, option);

    TSYM_ERROR("Return vector with zero dimension.");

    return Vector();
}

tsym::Vector tsym::Matrix::solveChecked(const Vector& rhs, Pivoting option) const
{
    auto ts = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds ms;
    unsigned nPivotSwaps;
    decltype(ts) te;
    Matrix PLU(*this);
    Vector b(rhs);
    Vector x(nRow);

    nPivotSwaps = PLU.compPartialPivots(option, &b);
    PLU.factorizeLU();

    if (PLU.detFromLU(nPivotSwaps).isZero()) {
        TSYM_WARNING("Can't solve system of equations with singular coefficient matrix!");
        x = Vector();
    } else {
        PLU.compXFromLU(x, b);

        te = std::chrono::high_resolution_clock::now();
        ms = std::chrono::duration_cast<std::chrono::microseconds>(te - ts);

        TSYM_INFO("Solved %zu-dim. system of equations in %.2f ms.", nRow,
                static_cast<float>(ms.count())/1000.0);
    }

    return x;
}

unsigned tsym::Matrix::compPartialPivots(Pivoting option, Vector *b)
    /* Returns the number of row swaps. */
{
    switch (option) {
        case Pivoting::LEAST_COMPLEXITY:
            return partialPivotByLeastComplexity(b);
        case Pivoting::FIRST_NON_ZERO:
        default:
            return partialPivotFirstNonZero(b);
    }
}

unsigned tsym::Matrix::partialPivotByLeastComplexity(Vector *b)
{
    std::vector<std::vector<size_t>> pivotIndices(nRow);
    Vector *rhsCopy = b == nullptr ? nullptr : new Vector(*b);
    Matrix coefficientCopy(*this);

    collectAndSort(pivotIndices);
    selectPivots(pivotIndices);

    for (size_t i = 0; i < nRow; ++i) {
        assert(pivotIndices[i].size() == 1);

        size_t targetLine = pivotIndices[i][0];

        for (size_t j = 0; j < nCol; ++j)
            data[targetLine][j] = coefficientCopy.data[i][j];

        if (b != nullptr)
            b->data[targetLine] = rhsCopy->data[i];
    }

    delete rhsCopy;

    return swapCount(pivotIndices);
}

void tsym::Matrix::collectAndSort(std::vector<std::vector<size_t>>& indices) const
{
    for (size_t i = 0; i < nRow; ++i)
        collectAndSort(i, indices[i]);
}

void tsym::Matrix::collectAndSort(size_t i, std::vector<size_t>& lineIndices) const
{
    const auto localData = data;
    auto sortByComplexity = [i, localData] (size_t pivotA, size_t pivotB) {
        return localData[i][pivotA].complexity() < localData[i][pivotB].complexity();
    };

    for (size_t j = 0; j < nCol; ++j)
        if (!data[i][j].isZero())
            lineIndices.push_back(j);

    assert(lineIndices.size() > 0);

    std::sort(lineIndices.begin(), lineIndices.end(), sortByComplexity);
}

void tsym::Matrix::selectPivots(std::vector<std::vector<size_t>>& indices) const
/* Decides how lines are reordered based on pivot, i.e., divisor complexity. As conflicts might
 * appear, permutation and recursion continue until a pivot distribution is found. This is a
 * trial-and-error logic, and there is no guarantee that the selected reordering yields the lowest
 * pivot complexity. However, as long as the coefficient matrix is not singular, an order is
 * provided that ensures no division by zero will happen during LU decomposition. */
{
    std::vector<size_t> selectedPivots(nRow);
    std::set<size_t> targetLineIndices;

    for (size_t i = 0; i < nRow; ++i)
        for (const auto& pivotIndex : indices[i])
            if (targetLineIndices.count(pivotIndex) == 0) {
                targetLineIndices.insert(pivotIndex);
                selectedPivots[i] = pivotIndex;
                break;
            }

    if (isLineMissing(targetLineIndices)) {
        std::rotate(indices.begin(), indices.begin() + 1, indices.end());
        selectPivots(indices);
        std::rotate(indices.rbegin(), indices.rbegin() + 1, indices.rend());
    } else
        for (size_t i = 0; i < nRow; ++i)
            indices[i] = { selectedPivots[i] };
}

bool tsym::Matrix::isLineMissing(std::set<size_t>& pivots) const
{
    const auto end = pivots.end();

    for (size_t i = 0; i < nRow; ++i)
        if (pivots.find(i) == end)
            return true;

    return false;
}

unsigned tsym::Matrix::swapCount(std::vector<std::vector<size_t>>& indices) const
/* Uses the selected indices as a sparse representation of a perturbed unit matrix and swaps rows
 * until the original distribution of diagonal elements is retained. */
{
    unsigned count = 0;

    for (size_t i = 0; i < nRow; ++i) {
        if (indices[i][0] == i)
            continue;

        for (size_t n = i + 1; n < nRow; ++n)
            if (i == indices[n][0]) {
                std::swap(indices[n], indices[i]);
                ++count;
                break;
            }
    }

    return count;
}

unsigned tsym::Matrix::partialPivotFirstNonZero(Vector *b)
{
    unsigned swapCount = 0;

    for (size_t j = 0; j + 1 < nCol; ++j) {
        if (!data[j][j].isZero())
            continue;

        for (size_t i = j + 1; i < nRow; ++i)
            if (!data[i][j].isZero()) {
                swapRows(i, j);

                if (b != nullptr)
                    std::swap(b->data[j], b->data[i]);

                ++swapCount;

                break;
            }
    }

    return swapCount;
}

void tsym::Matrix::swapRows(size_t index1, size_t index2)
{
    for (size_t j = 0; j < nCol; ++j)
        std::swap(data[index1][j], data[index2][j]);
}

void tsym::Matrix::factorizeLU()
{
    for (size_t j = 0; j + 1 < nCol; ++j)
        for (size_t i = j + 1; i < nRow; ++i) {
            data[i][j] /= data[j][j];
            for (size_t k = j + 1; k < nCol; ++k)
                data[i][k] -= data[i][j]*data[j][k];
        }
}

void tsym::Matrix::compXFromLU(Vector& x, Vector& b) const
{
    for (size_t i = 0; i < nRow; ++i)
        for (size_t j = 0; j < i; ++j)
            b.data[i] -= data[i][j]*b.data[j];

    assert(areAllItemsZero(x));

    for (size_t i = nRow - 1; i + 1 > 0; --i) {
        for (size_t j = i + 1; j < nCol; ++j) {
            x.data[i] -= data[i][j]*x.data[j];
        }

        x.data[i] = ((b.data[i] + x.data[i])/data[i][i]).normal();
    }
}

tsym::Matrix tsym::Matrix::inverse() const
{
    if (!isSquare())
        TSYM_ERROR("Inversion for %zux%zu maxtrix impossible!", nRow, nCol);
    else if (det() == 0)
        TSYM_ERROR("Matrix is singular, no inversion possible!");
    else
        return checkedInverse();

    TSYM_ERROR("Return zero dimension matrix.");

    return Matrix();
}

tsym::Matrix tsym::Matrix::checkedInverse() const
{
    Matrix inverse(nRow, nRow);
    Vector inverseCol;
    Vector b(nRow);

    for (size_t i = 0; i < nRow; ++i, b.data[i - 1] = 0) {
        b.data[i] = 1;

        inverseCol = solve(b);

        for (size_t j = 0; j < nRow; ++j)
            inverse.data[j][i] = inverseCol.data[j];
    }

    return inverse;
}

tsym::Var tsym::Matrix::det(Pivoting option) const
{
    if (nRow == nCol && nRow != 0)
        return checkedDet(option);

    TSYM_ERROR("Determinant request for %zux%zu matrix! Return zero determinant.", nRow, nCol);

    return constZero();
}

tsym::Var tsym::Matrix::checkedDet(Pivoting option) const
{
    unsigned nPivotSwaps;
    Matrix PLU(*this);

    nPivotSwaps = PLU.compPartialPivots(option, nullptr);

    PLU.factorizeLU();

    return PLU.detFromLU(nPivotSwaps);
}

tsym::Var tsym::Matrix::detFromLU(unsigned nPivotSwaps) const
{
    Var det(nPivotSwaps % 2 == 0 ? 1 : -1);

    for (size_t i = 0; i < nRow; ++i)
        det *= data[i][i];

    det = det.normal();

    return det;
}

size_t tsym::Matrix::rowSize() const
{
    return nRow;
}

size_t tsym::Matrix::colSize() const
{
    return nCol;
}

bool tsym::Matrix::isSymmetric() const
{
    if (nRow != nCol)
        return false;

    for (size_t i = 1; i < nRow; ++i)
        for (size_t j = 0; j < i; ++j)
            if (data[i][j] != data[j][i])
                return false;

    return true;
}

bool tsym::Matrix::isSquare() const
{
    return nRow == nCol;
}

bool tsym::Matrix::equal(const Matrix& other) const
{
    if (nRow != other.nRow || nCol != other.nCol)
        return false;

    for (size_t i = 0; i < nRow; ++i)
        for (size_t j = 0; j < nCol; ++j)
            if (data[i][j] != other.data[i][j])
                return false;

    return true;
}

bool tsym::operator == (const Matrix& lhs, const Matrix& rhs)
{
    return lhs.equal(rhs);
}

bool tsym::operator != (const Matrix& lhs, const Matrix& rhs)
{
    return !lhs.equal(rhs);
}

tsym::Matrix tsym::operator + (Matrix lhs, const Matrix& rhs)
{
    lhs += rhs;

    return lhs;
}

tsym::Matrix tsym::operator - (Matrix lhs, const Matrix& rhs)
{
    lhs -= rhs;

    return lhs;
}

tsym::Matrix tsym::operator * (Matrix lhs, const Matrix& rhs)
{
    lhs *= rhs;

    return lhs;
}

tsym::Matrix tsym::operator * (Matrix lhs, const Var& rhs)
{
    lhs *= rhs;

    return lhs;
}

tsym::Matrix tsym::operator * (const Var& lhs, Matrix rhs)
{
    rhs *= lhs;

    return rhs;
}
