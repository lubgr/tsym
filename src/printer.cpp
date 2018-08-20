
#include "printer.h"
#include <cassert>
#include "base.h"
#include "bplist.h"
#include "numberfct.h"
#include "numeric.h"
#include "plaintextprintengine.h"
#include "power.h"
#include "product.h"

namespace tsym {
    namespace {
        enum class PowerAsFraction : bool { TRUE, FALSE };

        template <class Engine> class Printer {
          public:
            Printer(Engine& engine, PowerAsFraction opt)
                : engine(engine)
                , powerAsFraction(opt)
            {}

            void print(const Base& base)
            {
                engine.registerToplevelPrintFunction([this](Engine&, const BasePtr& ptr) { print(*ptr); });

                if (base.isSymbol())
                    symbol(base);
                else if (base.isNumeric())
                    printer::print(engine, base.numericEval());
                else if (base.isPower())
                    power(base.base(), base.exp());
                else if (base.isSum())
                    sum(base);
                else if (base.isProduct())
                    product(base);
                else if (base.isFunction())
                    function(base);
                else if (base.isConstant())
                    engine.symbol(base.name());
                else {
                    assert(base.isUndefined());
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
                if (exp->isEqual(*Numeric::half()))
                    engine.openSquareRoot().invokePrint(base).closeSquareRoot();
                else if (isNegativeNumeric(exp) && powerAsFraction == PowerAsFraction::TRUE)
                    powerNegNumericExp(base, exp);
                else
                    standardPower(base, exp);
            }

            bool isNegativeNumeric(const BasePtr& ptr)
            {
                return ptr->isNumeric() && ptr->isNegative();
            }

            void powerNegNumericExp(const BasePtr& base, const BasePtr& exp)
            {
                const bool denomNeedsParentheses = isScalarPowerBase(base);

                engine.openNumerator()
                  .number(1)
                  .closeNumerator()
                  .openDenominator(denomNeedsParentheses)
                  .invokePrint(Power::create(base, Product::minus(exp)))
                  .closeDenominator(denomNeedsParentheses);
            }

            bool isScalarPowerBase(const BasePtr& base)
            {
                const auto isPositiveInt = [](const auto& base) {
                    if (!base->isNumeric())
                        return false;

                    const auto n = base->numericEval();

                    return isInt(n) && n > 0;
                };

                return base->isSymbol() || base->isConstant() || base->isFunction() || isPositiveInt(base);
            }

            void standardPower(const BasePtr& base, const BasePtr& exp)
            {
                powerBase(base);
                powerExponent(exp);
            }

            void powerBase(const BasePtr& base)
            {
                if (isScalarPowerBase(base))
                    engine.invokePrint(base);
                else
                    engine.openParentheses().invokePrint(base).closeParentheses();
            }

            void powerExponent(const BasePtr& exp)
            {
                if (exp->isOne())
                    return;
                else if (isScalarPowerExp(exp))
                    engine.openScalarExponent().invokePrint(exp).closeScalarExponent();
                else
                    engine.openCompositeExponent().invokePrint(exp).closeCompositeExponent();
            }

            bool isScalarPowerExp(const BasePtr& exp)
            {
                if (exp->isSymbol() || exp->isConstant() || exp->isFunction())
                    return true;
                else if (exp->isNumeric())
                    return isInt(exp->numericEval()) && exp->isPositive();
                else
                    return false;
            }

            void sum(const Base& sum)
            {
                auto summands = sum.operands();

                engine.invokePrint(summands.front());
                summands.pop_front();

                for (auto& summand : summands) {
                    if (isProductWithNegativeNumeric(summand)) {
                        engine.minusSign();
                        summand = Product::minus(summand);
                    } else
                        engine.plusSign();

                    engine.invokePrint(summand);
                }
            }

            bool isProductWithNegativeNumeric(const BasePtr& summand)
            {
                if (summand->isProduct()) {
                    const auto& first = summand->operands().front();

                    return first->isNumeric() && first->isNegative();
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
                else if (num.size() == 1 && precedence(num.front()) < productPrecedence)
                    engine.openParentheses().invokePrint(num.front()).closeParentheses();
                else
                    productWithoutFractions(num);

                if (denom.empty())
                    return;

                engine.divisionSign();

                if (denom.size() == 1 && precedence(denom.front()) > productPrecedence)
                    engine.invokePrint(denom.front());
                else
                    engine.openParentheses().invokePrint(Product::create(denom)).closeParentheses();
            }

            std::pair<tsym::BasePtrList, tsym::BasePtrList> getProductFrac(const BasePtrList& origFactors)
            {
                auto frac = std::pair<BasePtrList, BasePtrList>{};

                for (const auto& origFactor : origFactors) {
                    const auto& exp = origFactor->exp();

                    if (isNegativeNumeric(exp)) {
                        frac.second.push_back(Power::create(origFactor->base(), Product::minus(exp)));
                    } else
                        frac.first.push_back(origFactor);
                }

                if (frac.first.empty() || frac.second.size() <= 1 || !frac.first.front()->isNumeric())
                    return frac;

                /* Adjust the previous logic and move factors like 2/3 to numerator/denominator. */
                const auto fracFactor = frac.first.front()->numericEval();
                frac.first.pop_front();

                frac.first.push_front(Numeric::create(fracFactor.numerator()));
                frac.second.push_front(Numeric::create(fracFactor.denominator()));

                return frac;
            }

            unsigned precedence(const BasePtr& ptr)
            {
                if (ptr->isSum())
                    return 1;
                else if (ptr->isProduct())
                    return 2;
                else if (ptr->isPower())
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
                    engine.invokePrint(first);
                else if (Product::minus(first)->isOne())
                    engine.unaryMinusSign();
                else if (first->isOne())
                    ;
                else if (precedence(first) < productPrecedence)
                    engine.openParentheses().invokePrint(first).closeParentheses().timesSign();
                else
                    engine.invokePrint(first).timesSign();

                for (auto factor = cbegin(factors); factor != cend(factors); ++factor) {
                    if (precedence(*factor) < productPrecedence)
                        engine.openParentheses().invokePrint(*factor).closeParentheses();
                    else
                        engine.invokePrint(*factor);

                    if (factor != std::prev(cend(factors)))
                        engine.timesSign();
                }
            }

            void function(const Base& fct)
            {
                const auto& ops = fct.operands();

                engine.functionName(fct.name()).openParentheses().invokePrint(ops.front());

                if (ops.size() == 2)
                    engine.comma().invokePrint(ops.back());

                engine.closeParentheses();
            }

            Engine& engine;
            PowerAsFraction powerAsFraction;
        };

        template <class Engine> void print(Engine& engine, const Base& base, PowerAsFraction opt)
        {
            Printer<Engine> printer(engine, opt);
            printer.print(base);
        }
    }
}

template <class Engine> void tsym::printer::print(Engine& engine, const Number& number)
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

template <class Engine> void tsym::printer::print(Engine& engine, const Base& base)
{
    print(engine, base, PowerAsFraction::TRUE);
}

template <class Engine> void tsym::printer::printDebug(Engine& engine, const Base& base)
{
    print(engine, base, PowerAsFraction::FALSE);
}

template void tsym::printer::print<tsym::PlaintextPrintEngine>(PlaintextPrintEngine&, const Number&);
template void tsym::printer::print<tsym::PlaintextPrintEngine>(PlaintextPrintEngine&, const Base&);
template void tsym::printer::printDebug<tsym::PlaintextPrintEngine>(PlaintextPrintEngine&, const Base&);
