
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
    }
}

tsym::Trigonometric::Trigonometric(const BasePtr& arg, Type type) :
    Function(arg, getStr(type)),
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

tsym::BasePtr tsym::Trigonometric::create(Type type, const BasePtr& arg)
{
    if (arg->isUndefined())
        return arg;
    else if (arg->isFunction())
        return createFromFunction(type, arg);
    else if (arg->isNumericallyEvaluable())
        return createNumerically(type, arg);
    else
        return BasePtr(new Trigonometric(arg, type));
}

tsym::BasePtr tsym::Trigonometric::createNumerically(Type type, const BasePtr& arg)
{
    NumTrigoSimpl numTrigo;

    numTrigo.setType(type);
    numTrigo.setArg(arg);
    numTrigo.compute();

    if (numTrigo.hasSimplifiedResult())
        return numTrigo.get();
    else
        return BasePtr(new Trigonometric(arg, type));
}

tsym::BasePtr tsym::Trigonometric::createFromFunction(Type type, const BasePtr& arg)
{
    const Trigonometric *trigo(tryCast(arg));

    if (trigo == NULL)
        return BasePtr(new Trigonometric(arg, type));
    else
        return createFromTrigo(type, arg);
}

const tsym::Trigonometric *tsym::Trigonometric::tryCast(const BasePtr& arg)
{
    const Trigonometric *cast;

    cast = dynamic_cast<const Trigonometric*>(arg.base());

    return cast;
}

tsym::BasePtr tsym::Trigonometric::createFromTrigo(Type type, const BasePtr& arg)
{
    const Trigonometric *other(tryCast(arg));
    const Type otherType = other->type;

    assert(other != NULL);

    if (isOtherTheInverse(type, otherType))
        return other->arg;
    else if (isThisTheInverse(type, otherType) && arg->isNumericallyEvaluable())
        /* If the argument lies in an invalid range for the inner trigonometric function, the result
         * has been Undefined in the first place, thus no additional checks necessary here. */
        return shiftArgIntoRange(type, other->arg);
    else
        return createFromTrigoNoInverse(type, arg);
}

bool tsym::Trigonometric::isOtherTheInverse(Type type, Type otherType)
    /* Doesn't return true for a general pair of e.g. asin - sin, the first type must be the
     * non-inverse part. */
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
    const BasePtr two(Numeric::create(2));
    const BasePtr twoPi(Product::create(two, Constant::createPi()));
    BasePtr interval[2];
    BasePtr endFactor;

    defineIntervalAndEndFactor(type, interval, endFactor);

    while (arg->numericEval() >= interval[1]->numericEval())
        arg = Sum::create(arg, Product::minus(twoPi));

    while (arg->numericEval() < interval[0]->numericEval())
        arg = Sum::create(arg, twoPi);

    if (arg->numericEval() >= interval[1]->numericEval())
        arg = Sum::create(Product::create(endFactor, two, interval[1]),
                Product::minus(endFactor, arg));

    return arg;
}

void tsym::Trigonometric::defineIntervalAndEndFactor(Type type, BasePtr *interval, BasePtr& factor)
{
    const BasePtr pi(Constant::createPi());
    const BasePtr piHalf(Product::create(pi, Numeric::create(1, 2)));

    if (type == ASIN || type == ATAN) {
        interval[0] = Product::minus(piHalf);
        interval[1] = piHalf;
    } else {
        interval[0] = Numeric::zero();
        interval[1] = pi;
    }

    factor = type == ATAN ? Numeric::mOne() : Numeric::one();
}

tsym::BasePtr tsym::Trigonometric::createFromTrigoNoInverse(Type type, const BasePtr& arg)
{
    const Trigonometric *other(tryCast(arg));
    const Type otherType = other->type;
    const BasePtr one(Numeric::one());
    const BasePtr aux1(Power::sqrt(Sum::create(one, Product::minus(square(other->arg)))));
    const BasePtr aux2(Power::sqrt(Sum::create(one, square(other->arg))));

    if ((type == SIN && otherType == ACOS) || (type == COS && otherType == ASIN))
        return aux1;
    else if (type == SIN && otherType == ATAN)
        return Fraction(other->arg, aux2).eval();
    else if (type == COS && otherType == ATAN)
        return Fraction(one, aux2).eval();
    else if (type == TAN && otherType == ASIN)
        return Fraction(other->arg, aux1).eval();
    else if (type == TAN && otherType == ACOS)
        return Fraction(aux1, other->arg).eval();
    else
        return BasePtr(new Trigonometric(arg, type));
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
    }

    return fct(arg->numericEval().toDouble());
}

tsym::Fraction tsym::Trigonometric::normal(SymbolMap& map) const
    /* Normalizes the function argument and replaces itself with a temporary symbol afterwards. */
{
    const BasePtr result(create(type, arg->normal()));
    const BasePtr replacement(map.getTmpSymbolAndStore(result));

    return Fraction(replacement);
}

tsym::BasePtr tsym::Trigonometric::diffWrtSymbol(const BasePtr& symbol) const
{
    const BasePtr outerDerivate(arg->diffWrtSymbol(symbol));
    const BasePtr innerDerivative(innerDiff());

    return Product::create(innerDerivative, outerDerivate);
}

tsym::BasePtr tsym::Trigonometric::innerDiff() const
{
    BasePtr tmp;

    switch (type) {
        case SIN:
            return create(COS, arg);
        case COS:
            return Product::minus(create(SIN, arg));
        case TAN:
            return Sum::create(Numeric::one(), square(create(TAN, arg)));
        case ASIN:
            tmp = Sum::create(Numeric::one(), Product::minus(square(arg)));
            return Power::create(tmp, Numeric::create(-1, 2));
        case ACOS:
            tmp = Sum::create(Numeric::one(), Product::minus(square(arg)));
            return Product::minus(Power::create(tmp, Numeric::create(-1, 2)));
        case ATAN:
            tmp = Sum::create(Numeric::one(), square(arg));
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
    else
        return create(type, arg->subst(from, to));
}
