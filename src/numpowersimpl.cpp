
#include "numpowersimpl.h"
#include <cassert>
#include <cmath>
#include "logging.h"
#include "numberfct.h"

tsym::NumPowerSimpl::NumPowerSimpl(Payload data)
    : orig{data}
    , result{std::move(data)}
{
    compute();
}

tsym::NumPowerSimpl::NumPowerSimpl(Payload data, Int maxPrimeResolutionLimit)
    : orig{data}
    , result{std::move(data)}
    , maxPrimeLimit{std::move(maxPrimeResolutionLimit)}
{
    compute();
}

bool tsym::NumPowerSimpl::isInputValid() const
{
    return !(result.base < 0 && !isInt(result.exp));
}

const tsym::Number& tsym::NumPowerSimpl::getNewBase() const
{
    return result.base;
}

const tsym::Number& tsym::NumPowerSimpl::getNewExp() const
{
    return result.exp;
}

const tsym::Number& tsym::NumPowerSimpl::getPreFactor() const
{
    return result.preFactor;
}

void tsym::NumPowerSimpl::compute()
{
    if (!isInputValid())
        TSYM_ERROR("Illegal numeric power with base: %S and exponent %S", result.base, result.exp);
    else if (result.base.isDouble() || result.exp.isDouble())
        computeNonRational();
    else
        computeRational();
}

void tsym::NumPowerSimpl::computeNonRational()
/* This will always have a scalar as the result. The functionality of the Number class can thus
 * be used. */
{
    result.base = result.base.toThe(result.exp);
    result.base *= result.preFactor;

    result.preFactor = 1;
    result.exp = 1;
}

void tsym::NumPowerSimpl::computeRational()
{
    computeNegOrPosExp();
    adjustExpSignAndBase();
}

void tsym::NumPowerSimpl::computeNegOrPosExp()
{
    if (result.exp < 0)
        computeNegExp();
    else
        computePosExp();
}

void tsym::NumPowerSimpl::computeNegExp()
/* Shifts the handling to positive exponents by and pre- and postmodifying the components. */
{
    result.exp *= -1;
    result.preFactor = result.preFactor.toThe(-1);

    computePosExp();

    result.preFactor = result.preFactor.toThe(-1);

    if (result.base == 0)
        return;
    else if (result.exp == 1)
        result.base = result.base.toThe(-1);
    else
        result.exp *= -1;
}

void tsym::NumPowerSimpl::computePosExp()
{
    if (result.base < 0)
        shiftNegBase();

    computePosExpPosBase();
}

void tsym::NumPowerSimpl::shiftNegBase()
{
    Number extraction(-1);

    extraction = extraction.toThe(result.exp);

    result.base *= -1;
    result.preFactor *= extraction;
}

void tsym::NumPowerSimpl::computePosExpPosBase()
{
    if (result.preFactor < 0)
        shiftNegPreFac();

    computeAllPos();
}

void tsym::NumPowerSimpl::shiftNegPreFac()
{
    result.preFactor *= -1;

    isPreFacNegative = true;
}

void tsym::NumPowerSimpl::computeAllPos()
{
    if (areValuesSmallEnough())
        cancel();
    else if (isInt(result.exp))
        adjustExpGreaterThanOne();

    shiftPreFacSignBack();
}

bool tsym::NumPowerSimpl::areValuesSmallEnough() const
{
    if (abs(result.base.numerator()) > maxPrimeLimit || result.base.denominator() > maxPrimeLimit)
        return false;
    else if (abs(result.preFactor.numerator()) > maxPrimeLimit || result.preFactor.denominator() > maxPrimeLimit)
        return false;
    else
        return true;
}

void tsym::NumPowerSimpl::cancel()
/* Can be understood as the main method of this class, the changes made before prepared the
 * following operations. Here, all components (preFactor, base and exponent are not undefined,
 * but positive rational numbers. */
{
    defNewBasePrimes();
    defPreFacPrimesInPower();

    cancelAndExtract();
    collectPrimesInPower();

    primesToComponents();
    adjustExpGreaterThanOne();
}

void tsym::NumPowerSimpl::defNewBasePrimes()
{
    nbPrimes = PrimeFac(result.base);
}

void tsym::NumPowerSimpl::defPreFacPrimesInPower()
{
    const Number exp(1, result.exp.numerator());
    Number preFacInPower(result.preFactor);

    preFacInPower = preFacInPower.toThe(exp);

    /* If the power isn't resolvable, no prefactor prime factorization. */
    if (preFacInPower.isDouble())
        return;

    pfPrimes = PrimeFac(preFacInPower);
    pfPrimes.toThe(result.exp.denominator());

    result.preFactor = 1;
}

void tsym::NumPowerSimpl::cancelAndExtract()
{
    nbPrimes.multiply(pfPrimes);

    pfPrimes = nbPrimes.extract(result.exp);

    result.preFactor *= pfPrimes.eval();
}

void tsym::NumPowerSimpl::collectPrimesInPower()
{
    result.exp = nbPrimes.collectToNewExp(result.exp);
}

void tsym::NumPowerSimpl::primesToComponents()
{
    result.base = nbPrimes.eval();

    if (result.base == 1) {
        /* Adjust to the representation of simple numbers (exponent = 1, prefactor = 1). */
        assert(result.exp == 1);
        std::swap(result.base, result.preFactor);
    }
}

void tsym::NumPowerSimpl::adjustExpGreaterThanOne()
/* Performs a^b = prefac*a^d with b > 1 and d < 1. */
{
    Number baseExtraction(result.base);

    if (result.exp > 1 || result.exp < -1) {
        const Number expFloor(Int(result.exp.numerator() / result.exp.denominator()));
        baseExtraction = baseExtraction.toThe(expFloor);

        result.preFactor *= baseExtraction;
        result.exp -= expFloor;
    }
}

void tsym::NumPowerSimpl::adjustExpSignAndBase()
/* Turns a power of type (1/a)^exp into a^(-exp) or (a/b)^(-b/c) into (b/a)^(b/c). */
{
    if ((result.base.numerator() == 1 && result.exp != 1) || (result.exp < 0 && isFraction(result.base))) {
        result.base = result.base.toThe(-1);
        result.exp *= -1;
    }
}

void tsym::NumPowerSimpl::shiftPreFacSignBack()
{
    if (!isPreFacNegative)
        return;

    if (result.exp == 1)
        result.base *= -1;
    else
        result.preFactor *= -1;
}
