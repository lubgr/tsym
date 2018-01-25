#ifndef TSYM_VECTOR_H
#define TSYM_VECTOR_H

#include <initializer_list>

namespace tsym { class Var; }

namespace tsym {
    class Vector {
        /* This simple vector class wraps around a primitive pointer to dynamically allocated
         * storage, while the memory managment is hidden. */
        public:
            Vector() = default;
            explicit Vector(size_t size);
            explicit Vector(std::initializer_list<Var> data);
            Vector(const Vector& other);
            Vector(Vector&& other);
            Vector& operator = (const Vector& rhs);
            Vector& operator = (Vector&& rhs);
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
            void deleteMem();
            void copyValuesFromVector(const Vector& other);

            Var *data = nullptr;
            size_t dim = 0;
    };

    bool operator == (const Vector& lhs, const Vector& rhs);
    bool operator != (const Vector& lhs, const Vector& rhs);

    Vector operator + (Vector lhs, const Vector& rhs);
    Vector operator - (Vector lhs, const Vector& rhs);

    Vector operator * (Vector lhs, const Var& rhs);
    Vector operator * (const Var& lhs, Vector rhs);
}

#endif
