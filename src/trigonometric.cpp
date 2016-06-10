
#include <cassert>
#include <cmath>
#include "trigonometric.h"
#include "product.h"
#include "power.h"
#include "sum.h"
#include "numeric.h"
#include "logging.h"
#include "constant.h"
#include "numtrigosimpl.h"
#include "symbolmap.h"

namespace tsym {
    namespace {
        /* Local helper functions for simplification below. */
        BasePtr square(const BasePtr& arg)
        {
            return Power::create(arg, Numeric::create(2));
        }

        BasePtr timesPi(int num, int denom = 1)
        {
            static const BasePtr pi(Constant::createPi());

            return Product::create(Numeric::create(num, denom), pi);
        }

        BasePtr atan2ArgEval(const BasePtr& y, const BasePtr& x)
        {
            return Product::create(y, Power::oneOver(x));
        }
    }
}

tsym::Trigonometric::Trigonometric(const BasePtrList& args, Type type) :
    Function(args, getStr(type)),
    arg1(ops.front()),
    /* Points to ops.front() except for atan2: */
    arg2(ops.back()),
    type(type)
{}

tsym::Trigonometric::~Trigonometric() {}

tsym::BasePtr tsym::Trigonometric::createSin(const BasePtr& arg)
{
    return create(SIN, arg);
}

tsym::BasePtr tsym::Trigonometric::createCos(const BasePtr& arg)
{
    return create(COS, arg);
}

tsym::BasePtr tsym::Trigonometric::createTan(const BasePtr& arg)
{
    return create(TAN, arg);
}

tsym::BasePtr tsym::Trigonometric::createAsin(const BasePtr& arg)
{
    return create(ASIN, arg);
}

tsym::BasePtr tsym::Trigonometric::createAcos(const BasePtr& arg)
{
    return create(ACOS, arg);
}

tsym::BasePtr tsym::Trigonometric::createAtan(const BasePtr& arg)
{
    return create(ATAN, arg);
}

tsym::BasePtr tsym::Trigonometric::createAtan2(const BasePtr& y, const BasePtr& x)
    /* This method differs from those above because of a higher number of function arguments. */
{
    if (x->isUndefined() || y->isUndefined())
        return Undefined::create();
    else if (x->isNumericallyEvaluable() && y->isNumericallyEvaluable())
        return createAtan2Numerically(y, x);
    else
        return BasePtr(new Trigonometric(BasePtrList(y, x), ATAN2));
}

tsym::BasePtr tsym::Trigonometric::create(Type type, const BasePtr& arg)
{
    if (arg->isUndefined())
        return arg;
    else if (arg->isFunction())
        return createFromFunction(type, arg);
    else if (doesSymmetryApply(arg))
        return createBySymmetry(type, arg);
    else if (arg->isNumericallyEvaluable())
        return createNumerically(type, arg);
    else
        return BasePtr(new Trigonometric(BasePtrList(arg), type));
}

bool tsym::Trigonometric::doesSymmetryApply(const BasePtr& arg)
{
    if (arg->isSum())
        return haveAllNegativePrefactors(arg->operands());
    else if (arg->isProduct())
        return arg->expand()->constTerm()->numericEval() < 0;
    else
        return false;
}

bool tsym::Trigonometric::haveAllNegativePrefactors(const BasePtrList& operands)
{
    for (BasePtrList::const_iterator it = operands.begin(); it != operands.end(); ++it)
        if ((*it)->constTerm()->numericEval() > 0)
            return false;

    return true;
}

tsym::BasePtr tsym::Trigonometric::createBySymmetry(Type type, const BasePtr& negativeArg)
{
    const BasePtr positiveArg(Product::minus(negativeArg));

    if (type == COS)
        return create(type, positiveArg);
    else if (type == ACOS)
        return Sum::create(timesPi(1), Product::minus(create(type, positiveArg)));
    else
        return Product::minus(create(type, positiveArg));
}

tsym::BasePtr tsym::Trigonometric::createNumerically(Type type, const BasePtr& arg)
{
    NumTrigoSimpl numTrigo;

    assert(arg->isNumericallyEvaluable());

    numTrigo.setType(type);
    numTrigo.setArg(arg);
    numTrigo.compute();

    if (numTrigo.hasSimplifiedResult())
        return numTrigo.get();
    else if (arg->numericEval() < 0)
        return createNumericallyBySymmetry(type, arg);
    else
        return BasePtr(new Trigonometric(BasePtrList(arg), type));
}

tsym::BasePtr tsym::Trigonometric::createNumericallyBySymmetry(Type type, const BasePtr& arg)
    /* Here, the final BasePtr has to be created directly, i.e., not by entering the create(...)
     * cycle again, as this can cause infinite loops. */
{
    const BasePtr positiveArg(Product::minus(arg));
    const BasePtr shiftedResult(new Trigonometric(BasePtrList(positiveArg), type));

    if (type == COS)
        return shiftedResult;
    else if (type == ACOS)
        return Sum::create(timesPi(1), Product::minus(shiftedResult));
    else
        return Product::minus(shiftedResult);
}

tsym::BasePtr tsym::Trigonometric::createFromFunction(Type type, const BasePtr& arg)
{
    const Trigonometric *trigo(tryCast(arg));

    if (trigo == NULL)
        return BasePtr(new Trigonometric(BasePtrList(arg), type));
    else
        return createFromTrigo(type, arg);
}

const tsym::Trigonometric *tsym::Trigonometric::tryCast(const BasePtr& arg)
{
    const Trigonometric *cast;

    cast = dynamic_cast<const Trigonometric*>(arg.base());

    assert(cast == NULL || arg->isFunction());

    return cast;
}

tsym::BasePtr tsym::Trigonometric::createFromTrigo(Type type, const BasePtr& arg)
{
    const Trigonometric *other(tryCast(arg));
    const Type otherType = other->type;

    assert(other != NULL);

    if (isOtherTheInverse(type, otherType))
        return other->arg1;
    else if (isTanOfAtan2(type, otherType))
        /* Must be handeled separately due to different function argument: */
        return atan2ArgEval(other->arg1, other->arg2);
    else if (isThisTheInverse(type, otherType) && arg->isNumericallyEvaluable())
        /* If the argument lies in an invalid range for the inner trigonometric function, the result
         * has been Undefined in the first place, thus no additional checks necessary here. */
        return shiftArgIntoRange(type, other->arg1);
    else
        return createFromTrigoNoInverse(type, arg);
}

bool tsym::Trigonometric::isOtherTheInverse(Type type, Type otherType)
    /* Doesn't return true for a general pair of e.g. asin - sin, the first type must be the
     * non-inverse part. Atan2 isn't considered here. */
{
    if (type == SIN)
        return otherType == ASIN;
    else if (type == COS)
        return otherType == ACOS;
    else if (type == TAN)
        return otherType == ATAN;
    else
        return false;
}

bool tsym::Trigonometric::isTanOfAtan2(Type type, Type otherType)
{
    return type == TAN && otherType == ATAN2;
}

bool tsym::Trigonometric::isThisTheInverse(Type type, Type otherType)
{
    return isOtherTheInverse(otherType, type);
}

tsym::BasePtr tsym::Trigonometric::shiftArgIntoRange(Type type, BasePtr arg)
    /* Asin(sin(...)), acos(cos(...)) and atan(tan(...)) are handled here, where the argument is
     * numerically evaluable. First, the argument is shifted into or closely above the range of
     * definition of the non-inverse trigonometric function (which is [-pi/2, pi/2] for sine and
     * tangent and [0, pi] for the cosine) by addition/subtraction of 2*pi. If the resulting
     * argument lies within this range, it is returned. Otherwise, it is subtracted from the double
     * of the interval and in case of atan(tan(...)) premultiplied by -1. */
{
    BasePtr interval[2];
    BasePtr endFactor;

    defineIntervalAndEndFactor(type, interval, endFactor);

    while (arg->numericEval() >= interval[1]->numericEval())
        arg = Sum::create(arg, timesPi(-2));

    while (arg->numericEval() < interval[0]->numericEval())
        arg = Sum::create(arg, timesPi(2));

    if (arg->numericEval() >= interval[1]->numericEval())
        arg = Sum::create(Product::create(endFactor, Numeric::create(2), interval[1]),
                Product::minus(endFactor, arg));

    return arg;
}

void tsym::Trigonometric::defineIntervalAndEndFactor(Type type, BasePtr *interval, BasePtr& factor)
{
    if (type == ASIN || type == ATAN) {
        interval[0] = timesPi(-1, 2);
        interval[1] = timesPi(1, 2);
    } else {
        interval[0] = Numeric::zero();
        interval[1] = timesPi(1);
    }

    factor = type == ATAN ? Numeric::mOne() : Numeric::one();
}

tsym::BasePtr tsym::Trigonometric::createFromTrigoNoInverse(Type type, const BasePtr& arg)
{
    const Trigonometric *other(tryCast(arg));
    const Type otherType = other->type;
    const BasePtr one(Numeric::one());
    const BasePtr aux1(Power::sqrt(Sum::create(one, Product::minus(square(other->arg1)))));
    const BasePtr aux2(Power::sqrt(Sum::create(one, square(other->arg1))));
    const BasePtr aux3(Power::sqrt(Sum::create(square(other->arg1), square(other->arg2))));

    if ((type == SIN && otherType == ACOS) || (type == COS && otherType == ASIN))
        return aux1;
    else if (type == SIN && otherType == ATAN)
        return Fraction(other->arg1, aux2).eval();
    else if (type == SIN && otherType == ATAN2)
        return Fraction(other->arg1, aux3).eval();
    else if (type == COS && otherType == ATAN)
        return Fraction(one, aux2).eval();
    else if (type == COS && otherType == ATAN2)
        return Fraction(other->arg2, aux3).eval();
    else if (type == TAN && otherType == ASIN)
        return Fraction(other->arg1, aux1).eval();
    else if (type == TAN && otherType == ACOS)
        return Fraction(aux1, other->arg1).eval();
    else
        return BasePtr(new Trigonometric(BasePtrList(arg), type));
}

tsym::BasePtr tsym::Trigonometric::createAtan2Numerically(const BasePtr& y, const BasePtr& x)
{
    const Number yNum(y->numericEval());
    const Number xNum(x->numericEval());
    BasePtr increment;

    if (xNum == 0 && yNum > 0)
        return timesPi(1, 2);
    else if (xNum == 0 && yNum < 0)
        return timesPi(3, 2);
    else if (xNum == 0 && yNum == 0) {
        logging::warning() << "Illegal construction atan2(0, 0)";
        return Undefined::create();
    } else if (xNum > 0)
        increment = Numeric::zero();
    else if (yNum >= 0)
        increment = timesPi(1);
    else
        increment = timesPi(-1);

    return simplAtan2(y, x, increment);
}

tsym::BasePtr tsym::Trigonometric::simplAtan2(const BasePtr& y, const BasePtr& x,
        const BasePtr& increment)
{
    const BasePtr atan2Arg(atan2ArgEval(y, x));
    const Trigonometric *trigo(tryCast(atan2Arg));
    NumTrigoSimpl numTrigo;

    if (trigo != NULL && trigo->type == TAN)
        return trigo->arg1;

    numTrigo.setType(ATAN);
    numTrigo.setArg(atan2ArgEval(y, x));
    numTrigo.compute();

    if (numTrigo.hasSimplifiedResult())
        return shiftAtanResultIntoRange(numTrigo.get(), increment);
    else if (atan2Arg->numericEval() < 0)
        return createBySymmetry(ATAN, atan2Arg);
    else
        return BasePtr(new Trigonometric(BasePtrList(atan2Arg), ATAN));
}

tsym::BasePtr tsym::Trigonometric::shiftAtanResultIntoRange(BasePtr result, BasePtr summand)
{
    BasePtr increment(timesPi(2));

    assert(result->isNumericallyEvaluable());

    if (result->isNumeric() && !result->isZero()) {
        increment = Numeric::create(increment->numericEval());
        summand = Numeric::create(summand->numericEval());
    }

    result = Sum::create(result, summand);

    while (result->numericEval() < 0)
        result = Sum::create(result, increment);

    while (result->numericEval() >= increment->numericEval())
        /* This shouldn't happen, as atan returns values < Pi/2, and even incrementoing this with Pi
         * doesn't lead to values greater than 2*Pi. */
        result = Sum::create(result, Product::minus(increment));

    return result;
}

std::string tsym::Trigonometric::getStr(Type type)
{
    switch (type) {
        case SIN:
            return "sin";
        case COS:
            return "cos";
        case TAN:
            return "tan";
        case ASIN:
            return "asin";
        case ACOS:
            return "acos";
        case ATAN:
            return "atan";
        case ATAN2:
            return "atan2";
        default:
            logging::fatal() << "Unknown trigonometric function type!";
            return "Unknown";
    }
}

tsym::Number tsym::Trigonometric::numericEval() const
{
    if (!isNumericallyEvaluable())
        return Number::createUndefined();
    else
        return checkedNumericEval();
}

tsym::Number tsym::Trigonometric::checkedNumericEval() const
{
    double (*fct)(double) = NULL;

    switch (type) {
        case SIN:
            fct = &std::sin;
            break;
        case COS:
            fct = &std::cos;
            break;
        case TAN:
            fct = &std::tan;
            break;
        case ASIN:
            fct = &std::asin;
            break;
        case ACOS:
            fct = &std::acos;
            break;
        case ATAN:
            fct = &std::atan;
            break;
        case ATAN2:
            return std::atan2(arg1->numericEval().toDouble(), arg2->numericEval().toDouble());
    }

    return fct(arg1->numericEval().toDouble());
}

tsym::Fraction tsym::Trigonometric::normal(SymbolMap& map) const
    /* Normalizes the function argument and replaces itself with a temporary symbol afterwards. */
{
    if (type == ATAN2)
        return normalAtan2(map);
    else
        return normalOtherThanAtan2(map);
}

tsym::Fraction tsym::Trigonometric::normalAtan2(SymbolMap& map) const
{
    const BasePtr normalizedArg1(arg1->normal());
    const BasePtr normalizedArg2(arg2->normal());
    const BasePtr result(createAtan2(normalizedArg1, normalizedArg2));

    return Fraction(map.getTmpSymbolAndStore(result));
}

tsym::Fraction tsym::Trigonometric::normalOtherThanAtan2(SymbolMap& map) const
{
    const BasePtr result(create(type, arg1->normal()));

    return Fraction(map.getTmpSymbolAndStore(result));
}

tsym::BasePtr tsym::Trigonometric::diffWrtSymbol(const BasePtr& symbol) const
{
    if (type != ATAN2)
        return diffWrtSymbol(arg1, symbol);
    else
        return diffWrtSymbol(atan2ArgEval(arg1, arg2), symbol);
}

tsym::BasePtr tsym::Trigonometric::diffWrtSymbol(const BasePtr& arg, const BasePtr& symbol) const
{
    const BasePtr outerDerivative(arg->diffWrtSymbol(symbol));
    const BasePtr innerDerivative(innerDiff());

    return Product::create(innerDerivative, outerDerivative)->normal();
}

tsym::BasePtr tsym::Trigonometric::innerDiff() const
{
    BasePtr tmp;

    switch (type) {
        case SIN:
            return create(COS, arg1);
        case COS:
            return Product::minus(create(SIN, arg1));
        case TAN:
            return Sum::create(Numeric::one(), square(create(TAN, arg1)));
        case ASIN:
            tmp = Sum::create(Numeric::one(), Product::minus(square(arg1)));
            return Power::create(tmp, Numeric::create(-1, 2));
        case ACOS:
            tmp = Sum::create(Numeric::one(), Product::minus(square(arg1)));
            return Product::minus(Power::create(tmp, Numeric::create(-1, 2)));
        case ATAN:
            tmp = Sum::create(Numeric::one(), square(arg1));
            return Power::oneOver(tmp);
        case ATAN2:
            tmp = Sum::create(Numeric::one(), square(atan2ArgEval(arg1, arg2)));
            return Power::oneOver(tmp);
        default:
            logging::fatal() << "Unknown trigonometric function type!";
            return Undefined::create();
    }
}

tsym::BasePtr tsym::Trigonometric::subst(const BasePtr& from, const BasePtr& to) const
{
    if (isEqual(from))
        return to;
    else if (type == ATAN2)
        return createAtan2(arg1->subst(from, to), arg2->subst(from, to));
    else
        return create(type, arg1->subst(from, to));
}

bool tsym::Trigonometric::isPositive() const
{
    if (type == ATAN)
        return arg1->isPositive();
    else
        return isNumericallyEvaluable() ? numericEval() > 0 : false;
}

bool tsym::Trigonometric::isNegative() const
{
    if (type == ATAN)
        return arg1->isNegative();
    else
        return isNumericallyEvaluable() ? numericEval() < 0 : false;
}
