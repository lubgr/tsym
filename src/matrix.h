#ifndef TSYM_MATRIX_H
#define TSYM_MATRIX_H

#include <initializer_list>
#include <set>
#include "var.h"
#include "vector.h"

namespace tsym {
    class Matrix {
        /* Simple wrapper class around plain pointers, with some common matrix operations. */
        public:
            Matrix();
            Matrix(size_t nRow, size_t nCol);
            explicit Matrix(std::initializer_list<std::initializer_list<Var>> data);
            Matrix(const Matrix& other);
            Matrix(Matrix&& other);
            Matrix& operator = (const Matrix& rhs);
            Matrix& operator = (Matrix&& rhs);
            ~Matrix();

            Var& operator() (size_t i, size_t j);
            const Var& operator() (size_t i, size_t j) const;

            Matrix& operator += (const Matrix& rhs);
            Matrix& operator -= (const Matrix& rhs);
            Matrix& operator *= (const Matrix& rhs);
            Matrix& operator *= (const Var& rhs);
            Vector operator * (const Vector& rhs) const;

            const Matrix& operator + () const;
            Matrix operator - () const;

            Matrix transpose() const;
            Vector solve(const Vector& rhs) const;
            Matrix inverse() const;
            Var det() const;

            size_t rowSize() const;
            size_t colSize() const;
            bool isSymmetric() const;
            bool isSquare() const;
            bool equal(const Matrix& other) const;

        private:
            void allocateMem();
            void copyValuesFromMatrix(const Matrix& other);
            void deleteMem();
            void multiplyChecked(const Matrix& other);
            Vector solveChecked(const Vector& rhs) const;
            unsigned compPartialPivots(Vector *b);
            void collectAndSort(std::vector<std::vector<size_t>>& indices) const;
            void collectAndSort(size_t i, std::vector<size_t>& lineIndices) const;
            void selectPivots(std::vector<std::vector<size_t>>& indices) const;
            bool isLineMissing(std::set<size_t>& pivots) const;
            unsigned swapCount(std::vector<std::vector<size_t>>& indices) const;
            void factorizeLU();
            void compXFromLU(Vector& x, Vector& b) const;
            Matrix checkedInverse() const;
            Var checkedDet() const;
            Var detFromLU(unsigned nPivotSwaps) const;

            Var **data;
            size_t nRow;
            size_t nCol;
    };

    bool operator == (const Matrix& lhs, const Matrix& rhs);
    bool operator != (const Matrix& lhs, const Matrix& rhs);

    Matrix operator + (Matrix lhs, const Matrix& rhs);
    Matrix operator - (Matrix lhs, const Matrix& rhs);
    Matrix operator * (Matrix lhs, const Matrix& rhs);
    Matrix operator * (Matrix lhs, const Var& rhs);
    Matrix operator * (const Var& lhs, Matrix rhs);

    std::ostream& operator << (std::ostream& stream, const Matrix& m);
}

#endif
