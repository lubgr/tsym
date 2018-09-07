
#include "plaintextprintengine.h"
#include "logging.h"
#include "name.h"
#include "namefct.h"

tsym::PlaintextPrintEngine::PlaintextPrintEngine(std::ostream& out, CharSet charset)
    : PrintEngine<PlaintextPrintEngine>(out)
    , charset(charset)
{}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::symbol(const Name& name)
{
    if (charset == CharSet::ASCII)
        out << concat(name);
    else
        out << unicode(name);

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::positiveSymbol(const Name& name)
{
    symbol(name);

#ifndef TSYM_ASCII_ONLY
    if (charset == CharSet::UNICODE)
        out << "\u208A";
#endif

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::functionName(const Name& name)
{
    symbol(name);

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::number(double n)
{
    out << n;

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::number(const Int& n)
{
    out << n;

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::undefined()
{
    out << "Undefined";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::plusSign()
{
    out << " + ";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::minusSign()
{
    out << " - ";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::unaryMinusSign()
{
    out << "-";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::timesSign()
{
    out << "*";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::divisionSign()
{
    out << "/";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::comma()
{
    out << ", ";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::openNumerator(bool numeratorIsSum)
{
    if (numeratorIsSum)
        openParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::closeNumerator(bool numeratorWasSum)
{
    if (numeratorWasSum)
        closeParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::openDenominator(bool denominatorIsScalar)
{
    divisionSign();

    if (!denominatorIsScalar)
        openParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::closeDenominator(bool denominatorWasScalar)
{
    if (!denominatorWasScalar)
        closeParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::openScalarExponent()
{
    out << "^";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::closeScalarExponent()
{
    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::openCompositeExponent()
{
    openScalarExponent();
    openParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::closeCompositeExponent()
{
    closeParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::openSquareRoot()
{
    out << "sqrt";
    openParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::closeSquareRoot()
{
    closeParentheses();

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::openParentheses()
{
    out << "(";

    return *this;
}

tsym::PlaintextPrintEngine& tsym::PlaintextPrintEngine::closeParentheses()
{
    out << ")";

    return *this;
}
