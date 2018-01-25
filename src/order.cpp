
#include "order.h"
#include "symbol.h"
#include "numeric.h"
#include "constant.h"
#include "function.h"
#include "number.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "logging.h"

namespace tsym {
    static bool doPermuteSameType(const BasePtr& left, const BasePtr& right);
    static bool doPermuteBothSymbol(const BasePtr& left, const BasePtr& right);
    static bool doPermuteBothNumeric(const BasePtr& left, const BasePtr& right);
    static bool doPermuteBothNumber(const Number& left, const Number& right);
    static bool doPermuteBothPower(const BasePtr& left, const BasePtr& right);
    static bool doPermuteBothProduct(const BasePtr& left, const BasePtr& right);
    static bool doPermuteListReverse(const BasePtrCtr& left, const BasePtrCtr& right);
    static bool doPermuteBothSum(const BasePtr& left, const BasePtr& right);
    static bool doPermuteBothConstant(const BasePtr& left, const BasePtr& right);
    static bool doPermuteBothFunction(const BasePtr& left, const BasePtr& right);
    static bool doPermuteDifferentType(const BasePtr& left, const BasePtr& right);
    static bool isPowerSumSymbolOrFunction(const BasePtr& ptr);
    static bool doPermuteLeftProduct(const BasePtr& left, const BasePtr& right);
    static bool isSumSymbolOrFunction(const BasePtr& ptr);
    static bool doPermuteLeftPower(const BasePtr& left, const BasePtr& right);
    static bool doPermuteLastElement(const BasePtrCtr& lList, const BasePtr& right);
    static bool isSymbolOrFunction(const BasePtr& ptr);
    static bool doPermuteLeftSum(const BasePtr& left, const BasePtr& right);
    static bool doPermuteLeftFunctionRightSymbol(const BasePtr& left, const BasePtr& right);
}

bool tsym::order::doPermute(const BasePtr& left, const BasePtr& right)
{
    if (left->sameType(right))
        return doPermuteSameType(left, right);
    else
        return doPermuteDifferentType(left, right);
}

bool tsym::doPermuteSameType(const BasePtr& left, const BasePtr& right)
{
    if (left->isSymbol())
        return doPermuteBothSymbol(left, right);
    else if (left->isNumeric())
        return doPermuteBothNumeric(left, right);
    else if (left->isPower())
        return doPermuteBothPower(left, right);
    else if (left->isProduct())
        return doPermuteBothProduct(left, right);
    else if (left->isSum())
        return doPermuteBothSum(left, right);
    else if (left->isConstant())
        return doPermuteBothConstant(left, right);
    else if (left->isFunction())
        return doPermuteBothFunction(left, right);
    else if (left->isUndefined())
        TSYM_WARNING("Requesting order relation for an Undefined!");

    return false;
}

bool tsym::doPermuteBothSymbol(const BasePtr& left, const BasePtr& right)
{
    const Name& lName(left->name());
    const Name& rName(right->name());

    if (lName == rName)
        if (left->isPositive() != right->isPositive())
            /* For equal names and different signs, put the pos. symbol first: */
            return right->isPositive();

    return lName > rName;
}

bool tsym::doPermuteBothNumeric(const BasePtr& left, const BasePtr& right)
{
    const Number nLeft(left->numericEval());
    const Number nRight(right->numericEval());

    return doPermuteBothNumber(nLeft, nRight);
}

bool tsym::doPermuteBothNumber(const Number& left, const Number& right)
{
    return left > right;
}

bool tsym::doPermuteBothPower(const BasePtr& left, const BasePtr& right)
{
    const BasePtr lBase(left->base());
    const BasePtr rBase(right->base());
    const BasePtr lExp(left->exp());
    const BasePtr rExp(right->exp());

    if (lBase->isDifferent(rBase))
        return order::doPermute(lBase, rBase);
    else
        return order::doPermute(lExp, rExp);
}

bool tsym::doPermuteBothProduct(const BasePtr& left, const BasePtr& right)
{
    const BasePtrCtr& lFactors(left->operands());
    const BasePtrCtr& rFactors(right->operands());

    return doPermuteListReverse(lFactors, rFactors);
}

bool tsym::doPermuteListReverse(const BasePtrCtr& left, const BasePtrCtr& right)
{
    auto lIt(left.rbegin());
    auto rIt(right.rbegin());

    for (; lIt != left.rend() && rIt != right.rend(); ++lIt, ++rIt)
        if ((*lIt)->isDifferent(*rIt))
            return order::doPermute(*lIt, *rIt);

    if (lIt == left.rend() && rIt != right.rend())
        return false;
    else if (rIt == right.rend() && lIt != left.rend())
        return true;
    else
        return left.size() < right.size();
}

bool tsym::doPermuteBothSum(const BasePtr& left, const BasePtr& right)
{
    const BasePtrCtr& lSummands(left->operands());
    const BasePtrCtr& rSummands(right->operands());

    return doPermuteListReverse(lSummands, rSummands);
}

bool tsym::doPermuteBothConstant(const BasePtr& left, const BasePtr& right)
{
    const Name& lName(left->name());
    const Name& rName(right->name());

    return lName > rName;
}

bool tsym::doPermuteBothFunction(const BasePtr& left, const BasePtr& right)
{
    const Name& lName(left->name());
    const Name& rName(right->name());

    if (lName != rName)
        return lName > rName;
    else
        /* If the function argument is a sum or a product, this differs from Cohen's algorithm: the
         * operands are compared by taking the last different operand as significant, as it is the
         * case for comparing two sums or two products. Cohen proposes to handle sum/product
         * arguments exactly the other way around, while the advantage over using the normal
         * ordering procedure isn't obvious. Thus, we stick to the standard procedure, leading to
         * e.g. the correct ordering sin(b + c + d)*sin(a + c + e). */
        return order::doPermute(left->operands().front(), right->operands().front());
}

bool tsym::doPermuteDifferentType(const BasePtr& left, const BasePtr& right)
{
    if (left->isNumeric())
        return false;
    /* We differ from Cohen's algorithm here, as he didn't take a Constant type into account. It is
     * simply the leftmost part in any expression, except in comparison with a Numeric. */
    else if (left->isConstant() && !right->isNumeric())
        return false;
    else if (left->isProduct() && isPowerSumSymbolOrFunction(right))
        return doPermuteLeftProduct(left, right);
    else if (left->isPower() && isSumSymbolOrFunction(right))
        return doPermuteLeftPower(left, right);
    else if (left->isSum() && isSymbolOrFunction(right))
        return doPermuteLeftSum(left, right);
    else if (left->isFunction() && right->isSymbol())
        return doPermuteLeftFunctionRightSymbol(left, right);

    if (left->isUndefined() || right->isUndefined()) {
        TSYM_WARNING("Requesting order relation for Undefined base pointer!");
        return false;
    }

    return !order::doPermute(right, left);
}

bool tsym::isPowerSumSymbolOrFunction(const BasePtr& ptr)
{
    return ptr->isPower() || ptr->isSum() || ptr->isSymbol() || ptr->isFunction();
}

bool tsym::doPermuteLeftProduct(const BasePtr& left, const BasePtr& right)
{
    const BasePtrCtr& lList(left->operands());

    return doPermuteLastElement(lList, right);
}

bool tsym::doPermuteLastElement(const BasePtrCtr& lList, const BasePtr& right)
{
    const BasePtr lLastFactor(lList.back());

    if (lLastFactor->isEqual(right))
        return true;
    else
        return order::doPermute(lLastFactor, right);
}

bool tsym::isSumSymbolOrFunction(const BasePtr& ptr)
{
    return ptr->isSum() || ptr->isSymbol() || ptr->isFunction();
}

bool tsym::doPermuteLeftPower(const BasePtr& left, const BasePtr& right)
{
    const BasePtr lBase(left->base());
    const BasePtr lExp(left->exp());

    if (lBase->isDifferent(right))
        return order::doPermute(lBase, right);
    else
        return order::doPermute(lExp, Numeric::one());
}

bool tsym::isSymbolOrFunction(const BasePtr& ptr)
{
    return ptr->isSymbol() || ptr->isFunction();
}

bool tsym::doPermuteLeftSum(const BasePtr& left, const BasePtr& right)
{
    const BasePtrCtr& lList(left->operands());

    return doPermuteLastElement(lList, right);
}

bool tsym::doPermuteLeftFunctionRightSymbol(const BasePtr& left, const BasePtr& right)
{
    const Name& lName(left->name());
    const Name& rName(right->name());

    if (lName == rName)
        return false;
    else
        return lName > rName;
}

bool tsym::order::isCorrect(const BasePtr& left, const BasePtr& right)
{
    return !doPermute(left, right);
}
