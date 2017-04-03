
#include "vector.h"
#include "logging.h"
#include "numeric.h"
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
    }
}

tsym::Vector::Vector() :
    data(nullptr),
    dim(0)
{}

tsym::Vector::Vector(size_t size) :
    data(nullptr),
    dim(size)
{
    allocateMem();
}

tsym::Vector::Vector(std::initializer_list<Var> data) :
    dim(data.size())
{
    size_t i = 0;

    allocateMem();

    for (const auto& item : data)
        this->data[i++] = item;
}

tsym::Vector::Vector(const Vector& other) :
    dim(other.dim)
{
    allocateMem();
    copyValuesFromVector(other);
}

tsym::Vector::Vector(Vector&& other) :
    data(other.data),
    dim(other.dim)
{
    other.data = nullptr;
    other.dim = 0;
}

tsym::Vector& tsym::Vector::operator = (const Vector& rhs)
{
    if (this == &rhs)
        return *this;

    if (dim != rhs.dim) {
        this->~Vector();
        dim = rhs.dim;
        allocateMem();
    }

    copyValuesFromVector(rhs);

    return *this;
}

tsym::Vector& tsym::Vector::operator = (Vector&& rhs)
{
    deleteMem();

    dim = rhs.dim;
    data = rhs.data;

    rhs.data = nullptr;
    rhs.dim = 0;

    return *this;
}

tsym::Vector::~Vector()
{
    deleteMem();
}

void tsym::Vector::allocateMem()
{
    if (dim != 0)
        data = new Var[dim];
}

void tsym::Vector::deleteMem()
{
    if (dim != 0)
        delete[] data;
}

void tsym::Vector::copyValuesFromVector(const Vector& other)
{
    /* At this point, the dimensions of both vectors have been checked, they are equal. */
    for (size_t i = 0; i < dim; ++i)
        data[i] = other.data[i];
}

tsym::Var& tsym::Vector::operator () (size_t i)
{
    if (dim == 0)
        TSYM_ERROR("Vector has zero size! Return zero.");
    else if (i > dim)
        TSYM_ERROR("Vector index %zu out of bounds! Return zero.", i);
    else
        return data[i];

    /* A non-const reference to a static object is returned, it could possibly be modified from
     * the outside. So initialize it to zero before returning is necessary. */
    zero() = constZero();

    return zero();
}

const tsym::Var& tsym::Vector::operator () (size_t i) const
{
    if (dim == 0)
        TSYM_ERROR("Vector has zero size! Return zero.");
    else if (i > dim)
        TSYM_ERROR("Vector index %zu out of bounds! Return zero.", i);
    else
        return data[i];

    return constZero();
}

tsym::Vector& tsym::Vector::operator += (const Vector& rhs)
{
    if (dim == rhs.dim)
        for (size_t i = 0; i < dim; ++i)
            data[i] += rhs.data[i];
    else
        TSYM_ERROR("Illegal vector addition (dimensions: %zu, %zu!"
                " Return unmodified left hand side.", dim, rhs.dim);

    return *this;
}

tsym::Vector& tsym::Vector::operator -= (const Vector& rhs)
{
    if (dim == rhs.dim)
        for (size_t i = 0; i < dim; ++i)
            data[i] -= rhs.data[i];
    else
        TSYM_ERROR("Illegal vector subtraction (dimensions: %zu, %zu!"
                " Return unmodified left hand side.", dim, rhs.dim);

    return *this;
}

tsym::Vector& tsym::Vector::operator *= (const Var& rhs)
{
    for (size_t i = 0; i < dim; ++i)
        data[i] *= rhs;

    return *this;
}

const tsym::Vector& tsym::Vector::operator + () const
{
    return *this;
}

tsym::Vector tsym::Vector::operator - () const
{
    Vector res(dim);

    for (size_t i = 0; i < dim; ++i)
        res.data[i] = -data[i];

    return res;
}

tsym::Var tsym::Vector::dotProduct(const Vector& other) const
{
    Var dotProduct;

    if (dim != other.dim)
        TSYM_ERROR("Illegal vector dot product (dim.: %zu, %zu)", dim, other.dim);
    else
        for (size_t i = 0; i < dim; ++i)
            dotProduct += data[i]*other.data[i];

    return dotProduct;
}

size_t tsym::Vector::size() const
{
    return dim;
}

bool tsym::Vector::equal(const Vector& other) const
{
    if (dim != other.dim)
        return false;

    for (size_t i = 0; i < dim; i++)
        if (data[i] != (other.data[i]))
            return false;

    return true;
}

bool tsym::operator == (const Vector& lhs, const Vector& rhs)
{
    return lhs.equal(rhs);
}

bool tsym::operator != (const Vector& lhs, const Vector& rhs)
{
    return !lhs.equal(rhs);
}

tsym::Vector tsym::operator + (Vector lhs, const Vector& rhs)
{
    lhs += rhs;

    return lhs;
}

tsym::Vector tsym::operator - (Vector lhs, const Vector& rhs)
{
    lhs -= rhs;

    return lhs;
}

tsym::Vector tsym::operator * (Vector lhs, const Var& rhs)
{
    lhs *= rhs;

    return lhs;
}

tsym::Vector tsym::operator * (const Var& lhs, Vector rhs)
{
    rhs *= lhs;

    return rhs;
}

std::ostream& tsym::operator << (std::ostream& stream, const Vector& v)
{
    Printer printer(v);

    printer.print(stream);

    return stream;
}
