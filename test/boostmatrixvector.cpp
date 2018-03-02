
#include "boostmatrixvector.h"

BoostVector createBoostVector(std::initializer_list<tsym::Var> values)
{
    const auto dim = static_cast<BoostSizeType>(values.size());
    BoostVector vec(dim);
    BoostSizeType i = 0;

    for (const auto& entry: values)
        vec(i++) = entry;

    return vec;
}

BoostMatrix createBoostMatrix(std::initializer_list<std::initializer_list<tsym::Var>> values)
{
    const auto dim = static_cast<BoostSizeType>(values.size());
    BoostMatrix m(dim, dim);
    BoostSizeType i = 0;

    for (const auto& row: values) {
        BoostSizeType j = 0;
        for (const auto& entry: row)
            m(i, j++) = entry;
        ++i;
    }

    return m;
}

bool operator == (const BoostVector& lhs, const BoostVector& rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    for (BoostSizeType i = 0; i < lhs.size(); ++i)
        if (lhs(i) != rhs(i))
            return false;

    return true;
}

bool operator != (const BoostVector& lhs, const BoostVector& rhs)
{
    return !(lhs == rhs);
}

bool operator == (const BoostMatrix& lhs, const BoostMatrix& rhs)
{
    if (lhs.size1() != rhs.size1() || lhs.size2() != rhs.size2())
        return false;

    for (BoostSizeType i = 0; i < lhs.size1(); ++i)
        for (BoostSizeType j = 0; j < lhs.size2(); ++j)
            if (lhs(i, j) != rhs(i, j))
                return false;

    return true;
}

bool operator != (const BoostMatrix& lhs, const BoostMatrix& rhs)
{
    return !(lhs == rhs);
}
