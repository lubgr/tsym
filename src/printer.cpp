
#include "printer.h"
#include <cassert>
#include "base.h"
#include "basefct.h"
#include "baseptrlistfct.h"
#include "numberfct.h"
#include "numeric.h"
#include "plaintextprintengine.h"
#include "power.h"
#include "printengine.h"
#include "product.h"

namespace tsym {
    namespace {
        enum class PowerAsFraction : bool { TRUE, FALSE };

        class Printer {
          public:
            Printer(PrintEngine& engine, PowerAsFraction opt)
                : engine(engine)
                , powerAsFraction(opt)
            {}

            void print(const BasePtr& base)
            {
                return print(*base);
            }

            void print(const Base& base)
            {
                if (isSymbol(base))
                    symbol(base);
                else if (isNumeric(base))
                    tsym::print(engine, *base.numericEval());
                else if (isPower(base))
                    power(base.base(), base.exp());
                else if (isSum(base))
                    sum(base);
                else if (isProduct(base))
                    product(base);
                else if (isFunction(base))
                    function(base);
                else if (isConstant(base))
                    engine.symbol(base.name());
                else {
                    assert(isUndefined(base));
                    engine.undefined();
                }
            }

          private:
            void symbol(const Base& base)
            {
                if (base.isPositive())
                    engine.positiveSymbol(base.name());
                else
                    engine.symbol(base.name());
            }

            void power(const BasePtr& base, const BasePtr& exp)
            {
                if (exp->isEqual(*Numeric::half())) {
                    engine.openSquareRoot();
                    print(base);
                    engine.closeSquareRoot();
                } else if (isNegativeNumeric(*exp) && powerAsFraction == PowerAsFraction::TRUE)
                    powerNegNumericExp(base, exp);
                else
                    standardPower(base, exp);
            }

            void powerNegNumericExp(const BasePtr& base, const BasePtr& exp)
            {
                const bool denomNeedsParentheses = isScalarPowerBase(base);

                engine.openNumerator().number(1).closeNumerator().openDenominator(denomNeedsParentheses);

                print(Power::create(base, Product::minus(exp)));

                engine.closeDenominator(denomNeedsParentheses);
            }

            bool isScalarPowerBase(const BasePtr& base)
            {
                const auto isPositiveInt = [](const auto& base) {
                    if (const auto n = base->numericEval())
                        return isInt(*n) && *n > 0;

                    return false;
                };

                return isSymbol(*base) || isConstant(*base) || isFunction(*base) || isPositiveInt(base);
            }

            void standardPower(const BasePtr& base, const BasePtr& exp)
            {
                powerBase(base);
                powerExponent(exp);
            }

            void powerBase(const BasePtr& base)
            {
                if (isScalarPowerBase(base))
                    print(base);
                else {
                    engine.openParentheses();
                    print(base);
                    engine.closeParentheses();
                }
            }

            void powerExponent(const BasePtr& exp)
            {
                if (isOne(*exp))
                    return;
                else if (isScalarPowerExp(exp)) {
                    engine.openScalarExponent();
                    print(exp);
                    engine.closeScalarExponent();
                } else {
                    engine.openCompositeExponent();
                    print(exp);
                    engine.closeCompositeExponent();
                }
            }

            bool isScalarPowerExp(const BasePtr& exp)
            {
                if (isSymbol(*exp) || isConstant(*exp) || isFunction(*exp))
                    return true;
                else if (isNumeric(*exp))
                    return isInt(*exp->numericEval()) && exp->isPositive();
                else
                    return false;
            }

            void sum(const Base& sum)
            {
                auto summands = sum.operands();

                print(summands.front());
                summands.pop_front();

                for (auto& summand : summands) {
                    if (isProductWithNegativeNumeric(summand)) {
                        engine.minusSign();
                        summand = Product::minus(summand);
                    } else
                        engine.plusSign();

                    print(summand);
                }
            }

            bool isProductWithNegativeNumeric(const BasePtr& summand)
            {
                if (isProduct(*summand)) {
                    const auto& first = summand->operands().front();

                    return isNumeric(*first) && first->isNegative();
                } else
                    return false;
            }

            void product(const Base& product)
            {
                BasePtrList factors(product.operands());

                if (powerAsFraction == PowerAsFraction::TRUE)
                    productAsFraction(factors);
                else
                    productWithoutFractions(factors);
            }

            void productAsFraction(BasePtrList& factors)
            {
                const unsigned productPrecedence = 2;
                auto frac = getProductFrac(factors);
                auto& num = frac.first;
                auto& denom = frac.second;

                if (num.empty())
                    engine.number(1);
                else if (num.size() == 1 && precedence(num.front()) < productPrecedence) {
                    engine.openParentheses();
                    print(num.front());
                    engine.closeParentheses();
                } else
                    productWithoutFractions(num);

                if (denom.empty())
                    return;

                engine.divisionSign();

                if (denom.size() == 1 && precedence(denom.front()) > productPrecedence)
                    print(denom.front());
                else {
                    engine.openParentheses();
                    print(Product::create(denom));
                    engine.closeParentheses();
                }
            }

            std::pair<tsym::BasePtrList, tsym::BasePtrList> getProductFrac(const BasePtrList& origFactors)
            {
                auto frac = std::pair<BasePtrList, BasePtrList>{};

                for (const auto& origFactor : origFactors) {
                    const auto& exp = origFactor->exp();

                    if (isNegativeNumeric(*exp)) {
                        frac.second.push_back(Power::create(origFactor->base(), Product::minus(exp)));
                    } else
                        frac.first.push_back(origFactor);
                }

                if (frac.first.empty() || frac.second.size() <= 1 || !isNumeric(*frac.first.front()))
                    return frac;

                /* Adjust the previous logic and move factors like 2/3 to numerator/denominator. */
                const auto fracFactor = frac.first.front()->numericEval();
                frac.first.pop_front();

                assert(fracFactor);

                frac.first.push_front(Numeric::create(fracFactor->numerator()));
                frac.second.push_front(Numeric::create(fracFactor->denominator()));

                return frac;
            }

            unsigned precedence(const BasePtr& ptr)
            {
                if (isSum(*ptr))
                    return 1;
                else if (isProduct(*ptr))
                    return 2;
                else if (isPower(*ptr))
                    return 3;
                else
                    return 4;
            }

            void productWithoutFractions(BasePtrList& factors)
            {
                const BasePtr first = factors.front();
                const unsigned productPrecedence = 2;

                factors.pop_front();

                if (factors.empty())
                    print(first);
                else if (isOne(*Product::minus(first)))
                    engine.unaryMinusSign();
                else if (isOne(*first))
                    ;
                else if (precedence(first) < productPrecedence) {
                    engine.openParentheses();
                    print(first);
                    engine.closeParentheses().timesSign();
                } else {
                    print(first);
                    engine.timesSign();
                }

                for (auto factor = cbegin(factors); factor != cend(factors); ++factor) {
                    if (precedence(*factor) < productPrecedence) {
                        engine.openParentheses();
                        print(*factor);
                        engine.closeParentheses();
                    } else
                        print(*factor);

                    if (factor != std::prev(cend(factors)))
                        engine.timesSign();
                }
            }

            void function(const Base& fct)
            {
                const auto& ops = fct.operands();

                engine.functionName(fct.name()).openParentheses();
                print(ops.front());

                if (ops.size() == 2) {
                    engine.comma();
                    print(ops.back());
                }

                engine.closeParentheses();
            }

            PrintEngine& engine;
            PowerAsFraction powerAsFraction;
        };

        void print(PrintEngine& engine, const Base& base, PowerAsFraction opt)
        {
            Printer p(engine, opt);
            p.print(base);
        }
    }
}

void tsym::print(PrintEngine& engine, const Number& number)
{
    if (number.isDouble())
        engine.number(number.toDouble());
    else if (isInt(number))
        engine.number(number.numerator());
    else {
        assert(isFraction(number));

        engine.openNumerator()
          .number(number.numerator())
          .closeNumerator()
          .openDenominator(true)
          .number(number.denominator())
          .closeDenominator(true);
    }
}

void tsym::print(PrintEngine& engine, const Base& base)
{
    print(engine, base, PowerAsFraction::TRUE);
}

void tsym::printDebug(PrintEngine& engine, const Base& base)
{
    print(engine, base, PowerAsFraction::FALSE);
}
