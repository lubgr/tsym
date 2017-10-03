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

            /* Solution of linear systems of equations and the computation of determinants are
             * implemented by LU factorization with partial pivoting. There are two options for
             * choosing pivots: either every row with a zero diagonal element is exchanged with the
             * first row that has a non-zero diagonal element, or the complexity of every non-zero
             * entry of every row is used to find pivots that have the least complexity. Which
             * method is better suited seems to depend on the coefficients and the right hand side,
             * there is no rule of thumb for a choice. */
            enum class Pivoting { LEAST_COMPLEXITY, FIRST_NON_ZERO };

            Matrix transpose() const;
            Vector solve(const Vector& rhs, Pivoting option = Pivoting::LEAST_COMPLEXITY) const;
            Matrix inverse() const;
            Var det(Pivoting option = Pivoting::LEAST_COMPLEXITY) const;

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
            Vector solveChecked(const Vector& rhs, Pivoting option) const;
            unsigned compPartialPivots(Pivoting option, Vector *b);
            unsigned partialPivotByLeastComplexity(Vector *b);
            void collectAndSort(std::vector<std::vector<size_t>>& indices) const;
            void collectAndSort(size_t i, std::vector<size_t>& lineIndices) const;
            void selectPivots(std::vector<std::vector<size_t>>& indices) const;
            bool isLineMissing(std::set<size_t>& pivots) const;
            unsigned swapCount(std::vector<std::vector<size_t>>& indices) const;
            unsigned partialPivotFirstNonZero(Vector *b);
            void swapRows(size_t index1, size_t index2);
            void factorizeLU();
            void compXFromLU(Vector& x, Vector& b) const;
            Matrix checkedInverse() const;
            Var checkedDet(Pivoting option) const;
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
