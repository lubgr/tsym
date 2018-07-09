
#include "numtrigosimpl.h"
#include <cassert>
#include <cmath>
#include "logging.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "undefined.h"

namespace tsym {
    namespace {
        BasePtr timesPi(int num, int denom)
        {
            return Product::create(Numeric::create(num, denom), Constant::createPi());
        }

        const std::unordered_map<BasePtr, BasePtr>& sineTable()
        {
            static const BasePtr& zero(Numeric::zero());
            static const BasePtr& two(Numeric::two());
            static const BasePtr& half(Numeric::half());
            static const BasePtr& fourth(Numeric::fourth());
            static const BasePtr sqrtTwo(Power::sqrt(two));
            static const BasePtr sqrtSix(Power::sqrt(Numeric::create(6)));
            static const std::unordered_map<BasePtr, BasePtr> map{
              /* sin(0) = 0. */
              {zero, zero},
              /* sin(1/12*pi) = (sqrt(6) - sqrt(2))/4. */
              {timesPi(1, 12), Product::create(fourth, Sum::create(sqrtSix, Product::minus(sqrtTwo)))},
              /* sin(1/8*pi) = sqrt(2 - sqrt(2))/2. */
              {timesPi(1, 8), Product::create(half, Power::sqrt(Sum::create(two, Product::minus(sqrtTwo))))},
              /* sin(1/6*pi) = 1/2. */
              {timesPi(1, 6), half},
              /* sin(1/4*pi) = 1/sqrt(2). */
              {timesPi(1, 4), Power::oneOver(sqrtTwo)},
              /* sin(1/3*pi) = sqrt(3)/2. */
              {timesPi(1, 3), Product::create(half, Power::sqrt(Numeric::three()))},
              /* sin(3/8*pi) = sqrt(2 + sqrt(2))/2. */
              {timesPi(3, 8), Product::create(half, Power::sqrt(Sum::create(two, sqrtTwo)))},
              /* sin(5/12*pi) = (sqrt(6) + sqrt(2))/4. */
              {timesPi(5, 12), Product::create(fourth, Sum::create(sqrtSix, sqrtTwo))},
              /* sin(pi/2) = 1. */
              {timesPi(1, 2), Numeric::one()},
            };

            return map;
        }

        const std::unordered_map<BasePtr, BasePtr>& tanTable()
        {
            static const BasePtr& zero(Numeric::zero());
            static const BasePtr& one(Numeric::one());
            static const BasePtr& two(Numeric::two());
            static const BasePtr sqrtTwo(Power::sqrt(two));
            static const BasePtr sqrtThree(Power::sqrt(Numeric::three()));
            static const std::unordered_map<BasePtr, BasePtr> map{/* tan(0) = 0. */
              {zero, zero},
              /* tan(1/12*pi) = 2 - sqrt(3). */
              {timesPi(1, 12), Sum::create(two, Product::minus(sqrtThree))},
              /* tan(1/8*pi) = sqrt(2) - 1. */
              {timesPi(1, 8), Sum::create(sqrtTwo, Product::minus(one))},
              /* tan(1/6*pi) = 1/sqrt(3). */
              {timesPi(1, 6), Power::oneOver(sqrtThree)},
              /* tan(1/4*pi) = 1. */
              {timesPi(1, 4), one},
              /* tan(1/3*pi) = sqrt(3). */
              {timesPi(1, 3), sqrtThree},
              /* tan(3/8*pi) = sqrt(2) + 1. */
              {timesPi(3, 8), Sum::create(sqrtTwo, one)},
              /* tan(5/12*pi) = 2 + sqrt(3). */
              {timesPi(5, 12), Sum::create(two, sqrtThree)},
              /* tan(pi/2) = Undefined. */
              {timesPi(1, 2), Undefined::create()}};

            return map;
        }
    }
}

void tsym::NumTrigoSimpl::setType(Trigonometric::Type type)
{
    this->type = type;
}

void tsym::NumTrigoSimpl::setArg(const Number& arg)
{
    setArg(Numeric::create(arg));
}

void tsym::NumTrigoSimpl::setArg(const BasePtr& arg)
{
    assert(arg->isNumericallyEvaluable());

    this->arg = arg;
    origArg = arg;
}

void tsym::NumTrigoSimpl::compute()
{
    assert(!arg->isUndefined());

    reset();
    detour();
}

void tsym::NumTrigoSimpl::reset()
{
    isSimplified = false;
    sign = 1;
    arg = origArg;
    res = Undefined::create();
}

void tsym::NumTrigoSimpl::detour()
{
    if (isNotAnInverseFct())
        computeSinCosTan();
    else
        computeAsinAcosAtan();
}

bool tsym::NumTrigoSimpl::isNotAnInverseFct() const
{
    return type == Trigonometric::Type::SIN || type == Trigonometric::Type::COS || type == Trigonometric::Type::TAN;
}

void tsym::NumTrigoSimpl::computeSinCosTan()
{
    if (isArgRationalNonZeroNumeric())
        /* This can't be simplified, break early. */
        return;

    prepareSinCosTan();
    detourSinCosTan();
}

bool tsym::NumTrigoSimpl::isArgRationalNonZeroNumeric() const
{
    if (arg->isZero())
        return false;
    else
        return isRationalNumeric(arg);
}

bool tsym::NumTrigoSimpl::isRationalNumeric(const BasePtr& ptr) const
{
    if (ptr->isNumeric())
        return ptr->numericEval().isRational();
    else
        return false;
}

void tsym::NumTrigoSimpl::prepareSinCosTan()
{
    if (arg->isNumeric())
        adjustNumericArg();

    adjustArgRange();
}

void tsym::NumTrigoSimpl::adjustNumericArg()
{
    const Number n(arg->numericEval());

    arg = Product::create(Numeric::create(n / PI), Pi);
}

void tsym::NumTrigoSimpl::adjustArgRange()
{
    const BasePtr twoPi(timesPi(2, 1));

    /* The argument is a multiple of Pi, and it is shifted into 0 <= arg < 2*Pi. */
    while (arg->isNegative())
        arg = Sum::create(arg, twoPi);

    while (arg->numericEval() >= twoPi->numericEval())
        arg = Sum::create(arg, Product::minus(twoPi));
}

void tsym::NumTrigoSimpl::detourSinCosTan()
{
    if (type == Trigonometric::Type::SIN)
        sin();
    else if (type == Trigonometric::Type::COS)
        cos();
    else if (type == Trigonometric::Type::TAN)
        tan();
    else
        TSYM_ERROR("Wrong trigonometric function type!");
}

void tsym::NumTrigoSimpl::sin()
{
    const unsigned quadrant = getQuadrant();

    setSinSign(quadrant);
    shiftToFirstQuadrant(quadrant);

    return compShiftedSin();
}

unsigned tsym::NumTrigoSimpl::getQuadrant() const
{
    const BasePtr piMultiple(Product::create(arg, Power::oneOver(Pi)));
    const Number fac(piMultiple->numericEval());

    assert(fac >= 0 && fac < 2);

    if (fac < Number(1, 2))
        return 1;
    else if (fac < 1)
        return 2;
    else if (fac < Number(3, 2))
        return 3;
    else
        return 4;
}

void tsym::NumTrigoSimpl::setSinSign(unsigned quadrant)
{
    if (quadrant > 2)
        sign = -1;
    else
        sign = 1;
}

void tsym::NumTrigoSimpl::shiftToFirstQuadrant(unsigned quadrant)
{
    const BasePtr piHalf(timesPi(1, 2));

    assert(quadrant != 0 && quadrant <= 4);

    for (unsigned i = 1; i < quadrant; ++i)
        arg = Sum::create(arg, Product::minus(piHalf));

    if (quadrant == 2 || quadrant == 4)
        arg = Sum::create(piHalf, Product::minus(arg));
}

void tsym::NumTrigoSimpl::compShiftedSin()
{
    const BasePtr* exact(getValue(sineTable()));

    if (exact != nullptr)
        setResult(*exact);
    else if (isDoubleNumeric(origArg))
        compNumericalSin();
    else
        return;
}

const tsym::BasePtr* tsym::NumTrigoSimpl::getValue(const std::unordered_map<BasePtr, BasePtr>& table) const
/* Returns a pointer to the exact value (thus, the second entry of an element in the given
 * table), if one matches the argument. Numerical evaluation is carried out for all elements,
 * that don't exactly match. The latter could be made optional. However, the chance that the
 * following equality leads to a match by accident is extremely low. */
{
    const auto lookup = table.find(arg);

    return lookup == cend(table) ? getValueNumEval(table) : &lookup->second;
}

const tsym::BasePtr* tsym::NumTrigoSimpl::getValueNumEval(const std::unordered_map<BasePtr, BasePtr>& table) const
{
    const Number nArg(arg->numericEval());

    for (const auto& entry : table)
        if (nArg == entry.first->numericEval())
            return &entry.second;

    return nullptr;
}

void tsym::NumTrigoSimpl::setResult(const BasePtr& result)
{
    res = Product::create(Numeric::create(sign), result);
    isSimplified = true;
}

bool tsym::NumTrigoSimpl::isDoubleNumeric(const BasePtr& ptr) const
{
    if (ptr->isNumeric())
        return ptr->numericEval().isDouble();
    else
        return false;
}

void tsym::NumTrigoSimpl::compNumericalSin()
/* Shifts argument back to a plain Numeric, i.e., division by Constant Pi and multiplication
 * with (double) Numeric Pi. Then, the STL sine function is used. */
{
    arg = Product::create(arg, Power::oneOver(Pi));
    arg = Product::create(arg, Numeric::create(PI));

    compNumerically(&std::sin);
}

void tsym::NumTrigoSimpl::compNumerically(double (*fct)(double))
{
    double result;

    assert(arg->isNumeric());
    assert(arg->numericEval().isDouble());

    result = fct(arg->numericEval().toDouble());

    setResult(Numeric::create(result));
}

void tsym::NumTrigoSimpl::cos()
/* Implemented via cos(alpha + 90°) = sin(alpha). */
{
    arg = Sum::create(arg, timesPi(1, 2));

    /* The argument could be beyond 2*Pi now, thus adjust it if necessary. */
    adjustArgRange();

    sin();
}

void tsym::NumTrigoSimpl::tan()
{
    const unsigned quadrant = getQuadrant();
    const BasePtr* exact(getValue(tanTable()));

    setTanSign(quadrant);

    if (exact != nullptr)
        setResult(*exact);
    else
        tanViaSinCos();
}

void tsym::NumTrigoSimpl::setTanSign(unsigned quadrant)
{
    if (quadrant == 1 || quadrant == 3)
        sign = 1;
    else
        sign = -1;
}

void tsym::NumTrigoSimpl::tanViaSinCos()
{
    bool simplified = false;
    BasePtr cosine;
    BasePtr sine;

    setSinForTan(sine, simplified);
    setCosForTan(cosine, simplified);

    if (cosine->isZero())
        /* This should't happen, because tan(Pi/2) is in the tanTable. */
        setResult(Undefined::create());
    else if (simplified)
        setResult(Product::create(sine, Power::oneOver(cosine)));

    /* Restore original state. */
    type = Trigonometric::Type::TAN;
}

void tsym::NumTrigoSimpl::setSinForTan(BasePtr& result, bool& simplified)
{
    setForTan(Trigonometric::Type::SIN, result, simplified);
}

void tsym::NumTrigoSimpl::setCosForTan(BasePtr& result, bool& simplified)
{
    setForTan(Trigonometric::Type::COS, result, simplified);
}

void tsym::NumTrigoSimpl::setForTan(Trigonometric::Type type, BasePtr& result, bool& simplified)
{
    this->type = type;

    compute();

    result = res;
    simplified = isSimplified;

    reset();
}

void tsym::NumTrigoSimpl::computeAsinAcosAtan()
{
    if (isInverseArgOutOfRange()) {
        setResult(Undefined::create());
        return;
    }

    prepareAsinAcosAtan();
    detourAsinAcosAtan();
}

bool tsym::NumTrigoSimpl::isInverseArgOutOfRange() const
{
    const Number nArg(arg->numericEval());

    if (type == Trigonometric::Type::ATAN)
        return false;
    else if (nArg < -1 || nArg > 1)
        return true;
    else
        return false;
}

void tsym::NumTrigoSimpl::prepareAsinAcosAtan()
{
    const Number nArg(arg->numericEval());

    if (nArg < 0) {
        arg = Product::minus(arg);
        sign = -1;
    }
}

void tsym::NumTrigoSimpl::detourAsinAcosAtan()
{
    if (type == Trigonometric::Type::ASIN)
        asin();
    else if (type == Trigonometric::Type::ACOS)
        acos();
    else if (type == Trigonometric::Type::ATAN)
        atan();
    else
        TSYM_ERROR("Wrong trigonometric function type!");
}

void tsym::NumTrigoSimpl::asin()
{
    const BasePtr* exact(getKey(sineTable()));

    if (exact != nullptr)
        setResult(*exact);
    else if (isDoubleNumeric(origArg)) {
        reset();
        compNumerically(&std::asin);
    }
}

const tsym::BasePtr* tsym::NumTrigoSimpl::getKey(const std::unordered_map<BasePtr, BasePtr>& table) const
{
    const Number nArg(arg->numericEval());

    for (const auto& entry : table)
        if (entry.second->isUndefined())
            continue;
        else if (arg->isEqual(*entry.second))
            return &entry.first;
        else if (nArg == entry.second->numericEval())
            return &entry.first;

    return nullptr;
}

void tsym::NumTrigoSimpl::acos()
/* Implemented via acos(arg) = Pi/2 - asin(arg). */
{
    asin();

    if (!isSimplified)
        return;
    else
        acosFromAsinResult();
}

void tsym::NumTrigoSimpl::acosFromAsinResult()
{
    const BasePtr piHalf(timesPi(1, 2));

    if (isDoubleNumeric(res))
        res = Numeric::create(0.5 * PI - res->numericEval());
    else
        res = Sum::create(piHalf, Product::minus(res));
}

void tsym::NumTrigoSimpl::atan()
{
    const BasePtr* exact(getKey(tanTable()));

    if (exact != nullptr)
        setResult(*exact);
    else if (isDoubleNumeric(origArg)) {
        reset();
        compNumerically(&std::atan);
    }
}

bool tsym::NumTrigoSimpl::hasSimplifiedResult() const
{
    return isSimplified;
}

tsym::BasePtr tsym::NumTrigoSimpl::get() const
{
    if (isSimplified)
        return res;

    TSYM_ERROR("Requesting numeric trigonometric simplification for unsimplified "
               "expression! Return Undefined.");

    return Undefined::create();
}
