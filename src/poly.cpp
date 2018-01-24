
#include <cassert>
#include "poly.h"
#include "power.h"
#include "product.h"
#include "numeric.h"
#include "sum.h"
#include "logging.h"
#include "polyinfo.h"
#include "primitivegcd.h"
#include "subresultantgcd.h"
#include "cache.h"

namespace tsym {
    static BasePtrList divideEmptyList(const BasePtr& u, const BasePtr& v);
    static BasePtrList divideNonEmpty(const BasePtr& u, const BasePtr& v, const BasePtrList& L);
    static BasePtrList pseudoDivide(const BasePtr& u, const BasePtr& v, const BasePtr& x,
            bool computeQuotient);
    static BasePtrList pseudoDivideChecked(const BasePtr& u, const BasePtr& v, const BasePtr& x,
            bool computeQuotient);
    static int unitFromNonNumeric(const BasePtr& polynomial);
    static BasePtr getFirstSymbol(const BasePtr& polynomial);
    static BasePtr getFirstSymbol(const BasePtrList& polynomials);
    static const GcdStrategy *defaultGcd();
    static BasePtr nonTrivialContent(const BasePtr& expandedPolynomial, const BasePtr& x,
            const GcdStrategy *algo);
    static int minDegreeOfPower(const BasePtr& power, const tsym::BasePtr& variable);
    static int minDegreeOfSum(const BasePtr& sum, const tsym::BasePtr& variable);
    static int minDegreeOfProduct(const BasePtr& product, const tsym::BasePtr& variable);
}

tsym::BasePtrList tsym::poly::divide(const BasePtr& u, const BasePtr& v)
{
    static Cache<BasePtrList, BasePtrList> cache;
    const BasePtrList *cached(cache.retrieve({ u, v }));
    BasePtrList result;

    if (cached != nullptr)
        result = *cached;
    else
        result = divide(u, v, PolyInfo(u, v).listOfSymbols());

    return cache.insertAndReturn({ u, v }, result);
}

tsym::BasePtrList tsym::poly::divide(const BasePtr& u, const BasePtr& v, const BasePtrList& L)
    /* This function implements the algorithm given in Cohen, Computer Algebra and Symbolic
     * Computation [2003], page 211. */
{
    const BasePtr& zero(Numeric::zero());
    PolyInfo polyInfo(u, v);

    if (!polyInfo.isInputValid()) {
        TSYM_ERROR("Invalid polynomial division: %S, %S. Return Undefined quotient and remainder.",
                u, v);
        return BasePtrList(Undefined::create(), Undefined::create());
    } else if (L.empty())
        return divideEmptyList(u, v);
    else if (v->isEqual(u))
        return BasePtrList(Numeric::one(), zero);
    else if (u->isZero())
        return BasePtrList(zero, zero);
    else
        return divideNonEmpty(u, v, L);
}

tsym::BasePtrList tsym::divideEmptyList(const tsym::BasePtr& u, const tsym::BasePtr& v)
{
    const BasePtr quotient(Product::create(u, Power::oneOver(v)));

    if (quotient->isNumeric() && quotient->numericEval().isRational())
        return BasePtrList(quotient, Numeric::zero());
    else
        return BasePtrList(Numeric::zero(), u);
}

tsym::BasePtrList tsym::divideNonEmpty(const BasePtr& u, const BasePtr& v, const BasePtrList& L)
    /* The central part of the algorithm described in Cohen [2003]. */
{
    const BasePtr& x(L.front());
    BasePtr quotient(Numeric::zero());
    BasePtr remainder(u);
    int m = u->degree(x);
    int n = v->degree(x);
    BasePtrList d;
    BasePtr tmp;
    BasePtr c;

    assert(x->isSymbol());

    while (m >= n) {
        assert(m >= 0 && n >= 0);

        d = poly::divide(remainder->leadingCoeff(x), v->leadingCoeff(x), L.rest());

        if (!d.back()->isZero())
            return BasePtrList(quotient->expand(), remainder);

        c = d.front();
        tmp = Power::create(x, Numeric::create(m - n));
        quotient = Sum::create(quotient, Product::create(c, tmp));

        remainder = Sum::create(remainder, Product::minus(c, v, tmp))->expand();

        if (remainder->isZero())
            break;

        m = remainder->degree(x);
    }

    return BasePtrList(quotient->expand(), remainder);
}

tsym::BasePtrList tsym::poly::pseudoDivide(const BasePtr& u, const BasePtr& v, const BasePtr& x)
    /* See Cohen, Computer Algebra and Symbolic Computation [2003], page 240. */
{
    return pseudoDivide(u, v, x, true);
}

tsym::BasePtrList tsym::pseudoDivide(const BasePtr& u, const BasePtr& v, const BasePtr& x,
        bool computeQuotient)
{
    PolyInfo polyInfo(u, v);

    if (polyInfo.isInputValid())
        return pseudoDivideChecked(u, v, x, computeQuotient);

    TSYM_ERROR("Invalid polyn. pseudo-division: %S, %S. Return Undefined quotient/remainder.",  u,
            v);

    return BasePtrList(Undefined::create(), Undefined::create());
}

tsym::BasePtrList tsym::pseudoDivideChecked(const BasePtr& u, const BasePtr& v, const BasePtr& x,
        bool computeQuotient)
{
    const BasePtr lCoeffV(v->leadingCoeff(x));
    const int n = v->degree(x);
    BasePtr quotient(Numeric::zero());
    BasePtr remainder(u->expand());
    BasePtr lCoeffR;
    BasePtr tmp;
    int m = u->degree(x);
    int sigma = 0;

    assert(!v->expand()->isZero());

    while (m >= n) {
        lCoeffR = remainder->coeff(x, m);

        tmp = Product::create(lCoeffR, Power::create(x, Numeric::create(m - n)));

        if (computeQuotient)
            quotient = Sum::create(Product::create(lCoeffV, quotient), tmp);

        remainder = Sum::create(Product::create(lCoeffV, remainder),
                Product::minus(v, tmp))->expand();

        if (remainder->isZero())
            break;

        ++sigma;
        m = remainder->degree(x);
    }

    tmp = Power::create(lCoeffV, Numeric::create(std::max(u->degree(x) - n + 1, 0) - sigma));

    remainder = Product::create(tmp, remainder)->expand();
    quotient = computeQuotient ? Product::create(tmp, quotient)->expand() : Numeric::zero();

    return BasePtrList(quotient, remainder);
}

tsym::BasePtr tsym::poly::pseudoRemainder(const BasePtr& u, const BasePtr& v, const BasePtr& x)
{
    return pseudoDivide(u, v, x, false).back();
}

int tsym::poly::unit(const BasePtr& polynomial, const BasePtr& x)
{
    const BasePtr expanded(polynomial->expand());
    const BasePtr lCoeff(expanded->leadingCoeff(x));

    if (lCoeff->isZero())
        return 1;
    else if (lCoeff->isNumeric())
        return lCoeff->numericEval().sign();
    else
        return unitFromNonNumeric(lCoeff);
}

int tsym::unitFromNonNumeric(const BasePtr& polynomial)
{
    const BasePtr firstSymbol(getFirstSymbol(polynomial));

    if (firstSymbol->isUndefined()) {
        TSYM_ERROR("Polynomial unit request with illegal argument: %S", polynomial);
        return 1;
    }

    return poly::unit(polynomial, firstSymbol);
}

tsym::BasePtr tsym::getFirstSymbol(const BasePtr& polynomial)
{
    if (polynomial->isSymbol())
        return polynomial;
    else if (polynomial->isPower())
        return getFirstSymbol(polynomial->base());
    else if (polynomial->isSum() || polynomial->isProduct())
        return getFirstSymbol(polynomial->operands());

    return Undefined::create();
}

tsym::BasePtr tsym::getFirstSymbol(const BasePtrList& polynomials)
{
    BasePtr firstSymbol;

    for (const auto& item : polynomials) {
        firstSymbol = getFirstSymbol(item);

        if (!firstSymbol->isUndefined())
            return firstSymbol;
    }

    return Undefined::create();
}

tsym::BasePtr tsym::poly::gcd(const BasePtr& u, const BasePtr& v)
{
    static Cache<BasePtrList, BasePtr> cache;
    const BasePtr *cached(cache.retrieve({ u, v }));

    if (cached != nullptr)
        return *cached;
    else
        return cache.insertAndReturn({ u, v }, gcd(u, v, defaultGcd()));
}

const tsym::GcdStrategy *tsym::defaultGcd()
{
    static SubresultantGcd algo;

    return &algo;
}

tsym::BasePtr tsym::poly::gcd(const BasePtr& u, const BasePtr& v, const GcdStrategy *algo)
{
    return algo->compute(u, v);
}

tsym::BasePtr tsym::poly::content(const BasePtr& polynomial, const tsym::BasePtr& x)
{
    return content(polynomial, x, defaultGcd());
}

tsym::BasePtr tsym::poly::content(const BasePtr& polynomial, const tsym::BasePtr& x,
        const GcdStrategy *algo)
{
    const BasePtr expanded(polynomial->expand());

    if (expanded->isNumeric())
        /* This include the zero case. */
        return Numeric::create(expanded->numericEval().abs());
    else
        return nonTrivialContent(expanded, x, algo);
}

tsym::BasePtr tsym::nonTrivialContent(const BasePtr& expandedPolynomial, const BasePtr& x,
        const GcdStrategy *algo)
{
    const int minDegree = poly::minDegree(expandedPolynomial, x);
    const int degree = expandedPolynomial->degree(x);
    BasePtr content(Numeric::zero());

    for (int i = minDegree; i <= degree; ++i)
        content = poly::gcd(expandedPolynomial->coeff(x, i), content, algo);

    return content;
}

int tsym::poly::minDegree(const BasePtr& of, const BasePtr& variable)
{
    if (!variable->isSymbol())
        TSYM_WARNING("Requesting min. degree with non-Symbol argument %S", variable);

    if (of->isNumeric())
        return 0;
    else if (of->isEqual(variable))
        return 1;
    else if (of->isPower())
        return minDegreeOfPower(of, variable);
    else if (of->isSum())
        return minDegreeOfSum(of, variable);
    else if (of->isProduct())
        return minDegreeOfProduct(of, variable);

    return 0;
}

int tsym::minDegreeOfPower(const BasePtr& power, const tsym::BasePtr& variable)
{
    const Int largeExp = power->exp()->numericEval().numerator();
    const BasePtr base(power->base());
    int exp;

    if (!integer::fitsInto<int>(largeExp)) {
        TSYM_ERROR("%S: Exponent doesn't fit into primitive int! Return 0 (min. degree).", power);
        return 0;
    } else
        exp = static_cast<int>(largeExp);

    if (base->isEqual(variable))
        return exp;
    else
        return exp*poly::minDegree(base, variable);
}

int tsym::minDegreeOfSum(const BasePtr& sum, const tsym::BasePtr& variable)
{
    auto it(sum->operands().begin());
    int result = poly::minDegree(*it, variable);

    while (++it != sum->operands().end()) {
        int minDeg = poly::minDegree(*it, variable);
        if (minDeg < result)
            result = minDeg;
    }

    return result;
}

int tsym::minDegreeOfProduct(const BasePtr& product, const tsym::BasePtr& variable)
{
    int result = 0;

    for (const auto& factor : product->operands())
        result += poly::minDegree(factor, variable);

    return result;
}
