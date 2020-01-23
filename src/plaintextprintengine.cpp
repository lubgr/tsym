
#include "plaintextprintengine.h"
#include "int.h"
#include "name.h"
#include "namefct.h"

tsym::PlaintextPrintEngine::PlaintextPrintEngine(std::ostream& out, CharSet charset)
    : out(out)
    , charset(charset)
{}

tsym::PrintEngine& tsym::PlaintextPrintEngine::symbol(
  std::string_view name, std::string_view sub, std::string_view super)
{
    if (charset == CharSet::ASCII)
        out << concat({name, sub, super});
    else
        out << unicode({name, sub, super});

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::positiveSymbol(
  std::string_view name, std::string_view sub, std::string_view super)
{
    symbol(name, sub, super);

#ifndef TSYM_ASCII_ONLY
    if (charset == CharSet::UNICODE)
        out << "\u208A";
#endif

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::functionName(std::string_view name)
{
    symbol(name, {}, {});

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::floatingPoint(double n)
{
    out << n;

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::integer(long long n)
{
    out << n;

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::integer(std::string_view n)
{
    out << n;

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::undefined()
{
    out << "Undefined";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::plusSign()
{
    out << " + ";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::minusSign()
{
    out << " - ";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::unaryMinusSign()
{
    out << "-";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::timesSign()
{
    out << "*";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::divisionSign()
{
    out << "/";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::comma()
{
    out << ", ";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::openNumerator(bool numeratorIsSum)
{
    if (numeratorIsSum)
        openParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::closeNumerator(bool numeratorWasSum)
{
    if (numeratorWasSum)
        closeParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::openDenominator(bool denominatorIsScalar)
{
    divisionSign();

    if (!denominatorIsScalar)
        openParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::closeDenominator(bool denominatorWasScalar)
{
    if (!denominatorWasScalar)
        closeParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::openScalarExponent()
{
    out << "^";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::closeScalarExponent()
{
    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::openCompositeExponent()
{
    openScalarExponent();
    openParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::closeCompositeExponent()
{
    closeParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::openSquareRoot()
{
    out << "sqrt";
    openParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::closeSquareRoot()
{
    closeParentheses();

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::openParentheses()
{
    out << "(";

    return *this;
}

tsym::PrintEngine& tsym::PlaintextPrintEngine::closeParentheses()
{
    out << ")";

    return *this;
}
