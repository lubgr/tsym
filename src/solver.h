#ifndef TSYM_SOLVER_H
#define TSYM_SOLVER_H

#include "var.h"
#include "vector.h"
#include "matrix.h"

namespace tsym {
    class Solver {
        /* Simple convenience interface for solving systems of linear equations. The real solution
         * part is implemente in the Matrix class - here, only references to right hand side and
         * coefficient matrix are stored. */
        public:
            Solver();
            Solver(const Matrix& A, const Vector& rhs);

            void set(const Matrix& A);
            void set(const Vector& rhs);

            bool hasSolution() const;
            const Vector& solve();

        private:
            bool hasRequirements() const;

            const Matrix *A;
            const Vector *b;
            Vector x;
    };
}

#endif
