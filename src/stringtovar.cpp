
#include "stringtovar.h"
#include "parseradapter.h"

tsym::StringToVar::StringToVar(const std::string& source) :
    source(source),
    errorIndex(0)
{
    parse();
}

void tsym::StringToVar::parse()
{
    result = Var(parserAdapter::parse(source.c_str()));

    errors = parserAdapter::getErrors();
    errorIndex = parserAdapter::getFirstErrorIndex();

    parserAdapter::clearErrors();
}

bool tsym::StringToVar::success() const
{
    return errors.empty();
}

const std::vector<std::string>& tsym::StringToVar::errorMessages() const
{
    return errors;
}

unsigned tsym::StringToVar::firstErrorIndex() const
{
    return errorIndex;
}

const tsym::Var& tsym::StringToVar::get() const
{
    return result;
}
