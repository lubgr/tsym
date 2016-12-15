#ifndef TSYM_VECTOR_H
#define TSYM_VECTOR_H

#include "var.h"

namespace tsym {
    class Vector {
        /* This simple vector class wraps around a primitive pointer to dynamically allocated
         * storage, while the memory managment is hidden. This class has to be understood as a
         * column vector, there is no such thing as a row vector. */
        public:
            Vector();
            explicit Vector(size_t size);
            Vector(const Vector& other);
            Vector& operator = (const Vector& rhs);
            ~Vector();

            Var& operator() (size_t i);
            const Var& operator() (size_t i) const;

            Vector& operator += (const Vector& rhs);
            Vector& operator -= (const Vector& rhs);
            Vector& operator *= (const Var& rhs);

            const Vector& operator + () const;
            Vector operator - () const;

            Var dotProduct(const Vector& other) const;

            size_t size() const;
            bool equal(const Vector& other) const;

            friend class Matrix;

        private:
            void allocateMem();
            void copyValuesFromVector(const Vector& other);

            Var *data;
            size_t dim;
    };

    bool operator == (const Vector& lhs, const Vector& rhs);
    bool operator != (const Vector& lhs, const Vector& rhs);

    Vector operator + (Vector lhs, const Vector& rhs);
    Vector operator - (Vector lhs, const Vector& rhs);

    Vector operator * (Vector lhs, const Var& rhs);
    Vector operator * (const Var& lhs, Vector rhs);

    std::ostream& operator << (std::ostream& stream, const Vector& v);
}

#endif
