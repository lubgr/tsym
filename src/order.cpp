
#include "order.h"
#include <boost/range/adaptor/indirected.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm/lexicographical_compare.hpp>
#include "constant.h"
#include "function.h"
#include "logging.h"
#include "number.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "sum.h"
#include "symbol.h"

namespace tsym {
    static bool doPermuteSameType(const Base& left, const Base& right);
    static bool doPermuteBothSymbol(const Base& left, const Base& right);
    static bool doPermuteBothNumeric(const Base& left, const Base& right);
    static bool doPermuteBothNumber(const Number& left, const Number& right);
    static bool doPermuteBothPower(const Base& left, const Base& right);
    static bool doPermuteBothProduct(const Base& left, const Base& right);
    static bool doPermuteListReverse(const BasePtrList& left, const BasePtrList& right);
    static bool doPermuteBothSum(const Base& left, const Base& right);
    static bool doPermuteBothConstant(const Base& left, const Base& right);
    static bool doPermuteBothFunction(const Base& left, const Base& right);
    static bool doPermuteDifferentType(const Base& left, const Base& right);
    static bool isPowerSumSymbolOrFunction(const Base& arg);
    static bool doPermuteLeftProduct(const Base& left, const Base& right);
    static bool isSumSymbolOrFunction(const Base& arg);
    static bool doPermuteLeftPower(const Base& left, const Base& right);
    static bool doPermuteLastElement(const BasePtrList& lList, const Base& right);
    static bool isSymbolOrFunction(const Base& arg);
    static bool doPermuteLeftSum(const Base& left, const Base& right);
    static bool doPermuteLeftFunctionRightSymbol(const Base& left, const Base& right);
}

bool tsym::doPermute(const Base& left, const Base& right)
{
    if (left.sameType(right))
        return doPermuteSameType(left, right);
    else
        return doPermuteDifferentType(left, right);
}

bool tsym::doPermuteSameType(const Base& left, const Base& right)
{
    if (left.isSymbol())
        return doPermuteBothSymbol(left, right);
    else if (left.isNumeric())
        return doPermuteBothNumeric(left, right);
    else if (left.isPower())
        return doPermuteBothPower(left, right);
    else if (left.isProduct())
        return doPermuteBothProduct(left, right);
    else if (left.isSum())
        return doPermuteBothSum(left, right);
    else if (left.isConstant())
        return doPermuteBothConstant(left, right);
    else if (left.isFunction())
        return doPermuteBothFunction(left, right);
    else if (left.isUndefined())
        TSYM_WARNING("Requesting order relation for an Undefined!");

    return false;
}

bool tsym::doPermuteBothSymbol(const Base& left, const Base& right)
{
    const Name& lName{left.name()};
    const Name& rName{right.name()};

    if (lName == rName)
        if (left.isPositive() != right.isPositive())
            /* For equal names and different signs, put the pos. symbol first: */
            return right.isPositive();

    return lName > rName;
}

bool tsym::doPermuteBothNumeric(const Base& left, const Base& right)
{
    const auto nLeft(left.numericEval());
    const auto nRight(right.numericEval());

    assert(nLeft && nRight);

    return doPermuteBothNumber(*nLeft, *nRight);
}

bool tsym::doPermuteBothNumber(const Number& left, const Number& right)
{
    return left > right;
}

bool tsym::doPermuteBothPower(const Base& left, const Base& right)
{
    const Base& lBase(*left.base());
    const Base& rBase(*right.base());
    const Base& lExp(*left.exp());
    const Base& rExp(*right.exp());

    if (lBase.isDifferent(rBase))
        return doPermute(lBase, rBase);
    else
        return doPermute(lExp, rExp);
}

bool tsym::doPermuteBothProduct(const Base& left, const Base& right)
{
    const BasePtrList& lFactors(left.operands());
    const BasePtrList& rFactors(right.operands());

    return doPermuteListReverse(lFactors, rFactors);
}

bool tsym::doPermuteListReverse(const BasePtrList& left, const BasePtrList& right)
{
    using boost::adaptors::indirected;
    using boost::adaptors::reversed;

    return boost::lexicographical_compare(right | reversed | indirected, left | reversed | indirected,
      [](const auto& bp1, const auto& bp2) { return bp1.isDifferent(bp2) && isCorrect(bp1, bp2); });
}

bool tsym::doPermuteBothSum(const Base& left, const Base& right)
{
    const BasePtrList& lSummands(left.operands());
    const BasePtrList& rSummands(right.operands());

    return doPermuteListReverse(lSummands, rSummands);
}

bool tsym::doPermuteBothConstant(const Base& left, const Base& right)
{
    const Name& lName{left.name()};
    const Name& rName{right.name()};

    return lName > rName;
}

bool tsym::doPermuteBothFunction(const Base& left, const Base& right)
{
    const Name& lName{left.name()};
    const Name& rName{right.name()};

    if (lName != rName)
        return lName > rName;
    else
        /* If the function argument is a sum or a product, this differs from Cohen's algorithm: the
         * operands are compared by taking the last different operand as significant, as it is the
         * case for comparing two sums or two products. Cohen proposes to handle sum/product
         * arguments exactly the other way around, while the advantage over using the normal
         * ordering procedure isn't obvious. Thus, we stick to the standard procedure, leading to
         * e.g. the correct ordering sin(b + c + d)*sin(a + c + e). */
        return doPermute(*left.operands().front(), *right.operands().front());
}

bool tsym::doPermuteDifferentType(const Base& left, const Base& right)
{
    if (left.isNumeric())
        return false;
    /* We differ from Cohen's algorithm here, as he didn't take a Constant type into account. It is
     * simply the leftmost part in any expression, except in comparison with a Numeric. */
    else if (left.isConstant() && !right.isNumeric())
        return false;
    else if (left.isProduct() && isPowerSumSymbolOrFunction(right))
        return doPermuteLeftProduct(left, right);
    else if (left.isPower() && isSumSymbolOrFunction(right))
        return doPermuteLeftPower(left, right);
    else if (left.isSum() && isSymbolOrFunction(right))
        return doPermuteLeftSum(left, right);
    else if (left.isFunction() && right.isSymbol())
        return doPermuteLeftFunctionRightSymbol(left, right);

    if (left.isUndefined() || right.isUndefined()) {
        TSYM_WARNING("Requesting order relation for Undefined base pointer!");
        return false;
    }

    return !doPermute(right, left);
}

bool tsym::isPowerSumSymbolOrFunction(const Base& arg)
{
    return arg.isPower() || arg.isSum() || arg.isSymbol() || arg.isFunction();
}

bool tsym::doPermuteLeftProduct(const Base& left, const Base& right)
{
    const BasePtrList& lList(left.operands());

    return doPermuteLastElement(lList, right);
}

bool tsym::doPermuteLastElement(const BasePtrList& lList, const Base& right)
{
    const Base& lLastFactor(*lList.back());

    if (lLastFactor.isEqual(right))
        return true;
    else
        return doPermute(lLastFactor, right);
}

bool tsym::isSumSymbolOrFunction(const Base& arg)
{
    return arg.isSum() || arg.isSymbol() || arg.isFunction();
}

bool tsym::doPermuteLeftPower(const Base& left, const Base& right)
{
    const Base& lBase(*left.base());
    const Base& lExp(*left.exp());

    if (lBase.isDifferent(right))
        return doPermute(lBase, right);
    else
        return doPermute(lExp, *Numeric::one());
}

bool tsym::isSymbolOrFunction(const Base& arg)
{
    return arg.isSymbol() || arg.isFunction();
}

bool tsym::doPermuteLeftSum(const Base& left, const Base& right)
{
    const BasePtrList& lList(left.operands());

    return doPermuteLastElement(lList, right);
}

bool tsym::doPermuteLeftFunctionRightSymbol(const Base& left, const Base& right)
{
    const Name& lName{left.name()};
    const Name& rName{right.name()};

    if (lName == rName)
        return false;
    else
        return lName > rName;
}

bool tsym::isCorrect(const Base& left, const Base& right)
{
    return !doPermute(left, right);
}
