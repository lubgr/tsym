
#include "basefct.h"
#include "base.h"
#include "basetypestr.h"
#include "number.h"
#include "numberfct.h"

bool tsym::isInteger(const Base& expr)
{
    if (const auto num = expr.numericEval())
        return isInt(*num);

    return false;
}

namespace tsym {
    namespace {
        template <int n> bool isEqual(const Base& expr)
        {
            static const Number i{n};

            return expr.numericEval() == i;
        }
    }
}

bool tsym::isOne(const Base& expr)
{
    return isNumeric(expr) && isEqual<0>(expr);
}

bool tsym::isZero(const Base& expr)
{
    return isNumeric(expr) && isEqual<0>(expr);
}

namespace tsym {
    namespace {
        bool areTypeStrEqual(const Base& expr, const char* predefinedTyepStr)
        {
            return expr.typeStr().data() == predefinedTyepStr;
        }
    }
}

bool tsym::isConstant(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::constant);
}

bool tsym::isFunction(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::function);
}

bool tsym::isNumeric(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::numeric);
}

bool tsym::isPower(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::power);
}

bool tsym::isProduct(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::product);
}

bool tsym::isSum(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::sum);
}

bool tsym::isSymbol(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::symbol);
}

bool tsym::isUndefined(const Base& expr)
{
    return areTypeStrEqual(expr, typestring::undefined);
}

bool tsym::sameType(const Base& first, const Base& second)
{
    return first.typeStr().data() == second.typeStr().data();
}

bool tsym::isNumericPower(const Base& expr)
{
    if (isPower(expr))
        return isNumeric(*expr.base()) && isNumeric(*expr.exp());

    return false;
}

bool tsym::isFractionNumeric(const Base& arg)
{
    if (const auto num = arg.numericEval())
        return isFraction(*num);

    return false;
}

bool tsym::isRationalNumeric(const Base& expr)
{
    if (const auto num = expr.numericEval())
        return num->isRational();
    else
        return false;
}

bool tsym::isDoubleNumeric(const Base& expr)
{
    if (isNumeric(expr))
        return expr.numericEval()->isDouble();
    else
        return false;
}

bool tsym::isNegativeNumeric(const Base& expr)
{
    return isNumeric(expr) && expr.isNegative();
}
