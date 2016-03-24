
#include <cassert>
#include <iomanip>
#include "printer.h"
#include "symbol.h"
#include "numeric.h"
#include "power.h"
#include "product.h"
#include "function.h"
#include "constant.h"
#include "trigonometric.h"
#include "logarithm.h"
#include "sum.h"
#include "logging.h"

bool tsym::Printer::convertToFrac = true;

tsym::Printer::Printer()
{
    setDefaults();
}

tsym::Printer::Printer(const Var& var)
{
    setDefaults();
    print(var.getBasePtr());
}

tsym::Printer::Printer(const Number& number)
{
    setDefaults();
    print(Numeric::create(number));
}

tsym::Printer::Printer(const BasePtr& ptr)
{
    setDefaults();
    print(ptr);
}

tsym::Printer::Printer(const Vector& vector)
{
    setDefaults();
    print(vector);
}

tsym::Printer::Printer(const Matrix& matrix)
{
    setDefaults();
    print(matrix);
}

void tsym::Printer::setDefaults() {}

void tsym::Printer::set(const Var& var)
{
    clearStream();
    print(var.getBasePtr());
}

void tsym::Printer::set(const Number& number)
{
    clearStream();
    print(Numeric::create(number));
}

void tsym::Printer::set(const BasePtr& ptr)
{
    clearStream();
    print(ptr);
}

void tsym::Printer::set(const Vector& vector)
{
    clearStream();
    print(vector);
}

void tsym::Printer::set(const Matrix& matrix)
{
    clearStream();
    print(matrix);
}

void tsym::Printer::clearStream()
{
    stream.str("");
}

void tsym::Printer::print(const BasePtr& ptr)
{
    if (ptr->isSymbol())
        printSymbol(ptr);
    else if (ptr->isNumeric())
        printNumeric(ptr);
    else if (ptr->isPower())
        printPower(ptr);
    else if (ptr->isSum())
        printSum(ptr);
    else if (ptr->isProduct())
        printProduct(ptr);
    else if (ptr->isFunction())
        printFunction(ptr);
    else if (ptr->isConstant())
        printConstant(ptr);
    else if (ptr->isUndefined())
        stream << "Undefined";
    else
        stream << "Unknown";
}

void tsym::Printer::printSymbol(const BasePtr& ptr)
{
    stream << ptr->name().plain();
}

void tsym::Printer::printNumeric(const BasePtr& ptr)
{
    printNumber(ptr->numericEval());
}

void tsym::Printer::printNumber(const Number& n)
{
    if (n.isDouble())
        stream << n.toDouble();
    else if (n.isUndefined())
        /* This shouldn't happen, because printing a number causes the creation of a BasePtr, which
         * will be of type Undefined, if the Number is undefined. */
        stream << "Undefined";
    else {
        stream << n.numerator();

        if (n.isFrac())
            stream << "/" << n.denominator();
    }
}

void tsym::Printer::printPower(const BasePtr& ptr)
{
    const BasePtr& base(ptr->base());
    const BasePtr& exp(ptr->exp());

    printPower(base, exp);
}

void tsym::Printer::printPower(const BasePtr& base, const BasePtr& exp)
{
    const BasePtr half(Numeric::create(1, 2));

    if (exp->isEqual(half)) {
        stream << "sqrt";
        printInParentheses(base);
        return;
    } else if (isNegativeNumeric(exp) && convertToFrac) {
        stream << "1/";
        printPower(base, toggleSign(exp));
        return;
    }

    printBase(base);
    printExponent(exp);
}

bool tsym::Printer::isNegativeNumeric(const BasePtr& ptr) const
{
    return ptr->isNumeric() && ptr->numericEval() < 0;
}

tsym::BasePtr tsym::Printer::toggleSign(const BasePtr& numeric) const
{
    assert(numeric->isNumeric());

    return Numeric::create(-numeric->numericEval());
}

void tsym::Printer::printBase(const BasePtr& base)
{
    if (needsBaseParentheses(base))
        printInParentheses(base);
    else
        print(base);
}

bool tsym::Printer::needsBaseParentheses(const BasePtr& base) const
{
    if (base->isSymbol() || base->isConstant() || base->isFunction() || isPositiveInt(base))
        return false;
    else
        return true;
}

bool tsym::Printer::isPositiveInt(const BasePtr& ptr) const
{
    Number n;

    if (!ptr->isNumeric())
        return false;

    n = ptr->numericEval();

    return n.isInt() && n > 0;
}

void tsym::Printer::printExponent(const BasePtr& exp)
{
    if (exp->isOne())
        return;

    stream << "^";

    if (needsExpParentheses(exp))
        printInParentheses(exp);
    else
        print(exp);
}

bool tsym::Printer::needsExpParentheses(const BasePtr& ptr) const
{
    Number nExp;

    if (ptr->isSymbol() || ptr->isConstant() || ptr->isFunction())
        return false;
    else if (!ptr->isNumeric())
        return true;

    nExp = ptr->numericEval();

    return !(nExp.isInt() && nExp> 0);
}

unsigned tsym::Printer::prec(const BasePtr& ptr) const
{
    const Base& base(*ptr);

    return prec(typeid(base));
}

unsigned tsym::Printer::prec(const std::type_info& type) const
{
    if (type == typeid(Sum))
        return 1;
    else if (type == typeid(Product))
        return 2;
    else if (type == typeid(Power))
        return 3;
    else
        return 4;
}

void tsym::Printer::printInParentheses(const BasePtr& ptr)
{
    stream << "(";

    print(ptr);

    stream << ")";
}

void tsym::Printer::printSum(const BasePtr& ptr)
{
    const BasePtrList summands(ptr->operands());
    BasePtr element;

    for (BasePtrList::const_iterator it = summands.begin(); it != summands.end(); ++it) {
        element = *it;

        if (it != summands.begin()) {
            if (isProductWithNegativeNumeric(*it)) {
                stream << " - ";
                element = Product::minus(*it);
            }
            else
                stream << " + ";
        }

        print(element);
    }
}

bool tsym::Printer::isProductWithNegativeNumeric(const BasePtr& ptr)
{
    BasePtr first;

    if (!ptr->isProduct())
        return false;

    first = ptr->operands().front();

    if (first->isNumeric())
        return first->numericEval() < 0;

    return false;
}

void tsym::Printer::printProduct(const BasePtr& ptr)
{
    BasePtrList factors(ptr->operands());
    const BasePtr& f1(factors.front());

    if (f1->isNumeric() && f1->numericEval() == -1) {
        stream << "-";
        factors.pop_front();
    }

    if (convertToFrac)
        printProductFrac(factors);
    else
        printProduct(factors);
}

void tsym::Printer::printProductFrac(const BasePtrList& factors)
{
    const std::pair<BasePtrList, BasePtrList> frac(getProductFrac(factors));
    const BasePtrList& num(frac.first);
    const BasePtrList& denom(frac.second);

    if (num.empty())
        stream << "1";
    else if (num.size() == 1 && prec(num.front()) < prec(typeid(Product)))
        printInParentheses(num.front());
    else
        printProduct(num);

    if (denom.empty())
        return;

    stream << "/";

    if (denom.size() == 1 && prec(denom.front()) > prec(typeid(Product)))
        print(denom.front());
    else
        printInParentheses(Product::create(denom));
}

std::pair<tsym::BasePtrList, tsym::BasePtrList> tsym::Printer::getProductFrac(
        const BasePtrList& origFactors)
{
    std::pair<BasePtrList, BasePtrList> frac;
    BasePtrList::const_iterator it;
    Number fracFactor;
    BasePtr base;
    BasePtr exp;

    for (it = origFactors.begin(); it != origFactors.end(); ++it) {
        if ((*it)->isPower())
            exp = (*it)->exp();
        else
            exp = Undefined::create();

        if (isNegativeNumeric(exp)) {
            base = (*it)->base();
            frac.second.push_back(Power::create(base, toggleSign(exp)));
        } else
            frac.first.push_back(*it);
    }

    if (frac.first.empty() || frac.second.size() <= 1 || !frac.first.front()->isNumeric())
        return frac;

    /* Adjust the previous logic and move factors like 2/3 to numerator/denominator. */
    fracFactor = frac.first.pop_front()->numericEval();

    frac.first.push_front(Numeric::create(fracFactor.numerator()));
    frac.second.push_front(Numeric::create(fracFactor.denominator()));

    return frac;
}

void tsym::Printer::printProduct(const BasePtrList& factors)
{
    const unsigned productPrec = prec(typeid(Product));

    for (BasePtrList::const_iterator it = factors.begin(); it != factors.end(); ++it) {
        if (prec(*it) < productPrec)
            printInParentheses(*it);
        else
            print(*it);

        if (it != --factors.end())
            stream << "*";
    }
}

void tsym::Printer::printFunction(const BasePtr& ptr)
{
    stream << ptr->name().plain() << "(" << ptr->operands().front() << ")";
}

void tsym::Printer::printConstant(const BasePtr& ptr)
{
    stream << ptr->name();
}

void tsym::Printer::print(const Vector& vector)
{
    int maxChars = getMaxCharacters(vector);

    if (vector.size() == 0) {
        stream << "[ ]";
        return;
    }

    for (size_t i = 0; i < vector.size(); ++i) {
        stream << "[ ";
        stream << std::right << std::setw(maxChars) << Printer(vector(i)).getStr();
        stream << " ]";
        if (i < vector.size() - 1)
            stream << std::endl;
    }
}

int tsym::Printer::getMaxCharacters(const Vector& vector) const
    /* A size_t return type would make more sense here, but as the return value is used as argument
     * to std::setw, it's casted to an integer here, which should be safe to do, since the quantity
     * in question is the length of a string of a printed expression. */
{
    int maxLength = 0;
    size_t origLength;
    int length;

    for (size_t i = 0; i < vector.size(); ++i) {
        origLength = Printer(vector(i)).getStr().length();
        length = static_cast<int>(origLength);
        maxLength = length > maxLength ? length : maxLength;
    }

    return maxLength;
}

void tsym::Printer::print(const Matrix& matrix)
{
    std::vector<int> maxChars;
    defMaxCharsPerColumn(matrix, maxChars);

    if (matrix.rowSize() + matrix.colSize() == 0) {
        stream << "[ ]";
        return;
    }

    for (size_t i = 0; i < matrix.rowSize(); ++i) {
        stream << "[ ";
        for (size_t j = 0; j < matrix.colSize(); ++j) {
            stream << std::right << std::setw(maxChars[j]) << Printer(matrix(i, j)).getStr();
            if (j < matrix.colSize() - 1)
                stream << "  ";
        }
        stream << " ]";
        if (i < matrix.rowSize() - 1)
            stream << std::endl;
    }
}

void tsym::Printer::defMaxCharsPerColumn(const Matrix& matrix, std::vector<int>& maxChars) const
    /* See getMaxCharacters above for an explanation of the types holding the character count. */
{
    int maxLength = 0;
    size_t origLength;
    int length;

    for (size_t i = 0; i < matrix.colSize(); ++i, maxLength = 0) {
        for (size_t j = 0; j < matrix.rowSize(); ++j) {
            origLength = Printer(matrix(j, i)).getStr().length();
            length = static_cast<int>(origLength);
            maxLength = length > maxLength ? length : maxLength;
        }
        maxChars.push_back(maxLength);
    }
}

void tsym::Printer::print(std::ostream& stream)
{
    stream << this->stream.str();
}

void tsym::Printer::enableFractions()
{
    convertToFrac = true;
}

void tsym::Printer::disableFractions()
{
    convertToFrac = false;
}

std::string tsym::Printer::getStr() const
{
    return stream.str();
}
