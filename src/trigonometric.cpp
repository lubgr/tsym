
#include <cassert>
#include <cmath>
#include "trigonometric.h"
#include "product.h"
#include "power.h"
#include "sum.h"
#include "numeric.h"
#include "logging.h"
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
    else
        return createFromTrigoNoInverse(type, arg);
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

bool tsym::Trigonometric::isOtherTheInverse(Type type, Type otherType)
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
