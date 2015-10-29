
#include <string>
#include <cstring>
#include <limits>
#include <vector>
#include "parseradapter.h"
#include "sum.h"
#include "symbol.h"
#include "numeric.h"
#include "product.h"
#include "undefined.h"
#include "power.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "logging.h"

/* These functions are defined in the (f)lex file scanner.l, see comment over there for details. */
extern "C" void *parseString(const char *string);
extern "C" unsigned getErrorColumn();

using namespace tsym;

namespace {
    void *toVoid(const BasePtr& basePtr)
    {
        return reinterpret_cast<void*>(new BasePtr(basePtr));
    }

    const BasePtr *fromVoid(const void *basePtr)
    {
        return reinterpret_cast<const BasePtr*>(basePtr);
    }

    BasePtr castNonZeroParseResult(const void *result)
    {
        const BasePtr *castedResult(fromVoid(result));
        const BasePtr copy(*castedResult);

        delete castedResult;

        return copy;
    }

    void *create(BasePtr (*fct)(const BasePtr& operand), const void *operand)
    {
        const BasePtr *castedOperand(fromVoid(operand));
        const BasePtr creation(fct(*castedOperand));

        return toVoid(creation);
    }

    void *create(BasePtr (*fct)(const BasePtr& operand1, const BasePtr& operand2),
            const void *operand1, const void *operand2)
    {
        const BasePtr *castedOperand1(fromVoid(operand1));
        const BasePtr *castedOperand2(fromVoid(operand2));
        const BasePtr creation(fct(*castedOperand1, *castedOperand2));

        return toVoid(creation);
    }

    std::vector<std::string>& errors()
    {
        static std::vector<std::string> errorVec;

        return errorVec;
    }

    void registerError(const std::string& message)
    {
        logging::error() << message;

        errors().push_back(message);
    }
}

tsym::BasePtr tsym::parserAdapter::parse(const char *string)
{
    const void *parseResult = parseString(string);

    if (parseResult == NULL) {
        logging::warning() << "Parsing \'" << string << "\' resulted in NULL pointer";
        return Undefined::create();
    } else
        return castNonZeroParseResult(parseResult);
}

void tsym::parserAdapter::clearErrors()
{
    /* Equivalent to call .clear() on the vector of error messages, but CppuTest then complains
     * about a memory leak. This makes sure, that allocated data inside of the vector object is
     * acutually deleted. */
    std::vector<std::string>().swap(errors());
}

const std::vector<std::string>& tsym::parserAdapter::getErrors()
{
    return errors();
}

unsigned tsym::parserAdapter::getFirstErrorIndex()
{
    if (errors().empty())
        return 0;
    else
        return getErrorColumn() - 1;
}

void *tsym_parserAdapter_createInteger(long value)
{
    const BasePtr numeric(Numeric::create(Int(value)));

    return toVoid(numeric);
}

void *tsym_parserAdapter_createMaxInt(const char *errorMessage)
{
    const BasePtr maxInt(Numeric::create(Int::max()));

    logging::error() << errorMessage << maxInt;

    return toVoid(maxInt);
}

void *tsym_parserAdapter_createDouble(double value)
{
    const BasePtr numeric(Numeric::create(value));

    return toVoid(numeric);
}

void *tsym_parserAdapter_createMaxDouble(const char *errorMessage)
{
    const BasePtr maxDouble(Numeric::create(std::numeric_limits<double>::max()));

    logging::error() << errorMessage << maxDouble;

    return toVoid(maxDouble);
}

void *tsym_parserAdapter_createSymbol(const char *name)
{
    const BasePtr s(Symbol::create(std::string(name)));

    return toVoid(s);
}

void *tsym_parserAdapter_createUndefined()
{
    const BasePtr undefined(Undefined::create());

    return toVoid(undefined);
}

void *tsym_parserAdapter_createSum(const void *summand1, const void *summand2)
{
    return create(Sum::create, summand1, summand2);
}

void *tsym_parserAdapter_createDifference(const void *summand1, const void *summand2)
{
    void *negativeSummand2(create(Product::minus, summand2));
    void *result(create(Sum::create, summand1, negativeSummand2));

    tsym_parserAdapter_deletePtr(negativeSummand2);

    return result;
}

void *tsym_parserAdapter_createMinus(const void *arg)
{
    return create(Product::minus, arg);
}

void *tsym_parserAdapter_createProduct(const void *factor1, const void *factor2)
{
    return create(Product::create, factor1, factor2);
}

void *tsym_parserAdapter_createQuotient(const void *dividend, const void *divisor)
{
    void *divisorAsFactor(create(Power::oneOver, divisor));
    void *result(create(Product::create, dividend, divisorAsFactor));

    tsym_parserAdapter_deletePtr(divisorAsFactor);

    return result;
}

void *tsym_parserAdapter_createPower(const void *base, const void *exponent)
{
    return create(Power::create, base, exponent);
}

void *tsym_parserAdapter_createSine(const void *arg)
{
    return create(Trigonometric::createSin, arg);
}

void *tsym_parserAdapter_createCosine(const void *arg)
{
    return create(Trigonometric::createCos, arg);
}

void *tsym_parserAdapter_createTangent(const void *arg)
{
    return create(Trigonometric::createTan, arg);
}

void *tsym_parserAdapter_createAsine(const void *arg)
{
    return create(Trigonometric::createAsin, arg);
}

void *tsym_parserAdapter_createAcosine(const void *arg)
{
    return create(Trigonometric::createAcos, arg);
}

void *tsym_parserAdapter_createAtangent(const void *arg)
{
    return create(Trigonometric::createAtan, arg);
}

void *tsym_parserAdapter_createLogarithm(const void *arg)
{
    return create(Logarithm::create, arg);
}

void *tsym_parserAdapter_createSquareRoot(const void *arg)
{
    return create(Power::sqrt, arg);
}

void tsym_parserAdapter_deletePtrs(void *ptr1, void *ptr2)
{
    tsym_parserAdapter_deletePtr(ptr1);
    tsym_parserAdapter_deletePtr(ptr2);
}

void tsym_parserAdapter_deletePtr(void *ptr)
{
    BasePtr *casted(reinterpret_cast<BasePtr*>(ptr));

    delete casted;

    ptr = NULL;
}

void tsym_parserAdapter_logParsingError(const char *message, char *yytext)
{
    std::string errorMessage(message);

    errorMessage.append(yytext);

    registerError(errorMessage);
}
