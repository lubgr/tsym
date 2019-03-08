
#include "constants.h"
#include "constant.h"
#include "var.h"

const tsym::Var& tsym::pi()
{
    static const tsym::Var instance(Constant::createPi());

    return instance;
}

const tsym::Var& tsym::euler()
{
    static const tsym::Var instance(Constant::createE());

    return instance;
}
