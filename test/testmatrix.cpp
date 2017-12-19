
#include <sstream>
#include "number.h"
#include "matrix.h"
#include "globals.h"
#include "logging.h"
#include "tsymtests.h"

using namespace tsym;

TEST_GROUP(Matrix)
{
    const Var zero = 0;
    const Var a = Var("a");
    const Var b = Var("b");
    const Var c = Var("c");
    const Var d = Var("d");
    const Var e = Var("e");
    const Var f = Var("f");
    const Var power = tsym::pow(a, 2*d);
    Matrix m;

    void setup()
    {

        m = Matrix(3, 3);
        m(0, 0) = a;
        m(0, 1) = Var(1, 2);
        m(0, 2) = b;
        m(1, 0) = zero;
        m(1, 1) = c + d;
        m(1, 2) = power; /* a^(2d). */
        m(2, 0) = 1;
        m(2, 1) = 5;
        m(2, 2) = b;
    }

    void checkMatrixM(const Matrix& m)
        /* Checks the given matrix to be identical with the one defined above. */
    {
        CHECK_EQUAL(3, m.rowSize());
        CHECK_EQUAL(3, m.colSize());

        CHECK_EQUAL(a, m(0, 0));
        CHECK_EQUAL(Var(1, 2), m(0, 1));
        CHECK_EQUAL(b, m(0, 2));
        CHECK_EQUAL(zero, m(1, 0));
        CHECK_EQUAL(c + d, m(1, 1));
        CHECK_EQUAL(power, m(1, 2));
        CHECK_EQUAL(1, m(2, 0));
        CHECK_EQUAL(5, m(2, 1));
        CHECK_EQUAL(b, m(2, 2));
    }
};

TEST(Matrix, rowAndColumnSize)
{
    const Matrix m(5, 17);

    CHECK_EQUAL(5, m.rowSize());
    CHECK_EQUAL(17, m.colSize());
}

TEST(Matrix, defaultZero)
{
    const Matrix zeroMatrix(2, 4);

    for (unsigned i = 0; i < 2; ++i)
        for (unsigned j = 0; j < 4; ++j)
            CHECK_EQUAL(zero, zeroMatrix(i, j));
}

TEST(Matrix, copyConstructor)
{
    const Matrix copy(m);

    checkMatrixM(copy);
}

TEST(Matrix, selfAssignment)
{
    m = m;

    checkMatrixM(m);
}

TEST(Matrix, assignmentLargerSize)
{
    Matrix otherM(2, 2);

    otherM = m;

    checkMatrixM(otherM);
}

TEST(Matrix, assignmentSmallerSize)
{
    Matrix otherM(5, 8);

    otherM = m;

    checkMatrixM(otherM);
}

TEST(Matrix, constructionWithInitializerList)
{
    const Matrix m {{ a, b, c }, { 1, 2, 3 }, { b, b, 0 }};

    CHECK_EQUAL(3, m.rowSize());
    CHECK_EQUAL(3, m.colSize());

    CHECK_EQUAL(a, m(0, 0));
    CHECK_EQUAL(2, m(1, 1));
    CHECK_EQUAL(b, m(2, 1));
}

TEST(Matrix, invalidConstructtionWithInitializerList)
{
    disableLog();
    const Matrix m {{ a, b, c }, { 2 }, { 0, 0, 0, b, 0 }};
    enableLog();

    CHECK_EQUAL(3, m.rowSize());
    CHECK_EQUAL(5, m.colSize());

    CHECK_EQUAL(c, m(0, 2));
    CHECK_EQUAL(2, m(1, 0));
    CHECK_EQUAL(b, m(2, 3));
    CHECK_EQUAL(0, m(2, 4));

    CHECK_EQUAL(0, m(0, 4));
    CHECK_EQUAL(0, m(1, 4));
}

TEST(Matrix, retainIndexOutOfRangeConst)
{
    const Matrix constM(m);

    disableLog();
    CHECK_EQUAL(zero, constM(5, 0));
    enableLog();
}

TEST(Matrix, retainIndexOutOfRangeUnallocatedConst)
{
    const Matrix m;

    disableLog();
    CHECK_EQUAL(0, m(0, 0));
    enableLog();
}

TEST(Matrix, squareMatrix)
{
    CHECK(m.isSquare());
}

TEST(Matrix, nonSquareMatrix)
{
    Matrix nonSquare(2, 3);

    CHECK_FALSE(nonSquare.isSquare());
}

TEST(Matrix, symmetricMatrix)
{
    Matrix sym(3, 3);

    sym(0, 0) = 0;
    sym(0, 1) = Var(1, 2);
    sym(0, 2) = b;
    sym(1, 0) = Var(1, 2);
    sym(1, 1) = 0;
    sym(1, 2) = 2*c + d;
    sym(2, 0) = b;
    sym(2, 1) = d + 2*c;
    sym(2, 2) = c;

    CHECK(sym.isSymmetric());
}

TEST(Matrix, squareNonSymmetricMatrix)
{
    CHECK_FALSE(m.isSymmetric());
}

TEST(Matrix, nonSquareNonSymmetricMatrix)
{
    Matrix nonSquare(3, 4);

    CHECK_FALSE(nonSquare.isSymmetric());
}

TEST(Matrix, equalMatrices)
{
    Matrix copy(m);

    CHECK(m == copy);
}

TEST(Matrix, differentMatrices)
{
    Matrix copy(m);

    copy(0, 0) = a*b*c;

    CHECK(m != copy);
}

TEST(Matrix, differentMatricesDimensions)
{
    Matrix diff(2, 3);

    CHECK(m != diff);
}

TEST(Matrix, transpose)
{
    m = m.transpose();

    CHECK_EQUAL(3, m.rowSize());
    CHECK_EQUAL(3, m.colSize());

    CHECK_EQUAL(a, m(0, 0));
    CHECK_EQUAL(zero, m(0, 1));
    CHECK_EQUAL(1, m(0, 2));
    CHECK_EQUAL(Var(1, 2), m(1, 0));
    CHECK_EQUAL(c + d, m(1, 1));
    CHECK_EQUAL(5, m(1, 2));
    CHECK_EQUAL(b, m(2, 0));
    CHECK_EQUAL(power, m(2, 1));
    CHECK_EQUAL(b, m(2, 2));

    m = m.transpose();

    checkMatrixM(m);
}

TEST(Matrix, transposeTestDimensionsOnly)
{
    Matrix t(2, 17);

    t = t.transpose();

    CHECK_EQUAL(17, t.rowSize());
    CHECK_EQUAL(2, t.colSize());
}

TEST(Matrix, simpleAddition)
{
    Matrix m2(3, 3);
    Matrix res;

    m2(0, 0) = 2*a;
    m2(0, 2) = -a;
    m2(1, 0) = 4;
    m2(1, 1) = b;
    m2(2, 1) = 9*d;
    m2(2, 2) = a + b;

    res = m + m2;

    CHECK_EQUAL(3*a, res(0, 0));
    CHECK_EQUAL(Var(1, 2), res(0, 1));
    CHECK_EQUAL(b - a, res(0, 2));
    CHECK_EQUAL(4, res(1, 0));
    CHECK_EQUAL(c + b + d, res(1, 1));
    CHECK_EQUAL(power, res(1, 2));
    CHECK_EQUAL(1, res(2, 0));
    CHECK_EQUAL(5 + 9*d, res(2, 1));
    CHECK_EQUAL(a + 2*b, res(2, 2));
}

TEST(Matrix, simpleSubtraction)
{
    Matrix m2(3, 3);
    Matrix res;

    m2(0, 0) = -a;
    m2(0, 2) = a;
    m2(1, 0) = 4;
    m2(1, 1) = 2*b;
    m2(2, 1) = 5*d;
    m2(2, 2) = -a + b;

    res = m - m2;

    CHECK_EQUAL(2*a, res(0, 0));
    CHECK_EQUAL(Var(1, 2), res(0, 1));
    CHECK_EQUAL(-a + b, res(0, 2));
    CHECK_EQUAL(-4, res(1, 0));
    CHECK_EQUAL(-2*b + c + d, res(1, 1));
    CHECK_EQUAL(power, res(1, 2));
    CHECK_EQUAL(1, res(2, 0));
    CHECK_EQUAL(-5*d + 5, res(2, 1));
    CHECK_EQUAL(a, res(2, 2));
}

TEST(Matrix, substractIdenticalMatrix)
{
    Matrix res(m);

    res -= m;

    for (unsigned i = 0; i < 3; ++i)
        for (unsigned j = 0; j < 3; ++j)
            CHECK_EQUAL(0, res(i, j));
}

TEST(Matrix, additionDifferentDimensions)
{
    Matrix m2(2, 3);

    disableLog();
    m += m2;
    enableLog();

    checkMatrixM(m);
}

TEST(Matrix, subtractionDifferentDimensions)
{
    Matrix m2(4, 1);

    disableLog();
    m -= m2;
    enableLog();

    checkMatrixM(m);
}

TEST(Matrix, multiplyByScalar)
{
    Var scalar(b);
    Var b2(b*b);
    Var b3(b*b2);

    m = m*scalar;
    m = scalar*m;

    CHECK_EQUAL(a*b2, m(0, 0));
    CHECK_EQUAL(Var(1, 2)*b2, m(0, 1));
    CHECK_EQUAL(b3, m(0, 2));
    CHECK_EQUAL(zero, m(1, 0));
    CHECK_EQUAL(b2*(c + d), m(1, 1));
    CHECK_EQUAL(b2*power, m(1, 2));
    CHECK_EQUAL(b2, m(2, 0));
    CHECK_EQUAL(5*b2, m(2, 1));
    CHECK_EQUAL(b3, m(2, 2));
}

TEST(Matrix, multipyTwoMatrices)
{
    Matrix m2(3, 2);
    Matrix res;

    m2(0, 0) = 2*a;
    m2(0, 1) = a;
    m2(1, 0) = 12;
    m2(1, 1) = b;
    m2(2, 0) = d;
    m2(2, 1) = b + c;

    res = m*m2;

    CHECK_EQUAL(3, res.rowSize());
    CHECK_EQUAL(2, res.colSize());

    CHECK_EQUAL(2*a*a + b*d + 6, res(0, 0));
    CHECK_EQUAL(a*a + b*(b + c) + Var(1, 2)*b, res(0, 1));
    CHECK_EQUAL(12*c + 12*d + d*power, res(1, 0));
    CHECK_EQUAL(b*(c + d) + power*(b + c), res(1, 1));
    CHECK_EQUAL(2*a + b*d + 60, res(2, 0));
    CHECK_EQUAL(a + b*(b + c) + 5*b, res(2, 1));
}

TEST(Matrix, multiplyTwoMatricesWrongDimensions)
{
    Matrix m2(4, 3);
    Matrix res;

    disableLog();
    res = m*m2;
    enableLog();

    checkMatrixM(m);
}

TEST(Matrix, matrixVectorProductDimensionCheck)
{
    const Matrix lhs(7, 2);
    const Vector rhs(2);
    const Vector result = lhs*rhs;

    CHECK_EQUAL(7, result.size());

    for (size_t i = 0; i < 7; ++i) {
        CHECK(result(i).isZero());
    }
}

TEST(Matrix, multiplyMatrixAndVector)
{
    Vector vec(3);
    Vector res;

    vec(0) = 2*b;
    vec(1) = 4;
    vec(2) = d;

    res = m*vec;

    CHECK_EQUAL(3, res.size());

    CHECK_EQUAL(2 + 2*a*b + b*d, res(0));
    CHECK_EQUAL(4*c + 4*d + d*power, res(1));
    CHECK_EQUAL(b*d + 2*b + 20, res(2));
}

TEST(Matrix, matrixVectorProductScalarResult)
{
    const Var expected = a + b*b + Var(4, 3);
    Matrix A(1, 3);
    Vector rhs(3);
    Vector res;

    A(0, 0) = a;
    A(0, 1) = b;
    A(0, 2) = 2;

    rhs(0) = 1;
    rhs(1) = b;
    rhs(2) = Var(2, 3);

    res = A*rhs;

    CHECK_EQUAL(1, res.size());
    CHECK_EQUAL(expected, res(0));
}

TEST(Matrix, multiplyMatrixAndVectorWrongDimensions)
{
    Vector vec(4);
    Vector res;

    disableLog();
    res = m*vec;
    enableLog();

    CHECK_EQUAL(3, res.size());
    for (size_t i = 0; i < res.size(); ++i) {
        CHECK(res(i).isZero());
    }
}

TEST(Matrix, luDecompWithoutPivoting)
{
    Matrix A(3, 3);
    Vector b(3);
    Vector x;

    A(0, 0) = 3;
    A(0, 1) = 4;
    A(0, 2) = 2;
    A(1, 0) = 0;
    A(1, 1) = 2;
    A(1, 2) = 1;
    A(2, 0) = 9;
    A(2, 1) = 22;
    A(2, 2) = 7;

    b(0) = 17;
    b(1) = 7;
    b(2) = 74;

    x = A.solve(b, Matrix::Pivoting::FIRST_NON_ZERO);

    CHECK_EQUAL(1, x(0));
    CHECK_EQUAL(2, x(1));
    CHECK_EQUAL(3, x(2));
}

TEST(Matrix, numericLuDecompPivoting)
{
    Matrix A(3, 3);
    Vector b(3);
    Vector x;

    A(0, 0) = 0;
    A(0, 1) = 2;
    A(0, 2) = 1;
    A(1, 0) = 3;
    A(1, 1) = 4;
    A(1, 2) = 2;
    A(2, 0) = 9;
    A(2, 1) = 22;
    A(2, 2) = 7;

    b(0) = 7;
    b(1) = 17;
    b(2) = 74;

    x = A.solve(b);

    CHECK_EQUAL(1, x(0));
    CHECK_EQUAL(2, x(1));
    CHECK_EQUAL(3, x(2));
}

TEST(Matrix, simplePivoting3x3)
{
    Matrix A(3, 3);
    Vector rhs(3);
    Vector x;

    A(0, 0) = tsym::pow(a, 2*sin(b));
    A(0, 1) = 17*b/29;
    A(1, 1) = 1/(a*b*c);
    A(1, 2) = tsym::pow(12, d);
    A(2, 0) = 1;
    A(2, 1) = 4*a;

    rhs(0) = tsym::pow(a, 2*sin(b))*d + 17*a*b/116;
    rhs(1) = tsym::pow(b, cos(b))*tsym::pow(12, d) + 1/(4*b*c);
    rhs(2) = d + a*a;

    x = A.solve(rhs, Matrix::Pivoting::FIRST_NON_ZERO);

    CHECK_EQUAL(3, x.size());
    CHECK_EQUAL(d, x(0));
    CHECK_EQUAL(a/4, x(1));
    CHECK_EQUAL(tsym::pow(b, cos(b)), x(2));
}

TEST(Matrix, luDecompPivotingByCycling)
{
    Matrix A(3, 3);
    Vector rhs(3);
    Vector x;

    A(0, 0) = a;
    A(0, 1) = 2;
    A(1, 1) = 1;
    A(2, 2) = 3;

    rhs(0) = 4;
    rhs(1) = 2;
    rhs(2) = 9;

    x = A.solve(rhs);

    CHECK_EQUAL(0, x(0));
    CHECK_EQUAL(2, x(1));
    CHECK_EQUAL(3, x(2));
}

TEST(Matrix, signSwitchDuringLUDecompWithPivoting2x2)
{
    const Matrix A {{ 0, 1 }, { 1, 0 }};
    const Vector rhs { a, b };
    const Vector expected { b, a };
    Vector x;

    x = A.solve(rhs);

    CHECK_EQUAL(expected, x);

    x = A.solve(rhs, Matrix::Pivoting::FIRST_NON_ZERO);

    CHECK_EQUAL(expected, x);
}

TEST(Matrix, signSwitchDuringLUDecompWithPivoting3x3)
{
    const Matrix A {{ 0, 1, a }, { b, 0, 2 }, { a, Var(-1, 2), 0 }};
    const Var expected(-a*b/2 + 2*a);

    CHECK_EQUAL(expected, A.det());
    CHECK_EQUAL(expected, A.det(Matrix::Pivoting::FIRST_NON_ZERO));
}

TEST(Matrix, noSignSwitchDuringLUDecompWithPivoting4x4)
{
    const Matrix A {{ 0, 1, a, 3 }, { b, 0, 2, 0 }, { a, Var(-1, 2), 0 , 2}, { 0, b, 3, 0 }};
    const Var expected(-6*a*b - 2*a*b*b + 21*b/2);

    CHECK_EQUAL(expected, A.det());
    CHECK_EQUAL(expected, A.det(Matrix::Pivoting::FIRST_NON_ZERO));
}

TEST(Matrix, linearEqSetDim2)
{
    Matrix A(2, 2);
    Vector rhs(2);
    Vector x;

    A(0, 0) = a;
    A(0, 1) = b;
    A(1, 0) = c;
    A(1, 1) = d;

    rhs(0) = a + b;
    rhs(1) = c + d;

    x = A.solve(rhs);

    CHECK_EQUAL(2, x.size());
    CHECK_EQUAL(1, x(0));
    CHECK_EQUAL(1, x(1));
}

TEST(Matrix, linearEqSetDim3)
{
    Vector rhs(3);
    Vector x;

    rhs(0) = 3*b/10 + b*f*f + e*a*a*f;
    rhs(1) = 3*(d + c)/5*b + tsym::pow(a, 2*d)*f*f;
    rhs(1) = Var(3, 5)*b*(d + c) + tsym::pow(a, 2*d)*f*f;
    rhs(2) = 3*b + b*f*f + e*a*f;

    x = m.solve(rhs);

    CHECK_EQUAL(3, x.size());
    CHECK_EQUAL(a*e*f, x(0));
    CHECK_EQUAL(3*b/5, x(1));
    CHECK_EQUAL(f*f, x(2));
}

TEST(Matrix, dependentLinearEqSetDim4)
{
    Matrix A(4, 4);
    Vector rhs(4);
    Vector x;

    A(0, 0) = a;
    A(0, 1) = b;
    A(1, 0) = 1;
    A(1, 1) = c;
    A(1, 2) = 2;
    A(1, 3) = 1;
    A(2, 2) = b*b;
    A(2, 3) = 1;
    A(3, 0) = a;
    A(3, 1) = a*c;
    A(3, 2) = 2*a;
    A(3, 3) = a;

    rhs(0) = 9;
    rhs(1) = a;
    rhs(2) = b*b;
    rhs(3) = a*a;

    disableLog();
    x = A.solve(rhs);
    enableLog();

    CHECK_EQUAL(0, x.size());
}

TEST(Matrix, illegalLinearEqSetNonSquareMatrix)
{
    const Matrix A(2, 3);
    const Vector rhs(3);
    Vector x;

    disableLog();
    x = A.solve(rhs);
    enableLog();

    CHECK_EQUAL(0, x.size());
}

TEST(Matrix, illegalLinearEqSetNonMatchingDimension)
{
    const Matrix A(4, 4);
    const Vector rhs(5);
    Vector x;

    disableLog();
    x = A.solve(rhs);
    enableLog();

    CHECK_EQUAL(0, x.size());
}

TEST(Matrix, illegalLinearEqSetZeroDimension)
{
    const Matrix A;
    const Vector rhs;
    Vector x;

    disableLog();
    x = A.solve(rhs);
    enableLog();

    CHECK_EQUAL(0, x.size());
}

TEST(Matrix, simpleSymbolDet)
{
    const Var expected(a*d - b*c);
    Matrix A(2, 2);

    A(0, 0) = a;
    A(0, 1) = b;
    A(1, 0) = c;
    A(1, 1) = d;

    CHECK_EQUAL(expected, A.det());
}

TEST(Matrix, simpleNumericDet)
{
    const Matrix A {{ 0, 1 }, { 1, 0 }};

    CHECK_EQUAL(-1, A.det());
}

TEST(Matrix, largeNumericDet)
{
    const Number expected(352255267.354137);
    const size_t size = 20;
    const Var APtr[size][size] = {
        { 0, -3, 0, 0, Var(2, 3), 3, 0, 2, 0.163763761215668, 1, 0.972645326126339, Var(-1,
                9), -2, Var(-2, 9), 0.518333759061494, -3, 2, 2, 0.784473042052998, 2 },
        { -0.817845830240927, 0, 0, 0.871111379270801, -0.587976833818062, 0, -0.730432211807773,
            -0.436438037403665, 0, 0, Var(-1, 3), 0, 0, 0.336413789908801, 0, 1, 0, 0, -1,
            Var(-1, 3) },
        { 0, 0, 0, -3, -2, 0, 0.432018803567675, 0.415954558314915, 0.334921106638503, 0,
            -0.441064077093597, -1, 0, -0.788960954477927, 0, -1, 2, 3, 0, 0.782883339525534 },
        { -0.701953171249646, Var(-2, 9), 0, 0.233319224614176, 0.421511139950083,
            -0.236214381078049, 2, 0.935533452453758, -1, -1, 3, -3, Var(1, 3),
            0.431540855728871, -1, 0, -0.304962232988921, Var(2, 9), 2, Var(-1, 3) },
        { 0.409536719769605, 0.169569519475026, Var(1, 3), 0, 0, 0.487494740627881, Var(1, 3),
            0, 1, -3, 0, 1, 0, -3, 0.197019152177339, Var(2, 9), 0, -0.217695943889126,
            -0.323049213293008, -2 },
        { -3, 2, 0.564340432881273, -0.726041746735617, 0.661481848233678, 0.251710811672905,
            0.291456760775844, 3, 0.743946440292804, -1, 0.893180217983624, -3, 3, 0,
            -0.517223090263141, Var(-1, 6), Var(-1, 3), -0.564726256451931, -1, 3 },
        { -0.362061416633498, 2, 0.878408900602927, Var(-1, 9), 0.285686875667206, -2, -2, 0,
            Var(-1, 6), 2, -1, 3, 0.464463147669131, 0, Var(1, 9), Var(1, 3), 0, 0, -3, -1
        },
        { -0.879212124759158, -0.814199905230186, 2, Var(2, 9), -0.198250188424975,
            0.501922631262054, 0, -2, Var(2, 9), -0.643714327219942, Var(-2, 3), -3, 2,
            Var(1, 9), Var(1, 6), -1, 0, Var(-1, 6), -0.891693022830127, -0.795647000362407
        },
        { 0.400446050091482, -2, Var(-1, 3), -0.179156659692230, -1, -0.102493130926656, 0, 1, 0,
            -2, -0.602360060388995, 0, 0, -1, -3, 0.472224469456821, -0.311738610520558, Var(1,
                    9), Var(-2, 9), -2 },
        { -2, -0.619736720716315, 0, -0.514268868053285, 1, -0.576144179562380, 2, 2, Var(1, 9),
            0.851523046057734, -2, Var(-1, 3), Var(1, 3), -0.661189346178467, -1, 2, -2, -3,
            0, 0.141647243131056 },
        { -3, -2, 0.756071627569365, 2, -3, 0, -0.286235664684729, 0.330354602667583,
            -0.404611840570539, -0.178961233748883, 0, 0, -3, 0, Var(-1, 3), -1, -3, 1, 2,
            -0.206800764892073 },
        { -0.780178283220871, 2, -1, 0, 0, -0.614305973735434, 0.334233970643127, 0.351167397856154,
            0.896517487896082, Var(1, 3), Var(-2, 9), 0, 0.556766076465591, 0,
            -0.255970680754231, 0.493476754055555, -2, -1, 0, -0.266899323261365 },
        { Var(-1, 3), 0, Var(-1, 3), -0.249453521690999, -3, 0.480133238056094,
            -0.712493294431367, 0.684729493715378, Var(1, 9), 0.634799018780988, Var(-1, 3),
            0, 0, Var(-2, 3), 1, 0.759958926984356, 1, -0.177062062068694, -0.738120757753549, -3
        },
        { Var(-1, 9), Var(2, 9), 0, Var(-1, 3), -2, Var(-1, 6), 0, 0,
            -0.991033486014383, Var(-1, 3), 2, 0, 0, 0, 0, -0.523520922745934, 1,
            0.841536360677068, -0.570936096606758, 0 },
        { -0.914577623243884, 0.643215642845554, Var(2, 3), Var(-2, 3), -1, Var(-1, 2),
            -0.448076097792330, -0.472765107276961, -1, 1, 1, -0.298450588426544, 0,
            -0.636200520881444, 0, 1, 0, 0, 0, -1 },
        { 0.560116920154630, 0.182250758566945, 0.790114920815374, 0, 0, 0, -1, 2,
            -0.751359753172712, -0.350507602358450, 0.445573509241984, Var(-1, 3),
            0.438068401333650, 0, 0.949396463448075, 0, 0.428948693635781, -0.432297272426761,
            -0.544789129383373, 0 },
        { -1, -0.918781660997322, -0.469630186237979, -0.501190892717313, -0.506809526188111, 0,
            -0.745707881530680, 0, 3, Var(-1, 2), 0.118891897894698, -0.671535749003615,
            0.420302665526368, 0.579728657232557, -0.971242483607050, 0.957223256031235, 0,
            Var(-2, 3), -0.726977244558762, 0.729950389364666 },
        { Var(1, 6), 0, 0, -1, 0.182417591159116, 0, Var(-2, 9), Var(-1, 6), 0, Var(2,
                3), 0, -3, -2, -2, 0.695995893453037, 3, -1, 0, -0.302923538999540,
        -0.193937987755568 },
        { 0, -0.877908243205685, 0, Var(-1, 3), 0, -1, -3, 0.305292463545492, 0.298548866432640,
            -3, 1, 0, Var(-1, 6), -0.390714584652508, -0.964800524467479, Var(-1, 6), 2,
            -0.891977924554555, 0, 0.318824163394123 },
        { Var(2, 9), Var(2, 3), 0.593093099557646, 0, 0.612785416052094, 0, Var(2, 3), -1,
            0, 0.854807696107082, 0.631006623400826, 0, 2, 0, -1, -0.290114424749472, Var(1, 2),
            Var(-1, 9), -0.420632521291579, 0 }
    };
    Matrix A(size, size);

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            A(i, j) = APtr[i][j];

    /* The tolerance shouldn't be too high here, because the resulting determinant is large. */
    DOUBLES_EQUAL(expected.toDouble(), A.det().toDouble(), 1.e-4);
}

TEST(Matrix, illegalDetRequestNonSquare)
    /* A non-square matrix should return zero as determinant. */
{
    Matrix A(2, 3);

    /* Some arbitrary non-zero entries. */
    A(0, 0) = a;
    A(0, 1) = 2;
    A(1, 1) = b;
    A(1, 2) = c + 3*d;

    disableLog();
    CHECK_EQUAL(0, A.det());
    enableLog();
}

TEST(Matrix, illegalDetRequestZeroDimension)
{
    Matrix A;

    disableLog();
    CHECK_EQUAL(0, A.det());
    enableLog();
}

TEST(Matrix, inverse2x2)
{
    Matrix A(2, 2);
    Matrix Ainv;
    Matrix one;

    A(0, 0) = a;
    A(0, 1) = b;
    A(1, 0) = c;
    A(1, 1) = d;

    Ainv = A.inverse();

    CHECK_EQUAL(2, Ainv.rowSize());
    CHECK_EQUAL(2, Ainv.colSize());

    one = A*Ainv;

    CHECK_EQUAL(1, one(0, 0).normal());
    CHECK_EQUAL(0, one(0, 1).normal());
    CHECK_EQUAL(1, one(1, 1).normal());
    CHECK_EQUAL(0, one(1, 0).normal());
}

TEST(Matrix, inverse3x3)
{
    Matrix A(3, 3);
    Matrix Ainv;
    Matrix one;

    A(0, 0) = a;
    A(0, 2) = 2*b;
    A(1, 0) = 10;
    A(1, 2) = c*d;
    A(2, 0) = a*a;
    A(2, 1) = b;
    A(2, 2) = 1;

    Ainv = A.inverse();

    CHECK_EQUAL(3, Ainv.rowSize());
    CHECK_EQUAL(3, Ainv.colSize());

    one = A*Ainv;

    for (size_t i = 0; i < 3; ++i) {
        CHECK_EQUAL(1, one(i, i).normal());
        for (size_t j = 0; j < 3; ++j)
            if (i != j) {
                CHECK_EQUAL(0, one(i, j).normal());
            }
    }
}

TEST(Matrix, largeDoubleInverse)
{
    const size_t size = 15;
    const Var APtr[size][size] = {
        { 9.29395026851320, 8.09033121585509, 5.14433525830085, 8.95249050693807, 6.96258322137449,
            4.11264527154185, 7.30290414594927, 9.37082878476097, 5.92939990091632,
            9.92026448526447, 6.16902185735627, 9.03313816781665, 5.45734909494898,
            8.98852263879611, 9.77177677229026 },
        { 4.83644760156884, 0, 4.84000869746979, 5.88337051798656, 4.70968745838288,
            3.66214064215514, 6.36230717624036, 3.50961417618395, 7.10581176984010,
            5.04166029537815, 2.98501351263885, 0.554722563774794, 2.96550468652435,
            7.40414657646940, 9.71714502100726 },
        { 5.20057329793591, 6.01009766751540, 1.71316070661009, 4.82084058527944, 4.54114692959545,
            0.945070546326013, 8.91574866473931, 8.01108136699260, 8.45182575466427,
            1.78514008364464, 3.78800832130653, 4.67115837166208, 7.18669413858628,
            2.27993849257348, 7.97237640672426 },
        { 0.561025730849580, 4.51258891898577, 2.27847708794378, 1.12754362691773, 7.43879850513026,
            6.47197296769185, 2.80893836509667, 7.69038720614297, 2.04368840164795,
            3.55508378637137, 5.92318484981771, 3.51922348365048, 8.91269422792576,
            9.63573495306973, 9.95285640971918 },
        { 8.73682388684816, 6.07077570539229, 6.42159414907996, 7.70457151194356, 0.791111649907894,
            7.99822805126661, 2.06406697074826, 6.40934069966954, 1.72747736324253,
            4.77243954227107, 1.88394196392722, 9.62850689915738, 2.16581789479282,
            1.96178221999787, 8.27788948946133 },
        { 7.68262445640236, 3.13318335485998, 4.59279740818428, 4.64149016258606, 1.24191514655683,
            0, 3.26797377447073, 7.52562359561980, 4.25482965002206, 1.73525641474494,
            1.08178228974695, 6.46199240855777, 3.25572719485427, 7.95316616228373, 7.85931353605145
        },
        { 3.36916771032943, 5.64049126807565, 5.27680747044406, 0.157335735959609, 7.33816419152185,
            3.59600014656244, 1.69509580117098, 9.05781704931730, 8.19611131415660,
            6.16728114589075, 1.57032243078534, 3.87307680870668, 5.85325587728887,
            2.36108850957649, 0.866762230017446 },
        { 0.574976392846100, 4.39075980793887, 5.02228193757148, 3.26566832850721, 1.98750778536600,
            9.57659564280478, 1.97988985325903, 2.58444491333057, 0.490094630292568,
            2.90518205537825, 3.50600599354464, 9.40279398297339, 0.225279774198123,
            0.530598418119101, 1.33504330100030 },
        { 7.58495385275344, 1.50183448509619, 8.28332029075993, 0.863869830881749, 7.58580284521211,
            9.44408695202429, 3.34824082948345, 0.257018679991457, 5.84379212414533,
            9.02498533309962, 1.25768463892998, 5.27143939279265, 5.27786438004969,
            7.25272212365802, 2.46073443141611 },
        { 4.16895493641656, 3.00199996861486, 7.91700724224825, 7.18400990478930, 6.44781518887440,
            2.51328471705331, 2.87583808065467, 3.77306225586734, 9.83678364562634,
            5.79081110656823, 3.29582643985839, 2.63564573715060, 3.74440312734826,
            9.08235768559694, 3.58456781835626 },
        { 8.13571414582448, 6.84640455616605, 6.44899816211425, 0.583563821392530, 8.41172404540720,
            5.51338933536693, 9.31172604196418, 0.544475486585051, 2.30694413209765,
            9.10470870651367, 2.94407160163276, 9.06301513849804, 1.12007007757400,
            5.38577885594940, 3.65809938637912 },
        { 4.97242785079997, 5.53975417080203, 1.74212763455302, 5.81236816633928, 9.68242261547356,
            6.98632607706483, 2.75833038887730, 8.88036703392089, 4.42380072714734,
            5.07822833212463, 8.92375572496908, 0, 3.56695961888082, 3.34810704987267,
            2.14664808823191 },
        { 2.00326171653685, 7.32420746258326, 3.68482906687982, 8.21143222879577, 5.85899463495970,
            3.95873024081868, 3.81417205321199, 3.30299998094557, 1.09148291019075,
            6.58063949671691, 3.58086483894916, 6.38884330539743, 2.90791512921258,
            2.92807219263060, 2.95328177453511 },
        { 6.80611836310833, 2.95080184762064, 3.34492866089684, 5.61981213289071, 5.01263459878027,
            1.41941696922046, 5.34452855396564, 2.73851703980014, 5.39863210114139,
            5.50395385215616, 6.35107686373044, 5.24791722993784, 1.73031360106109, 0,
            6.88234593102001 },
        { 8.36781127281422, 1.01339079242435, 1.46643566196019, 9.05988332172014, 8.32515502504598,
            8.79583647716310, 2.57238204217801, 4.42643857206381, 4.74812400429605,
            3.54101452266103, 3.75907522288290, 2.24228789070800, 7.49434585191406,
            6.74102530165140, 0 }
    };
    const Var AInvPtr[size][size] = {
        { -0.00288071968262256, -0.0693153332677301, 0.0156789565669110, -0.0147982395566584,
            0.0662556503668025, 0.0113197561148176, -0.0426257920665564, -0.0711917632459387,
            0.0287645810834458, 0.0156899167444463, 0.0322919799031649, 0.0398784253040378,
            -0.0638782530483221, 0.0111075282240292, 0.0137741796082851 },
        { 0.0846594711158889, -0.140026142463294, 0.0383415539370544, 0.104265391460832,
            0.263398342194149, -0.323118657136549, -0.0127609578156974, -0.114470602697389,
            -0.230425606063105, 0.281984439504022, 0.189416714278449, -0.0634571054151137,
            -0.220021678701846, -0.0926593496556599, 0.0406982098806788 },
        { -0.311647644431915, 0.00591490928785393, 0.166825315100842, -0.192197892813483,
            -0.277835736591644, 0.578981255003894, -0.204738712691484, 0.0372067783659400,
            0.617422076904644, -0.288588693079498, -0.321633679440864, 0.349205289799700,
            0.526894043040405, -0.0241024009504714, -0.326891021374311 },
        { -0.0330261215362486, 0.0447677144064443, -0.00814551157954093, -0.0351778564479346,
            -0.0112528689515597, 0.0360339447210369, -0.00995769684855987, 0.00116530088851475,
            -0.00776634120465172, -0.0155336720570947, -0.0418554969749489, -0.00558011356249475,
            0.111131181969656, 0.00457296612564604, 0.0292872566730555 },
        { -0.149417930545767, 0.0895103545143849, -0.192294303921549, 0.144335364258974,
            0.116149571462953, -0.123257187085581, 0.222970514629118, -0.0608324020023523,
            -0.391347831205904, 0.100024265837763, 0.251151619196449, -0.177539222869334,
            -0.0884700606720979, 0.132236160584511, 0.210189183026545 },
        { 0.0652113320342236, 0.0321029939865715, -0.0165044616348271, 0.0460609167812167,
            0.129287574749807, -0.205405869331669, 0.0306323229008375, 0.0282558019254022,
            -0.136901664980730, 0.0828766118715665, 0.0876136791582145, -0.0563408269755789,
            -0.177718690636502, -0.0626372419246596, 0.0801004968540340 },
        { -0.0707210860308481, 0.0826937350948717, 0.163637737542852, -0.165473000923057,
            -0.240340584949317, 0.292453169293325, -0.133559541994671, 0.0818874113711627,
            0.331906311019159, -0.250139661877045, -0.156468806178950, 0.186433764703030,
            0.271312358242801, -0.0903526195011965, -0.155621281415128 },
        { -0.0391053509439229, 0.0973770553908203, 0.00257446821720567, -0.103108747759235,
            -0.157687882421549, 0.256351911818072, 0.0366315439887857, 0.0700968201460340,
            0.149434326087054, -0.218536561254015, -0.132980423241944, 0.126253762680451,
            0.169568339295747, -0.0381941770419289, -0.0796576295069572 },
        { 0.176453191297798, -0.0278116895362268, -0.0688781377267835, 0.118759542607137,
            0.225051913806130, -0.433545230252534, 0.145665203089780, -0.00785012970951196,
            -0.413766765270447, 0.321587429406820, 0.214530705924181, -0.239550842742885,
            -0.423341277647616, 0.0271523624843426, 0.187764621473400 },
        { 0.173479373035539, 0.0329140317411679, -0.0203657038203875, -0.0651633277253249,
            -0.0743266215282311, -0.0277427862728045, 0.00836302241028621, 0.00494043350461448,
            0.137689276053252, -0.104369566100085, -0.109142120985255, 0.0118721968561130,
            0.0539234413375382, -0.0195901560295130, -0.0660333057418640 },
        { 0.00953685121258915, -0.120458404985550, 0.114759535524944, -0.0732938257541118,
            -0.182969768742052, 0.206258663231367, -0.208930418105406, 0.0705424764167445,
            0.334892097653176, -0.0861852122156680, -0.202841057410046, 0.219609315335593,
            0.122064195861557, 0.0388413146280774, -0.181154937369544 },
        { 0.0586075775454709, -0.0124227993588339, -0.104973533480439, 0.0738625315801758,
            0.0186719761605958, -0.0858652391043916, 0.120188366855191, 0.0736682545013683,
            -0.222910416327149, 0.0624541097887187, 0.102525973065975, -0.189132643081199,
            -0.133485071075621, 0.104228117824138, 0.139087065960051 },
        { -0.0857736961872437, -0.0896063520442892, 0.188206955154682, -0.0855890115246864,
            -0.191081343419576, 0.283599797237363, -0.165707112248890, -0.0101724979989318,
            0.470539088001210, -0.210756393605008, -0.279015914682410, 0.161923020051683,
            0.321713771552500, -0.0158519595215275, -0.168209215532831 },
        { 0.110269999182653, -0.0290676837557765, -0.0347897323362694, 0.0463156545164633,
            0.0256195089840706, -0.0813514129766720, -0.0219120015719012, 0.00730455595077183,
            -0.104386259984983, 0.118979106326161, 0.0756278692702257, -0.0467755358998044,
            -0.136808806048570, -0.0703240221074870, 0.0486641404959649 },
        { 0.00176683331752029, 0.0530310381347688, -0.0938712387835647, 0.138757391326374,
            0.189444688908547, -0.214346878750652, 0.0975557216032190, -0.0690616550846699,
            -0.270239299567082, 0.123891329811036, 0.148933843661104, -0.152214341949777,
            -0.169178936494543, 0.0670248276011444, 0.0957528399161877 }
    };
    const double TOL = 1.e-10;
    Matrix expected(size, size);
    Matrix A(size, size);
    Matrix Ainv;

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j) {
            A(i, j) = APtr[i][j];
            expected(i, j) = AInvPtr[i][j];
        }

    Ainv = A.inverse();

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            DOUBLES_EQUAL(expected(i, j).toDouble(), Ainv(i, j).toDouble(), TOL);
}

TEST(Matrix, illegalInverseNonSquare)
{
    Matrix A(2, 3);
    Matrix result;

    disableLog();
    result = A.inverse();
    enableLog();

    CHECK_EQUAL(0, result.rowSize());
    CHECK_EQUAL(0, result.colSize());
}

TEST(Matrix, illegalInverseSingular)
{
    Matrix A(2, 2);
    Matrix result;

    A(0, 0) = 2*a;
    A(0, 1) = -a*a;
    A(1, 1) = a;
    A(1, 0) = -2;

    disableLog();
    result = A.inverse();
    enableLog();

    CHECK_EQUAL(0, result.rowSize());
    CHECK_EQUAL(0, result.colSize());
}

TEST(Matrix, unaryPlusOperator)
{
    Matrix res;

    res = +m;

    checkMatrixM(res);
}

TEST(Matrix, unaryMinusOperator)
{
    Matrix res;

    res = -m;

    CHECK_EQUAL(-a, res(0, 0));
    CHECK_EQUAL(-Var(1, 2), res(0, 1));
    CHECK_EQUAL(-b, res(0, 2));
    CHECK_EQUAL(zero, res(1, 0));
    CHECK_EQUAL(-(c + d), res(1, 1));
    CHECK_EQUAL(-power, res(1, 2));
    CHECK_EQUAL(-1, res(2, 0));
    CHECK_EQUAL(-5, res(2, 1));
    CHECK_EQUAL(-b, res(2, 2));
}

TEST(Matrix, printerOperator)
{
    const std::string expect("[ a    1/2        b ]\n[ 0  c + d  a^(2*d) ]\n[ 1      5        b ]");
    std::stringstream stream;

    stream << m;

    CHECK_EQUAL(expect, stream.str());
}
