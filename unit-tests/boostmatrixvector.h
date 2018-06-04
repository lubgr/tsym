#ifndef TSYM_TEST_BOOSTMATRIXVECTOR_H
#define TSYM_TEST_BOOSTMATRIXVECTOR_H

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <initializer_list>
#include "plu.h"

using BoostMatrix = boost::numeric::ublas::matrix<tsym::Var>;
using BoostVector = boost::numeric::ublas::vector<tsym::Var>;
using BoostSizeType = BoostMatrix::size_type;
typedef tsym::plu::detail::MatrixAccessProxy<BoostMatrix, BoostSizeType> BoostMatrixProxy;
typedef tsym::plu::detail::VectorAccessProxy<BoostVector, BoostSizeType> BoostVectorProxy;

BoostVector createBoostVector(std::initializer_list<tsym::Var> values);
BoostMatrix createBoostMatrix(std::initializer_list<std::initializer_list<tsym::Var>> values);

namespace tsym {
    bool operator==(const BoostVector& lhs, const BoostVector& rhs);
    bool operator!=(const BoostVector& lhs, const BoostVector& rhs);
    bool operator==(const BoostMatrix& lhs, const BoostMatrix& rhs);
    bool operator!=(const BoostMatrix& lhs, const BoostMatrix& rhs);
}

#endif
