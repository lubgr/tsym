
#include "poly.h"
#include <boost/range/algorithm.hpp>
#include <cassert>
#include "bplist.h"
#include "cache.h"
#include "logging.h"
#include "numeric.h"
#include "polyinfo.h"
#include "power.h"
#include "primitivegcd.h"
#include "product.h"
#include "subresultantgcd.h"
#include "sum.h"
#include "undefined.h"

namespace tsym {
    namespace {
        BasePtrList pseudoDivideChecked(const BasePtr& u, const BasePtr& v, const Base& x, bool computeQuotient);
        BasePtr getFirstSymbol(const BasePtr& polynomial);
        BasePtr getFirstSymbol(const BasePtrList& polynomials);

        BasePtrList divideEmptyList(const BasePtr& u, const BasePtr& v)
        {
            const BasePtr quotient(Product::create(u, Power::oneOver(v)));

            if (quotient->isNumeric() && quotient->numericEval().isRational())
                return {quotient, Numeric::zero()};
            else
                return {Numeric::zero(), u};
        }

        BasePtrList divideNonEmpty(const BasePtr& u, const BasePtr& v, const BasePtrList& L)
        /* The central part of the algorithm described in Cohen [2003]. */
        {
            const Base& x(*L.front());
            BasePtr quotient(Numeric::zero());
            BasePtr remainder(u);
            int m = u->degree(x);
            int n = v->degree(x);
            BasePtrList d;
            BasePtr tmp;
            BasePtr c;

            assert(x.isSymbol());

            while (m >= n) {
                assert(m >= 0 && n >= 0);

                d = poly::divide(remainder->leadingCoeff(x), v->leadingCoeff(x), bplist::rest(L));

                if (!d.back()->isZero())
                    return {quotient->expand(), remainder};

                c = d.front();
                tmp = Power::create(x.clone(), Numeric::create(m - n));
                quotient = Sum::create(quotient, Product::create(c, tmp));

                remainder = Sum::create(remainder, Product::minus(c, v, tmp))->expand();

                if (remainder->isZero())
                    break;

                m = remainder->degree(x);
            }

            return {quotient->expand(), remainder};
        }

        BasePtrList pseudoDivideImpl(const BasePtr& u, const BasePtr& v, const Base& x, bool computeQuotient)
        {
            PolyInfo polyInfo(u, v);

            if (polyInfo.isInputValid())
                return pseudoDivideChecked(u, v, x, computeQuotient);

            TSYM_ERROR("Invalid polyn. pseudo-division: %S, %S. Return Undefined quotient/remainder.", u, v);

            return {Undefined::create(), Undefined::create()};
        }

        BasePtrList pseudoDivideChecked(const BasePtr& u, const BasePtr& v, const Base& x, bool computeQuotient)
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

                tmp = Product::create(lCoeffR, Power::create(x.clone(), Numeric::create(m - n)));

                if (computeQuotient)
                    quotient = Sum::create(Product::create(lCoeffV, quotient), tmp);

                remainder = Sum::create(Product::create(lCoeffV, remainder), Product::minus(v, tmp))->expand();

                if (remainder->isZero())
                    break;

                ++sigma;
                m = remainder->degree(x);
            }

            tmp = Power::create(lCoeffV, Numeric::create(std::max(u->degree(x) - n + 1, 0) - sigma));

            remainder = Product::create(tmp, remainder)->expand();
            quotient = computeQuotient ? Product::create(tmp, quotient)->expand() : Numeric::zero();

            return {quotient, remainder};
        }

        int unitFromNonNumeric(const BasePtr& polynomial)
        {
            const BasePtr firstSymbol(getFirstSymbol(polynomial));

            if (firstSymbol->isUndefined()) {
                TSYM_ERROR("Polynomial unit request with illegal argument: %S", polynomial);
                return 1;
            }

            return poly::unit(polynomial, firstSymbol);
        }

        BasePtr getFirstSymbol(const BasePtr& polynomial)
        {
            if (polynomial->isSymbol())
                return polynomial;
            else if (polynomial->isPower())
                return getFirstSymbol(polynomial->base());
            else if (polynomial->isSum() || polynomial->isProduct())
                return getFirstSymbol(polynomial->operands());

            return Undefined::create();
        }

        BasePtr getFirstSymbol(const BasePtrList& polynomials)
        {
            for (const auto& item : polynomials) {
                BasePtr firstSymbol = getFirstSymbol(item);

                if (!firstSymbol->isUndefined())
                    return firstSymbol;
            }

            return Undefined::create();
        }

        const Gcd& defaultGcd()
        {
            static SubresultantGcd algo;

            return algo;
        }

        BasePtr nonTrivialContent(const Base& expandedPolynomial, const Base& x, const Gcd& algo)
        {
            const int minDegree = poly::minDegree(expandedPolynomial, x);
            const int degree = expandedPolynomial.degree(x);
            BasePtr content(Numeric::zero());

            for (int i = minDegree; i <= degree; ++i)
                content = poly::gcd(expandedPolynomial.coeff(x, i), content, algo);

            return content;
        }

        int minDegreeOfPower(const Base& power, const tsym::Base& variable)
        {
            const Int largeExp = power.exp()->numericEval().numerator();
            const BasePtr base(power.base());
            int exp;

            if (!integer::fitsInto<int>(largeExp)) {
                TSYM_ERROR("%S: Exponent doesn't fit into primitive int! Return 0 (min. degree).", power);
                return 0;
            } else
                exp = static_cast<int>(largeExp);

            if (base->isEqual(variable))
                return exp;
            else
                return exp * poly::minDegree(*base, variable);
        }

        int minDegreeOfSum(const Base& sum, const tsym::Base& variable)
        {
            std::vector<int> degrees(sum.operands().size());

            boost::transform(sum.operands(), begin(degrees),
              [&variable](const auto& operand) { return poly::minDegree(*operand, variable); });

            return *boost::min_element(degrees);
        }

        int minDegreeOfProduct(const Base& product, const Base& variable)
        {
            int result = 0;

            for (const auto& factor : product.operands())
                result += poly::minDegree(*factor, variable);

            return result;
        }
    }
}

tsym::BasePtrList tsym::poly::divide(const BasePtr& u, const BasePtr& v)
{
    static cache::RegisteredCache<BasePtrList, BasePtrList> cache;
    static auto& map(cache.map);
    const auto lookup = map.find({u, v});

    if (lookup != cend(map))
        return lookup->second;

    auto result = divide(u, v, PolyInfo(u, v).listOfSymbols());

    return map.insert({{u, v}, result})->second;
}

tsym::BasePtrList tsym::poly::divide(const BasePtr& u, const BasePtr& v, const BasePtrList& L)
/* This function implements the algorithm given in Cohen, Computer Algebra and Symbolic
 * Computation [2003], page 211. */
{
    const BasePtr& zero(Numeric::zero());
    PolyInfo polyInfo(u, v);

    if (!polyInfo.isInputValid()) {
        TSYM_ERROR("Invalid polynomial division: %S, %S. Return Undefined quotient and remainder.", u, v);
        return {Undefined::create(), Undefined::create()};
    } else if (L.empty())
        return divideEmptyList(u, v);
    else if (v->isEqual(*u))
        return {Numeric::one(), zero};
    else if (u->isZero())
        return {zero, zero};
    else
        return divideNonEmpty(u, v, L);
}

tsym::BasePtrList tsym::poly::pseudoDivide(const BasePtr& u, const BasePtr& v, const BasePtr& x)
/* See Cohen, Computer Algebra and Symbolic Computation [2003], page 240. */
{
    return pseudoDivideImpl(u, v, *x, true);
}

tsym::BasePtr tsym::poly::pseudoRemainder(const BasePtr& u, const BasePtr& v, const BasePtr& x)
{
    return pseudoDivideImpl(u, v, *x, false).back();
}

int tsym::poly::unit(const BasePtr& polynomial, const BasePtr& x)
{
    const BasePtr expanded(polynomial->expand());
    const BasePtr lCoeff(expanded->leadingCoeff(*x));

    if (lCoeff->isZero())
        return 1;
    else if (lCoeff->isNumeric())
        return lCoeff->numericEval().sign();
    else
        return unitFromNonNumeric(lCoeff);
}

tsym::BasePtr tsym::poly::gcd(const BasePtr& u, const BasePtr& v)
{
    static cache::RegisteredCache<BasePtrList, BasePtr> cache;
    static auto& map(cache.map);
    const auto lookup = map.find({u, v});

    if (lookup != cend(map))
        return lookup->second;
    else
        return map.insert({{u, v}, gcd(u, v, defaultGcd())})->second;
}

tsym::BasePtr tsym::poly::gcd(const BasePtr& u, const BasePtr& v, const Gcd& algo)
{
    return algo.compute(u, v);
}

tsym::BasePtr tsym::poly::content(const BasePtr& polynomial, const tsym::BasePtr& x)
{
    return content(polynomial, x, defaultGcd());
}

tsym::BasePtr tsym::poly::content(const BasePtr& polynomial, const tsym::BasePtr& x, const Gcd& algo)
{
    const BasePtr expanded(polynomial->expand());

    if (expanded->isNumeric())
        /* This include the zero case. */
        return Numeric::create(expanded->numericEval().abs());
    else
        return nonTrivialContent(*expanded, *x, algo);
}

int tsym::poly::minDegree(const Base& of, const Base& variable)
{
    if (!variable.isSymbol())
        TSYM_WARNING("Requesting min. degree with non-Symbol argument %S", variable);

    if (of.isNumeric())
        return 0;
    else if (of.isEqual(variable))
        return 1;
    else if (of.isPower())
        return minDegreeOfPower(of, variable);
    else if (of.isSum())
        return minDegreeOfSum(of, variable);
    else if (of.isProduct())
        return minDegreeOfProduct(of, variable);

    return 0;
}
