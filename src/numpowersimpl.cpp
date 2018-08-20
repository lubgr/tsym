
#include "numpowersimpl.h"
#include <cassert>
#include <cmath>
#include "logging.h"
#include "numberfct.h"

void tsym::NumPowerSimpl::setPower(const Number& base, const Number& exp)
{
    origBase = base;
    origExp = exp;
    needsComputation = true;
}

void tsym::NumPowerSimpl::setPreFac(const Number& fac)
{
    origPreFac = fac;

    needsComputation = true;
}

void tsym::NumPowerSimpl::setMaxPrimeResolution(Int limit)
{
    maxPrimeLimit = std::move(limit);
}

bool tsym::NumPowerSimpl::isInputValid()
{
    return !(newBase < 0 && !isInt(newExp));
}

const tsym::Number& tsym::NumPowerSimpl::getNewBase()
{
    return get(newBase);
}

const tsym::Number& tsym::NumPowerSimpl::getNewExp()
{
    return get(newExp);
}

const tsym::Number& tsym::NumPowerSimpl::getPreFactor()
{
    return get(preFac);
}

const tsym::Number& tsym::NumPowerSimpl::get(const Number& component)
{
    if (needsComputation)
        computeAndSetFlag();

    return component;
}

void tsym::NumPowerSimpl::computeAndSetFlag()
{
    compute();

    needsComputation = false;
}

void tsym::NumPowerSimpl::compute()
{
    initFromOrig();

    if (!isInputValid())
        TSYM_ERROR("Illegal numeric power with base: %S and exponent %S", newBase, newExp);
    else if (newBase.isDouble() || newExp.isDouble())
        computeNonRational();
    else
        computeRational();
}

void tsym::NumPowerSimpl::initFromOrig()
{
    newBase = origBase;
    newExp = origExp;
    preFac = origPreFac;
    isPreFacNegative = false;
}

void tsym::NumPowerSimpl::computeNonRational()
/* This will always have a scalar as the result. The functionality of the Number class can thus
 * be used. */
{
    newBase = newBase.toThe(newExp);
    newBase *= preFac;

    preFac = 1;
    newExp = 1;
}

void tsym::NumPowerSimpl::computeRational()
{
    computeNegOrPosExp();
    adjustExpSignAndBase();
}

void tsym::NumPowerSimpl::computeNegOrPosExp()
{
    if (newExp < 0)
        computeNegExp();
    else
        computePosExp();
}

void tsym::NumPowerSimpl::computeNegExp()
/* Shifts the handling to positive exponents by and pre- and postmodifying the components. */
{
    newExp *= -1;
    preFac = preFac.toThe(-1);

    computePosExp();

    preFac = preFac.toThe(-1);

    if (newBase == 0)
        return;
    else if (newExp == 1)
        newBase = newBase.toThe(-1);
    else
        newExp *= -1;
}

void tsym::NumPowerSimpl::computePosExp()
{
    if (newBase < 0)
        shiftNegBase();

    computePosExpPosBase();
}

void tsym::NumPowerSimpl::shiftNegBase()
{
    Number extraction(-1);

    extraction = extraction.toThe(newExp);

    newBase *= -1;
    preFac *= extraction;
}

void tsym::NumPowerSimpl::computePosExpPosBase()
{
    if (preFac < 0)
        shiftNegPreFac();

    computeAllPos();
}

void tsym::NumPowerSimpl::shiftNegPreFac()
{
    preFac *= -1;

    isPreFacNegative = true;
}

void tsym::NumPowerSimpl::computeAllPos()
{
    if (areValuesSmallEnough())
        cancel();
    else if (isInt(newExp))
        adjustExpGreaterThanOne();

    shiftPreFacSignBack();
}

bool tsym::NumPowerSimpl::areValuesSmallEnough() const
{
    if (integer::abs(newBase.numerator()) > maxPrimeLimit || newBase.denominator() > maxPrimeLimit)
        return false;
    else if (integer::abs(preFac.numerator()) > maxPrimeLimit || preFac.denominator() > maxPrimeLimit)
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
    nbPrimes = PrimeFac(newBase);
}

void tsym::NumPowerSimpl::defPreFacPrimesInPower()
{
    const Number exp(1, newExp.numerator());
    Number preFacInPower(preFac);

    preFacInPower = preFacInPower.toThe(exp);

    /* If the power isn't resolvable, no prefactor prime factorization. */
    if (preFacInPower.isDouble())
        return;

    pfPrimes = PrimeFac(preFacInPower);
    pfPrimes.toThe(newExp.denominator());

    preFac = 1;
}

void tsym::NumPowerSimpl::cancelAndExtract()
{
    nbPrimes.multiply(pfPrimes);

    pfPrimes = nbPrimes.extract(newExp);

    preFac *= pfPrimes.eval();
}

void tsym::NumPowerSimpl::collectPrimesInPower()
{
    newExp = nbPrimes.collectToNewExp(newExp);
}

void tsym::NumPowerSimpl::primesToComponents()
{
    newBase = nbPrimes.eval();

    if (newBase == 1) {
        /* Adjust to the representation of simple numbers (exponent = 1, prefactor = 1). */
        assert(newExp == 1);
        std::swap(newBase, preFac);
    }
}

void tsym::NumPowerSimpl::adjustExpGreaterThanOne()
/* Performs a^b = prefac*a^d with b > 1 and d < 1. */
{
    Number baseExtraction(newBase);

    if (newExp > 1 || newExp < -1) {
        const Number expFloor(Int(newExp.numerator() / newExp.denominator()));
        baseExtraction = baseExtraction.toThe(expFloor);

        preFac *= baseExtraction;
        newExp -= expFloor;
    }
}

void tsym::NumPowerSimpl::adjustExpSignAndBase()
/* Turns a power of type (1/a)^exp into a^(-exp) or (a/b)^(-b/c) into (b/a)^(b/c). */
{
    if ((newBase.numerator() == 1 && newExp != 1) || (newExp < 0 && isFraction(newBase))) {
        newBase = newBase.toThe(-1);
        newExp *= -1;
    }
}

void tsym::NumPowerSimpl::shiftPreFacSignBack()
{
    if (!isPreFacNegative)
        return;

    if (newExp == 1)
        newBase *= -1;
    else
        preFac *= -1;
}
