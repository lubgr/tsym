
#include <stack>
#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <algorithm>
#include <boost/spirit/include/qi.hpp>
#include "numeric.h"
#include "logarithm.h"
#include "constant.h"
#include "product.h"
#include "undefined.h"
#include "power.h"
#include "symbol.h"
#include "sum.h"
#include "trigonometric.h"
#include "parser.h"

namespace tsym {
    namespace {
        namespace qi = boost::spirit::qi;

        template<typename Iterator> struct SymbolParser : qi::grammar<Iterator, std::vector<std::string>()> {
            SymbolParser() : SymbolParser::base_type(symbol)
            {
                using qi::ascii::alpha;
                using qi::ascii::alnum;

                name = alpha >> *alnum;
                subscript = (('{' >> +alnum >> '}') | alnum);

                symbol = name >> -('_' >> subscript);
            }

            qi::rule<Iterator, std::string()> name;
            qi::rule<Iterator, std::string()> subscript;
            qi::rule<Iterator, std::vector<std::string>()> symbol;
        };

        template<typename Iterator> struct BigIntParser : qi::grammar<Iterator, std::string()> {
            BigIntParser() : BigIntParser::base_type(intStr)
            {
                intStr = +qi::ascii::digit;
            }

            qi::rule<Iterator, std::string()> intStr;
        };

        template<class Iterator> class QiParser {
            private:
                enum class Fct { SIN, COS, TAN, ASIN, ACOS, ATAN, LOG, SQRT };
                using Rule = qi::rule<Iterator, boost::spirit::ascii::space_type>;
                using NoSpaceRule = qi::rule<Iterator>;

            public:
                QiParser()
                {
                    using boost::spirit::ascii::no_case;

                    functions.add("sin", Fct::SIN)("cos", Fct::COS)("tan", Fct::TAN)
                        ("asin", Fct::ASIN)("acos", Fct::ACOS)("atan", Fct::ATAN)("sqrt", Fct::SQRT)("log", Fct::LOG);

                    scalar = no_case[qi::lit("pi")][bind(&QiParser::pushPi)]
                        | no_case[qi::lit("euler")][bind(&QiParser::pushEuler)]
                        | symbol[bindWithArg(&QiParser::pushSymbol)]
                        | floatingPoint[bindWithArg(&QiParser::pushDouble)]
                        | integer[bindWithArg(&QiParser::pushInteger)];

                    oneArgFunction = (no_case[functions] >> '(' >> expression >> ')')
                        [bindWithArg(&QiParser::toFunction)];

                    twoArgFunction = (no_case[qi::lit("atan2")] >> '(' >> expression >> ',' >> expression >> ')')
                        [bind(&QiParser::toAtan2)];

                    function = oneArgFunction
                        | twoArgFunction;

                    operand = function
                        | scalar
                        | '(' >> expression >> ')';

                    factor = operand >> *('^' >> factor[bind(&QiParser::toPower)]);

                    posNegOperand = factor
                        | '-' >> factor[bind(&QiParser::flipSign)];

                    summand = posNegOperand >> *(('*' >> posNegOperand)[bind(&QiParser::toProduct)]
                            | ('/' >> posNegOperand)[bind(&QiParser::toQuotient)]);

                    expression = -qi::lit('+') >> summand >> *(('+' >> summand)[bind(&QiParser::toSum)]
                            | ('-' >> summand)[bind(&QiParser::toDifference)]);
                }

                parser::Result parse(Iterator first, Iterator last)
                {
                    bool success = qi::phrase_parse(first, last, expression, boost::spirit::ascii::space);

                    return result(success, first == last);
                }

                parser::Result result(bool success, bool fullyParsed)
                {
                    const BasePtr& parsed = stack.empty() ? Undefined::create() : stack.top();

                    return { parsed, success, fullyParsed };
                }

            private:
                template<class MethodPtr> auto bind(MethodPtr ptr)
                {
                    return std::bind(ptr, this);
                }

                template<class MethodPtr> auto bindWithArg(MethodPtr ptr)
                {
                    return std::bind(ptr, this, std::placeholders::_1);
                }

                void pushPi()
                {
                    stack.push(Constant::createPi());
                }

                void pushEuler()
                {
                    stack.push(Constant::createE());
                }

                void pushSymbol(const std::vector<std::string>& nameSubscript)
                {
                    const auto size = nameSubscript.size();

                    assert(size == 1 || size == 2);

                    const Name name(nameSubscript.front(), size == 2 ? nameSubscript.back() : "");

                    stack.push(Symbol::create(name));
                }

                void pushDouble(double n)
                {
                    stack.push(Numeric::create(n));
                }

                void pushInteger(const std::string& digits)
                {
                    stack.push(Numeric::create(Int(digits)));
                }

                void toFunction(Fct type)
                {
                    const BasePtr arg = pop();
                    BasePtr result;

                    switch (type) {
                        case Fct::SIN:
                            result = Trigonometric::createSin(arg); 
                            break;
                        case Fct::COS:
                            result = Trigonometric::createCos(arg); 
                            break;
                        case Fct::TAN:
                            result = Trigonometric::createTan(arg); 
                            break;
                        case Fct::ASIN:
                            result = Trigonometric::createAsin(arg); 
                            break;
                        case Fct::ACOS:
                            result = Trigonometric::createAcos(arg); 
                            break;
                        case Fct::ATAN:
                            result = Trigonometric::createAtan(arg); 
                            break;
                        case Fct::LOG:
                            result = Logarithm::create(arg); 
                            break;
                        case Fct::SQRT:
                            result = Power::sqrt(arg); 
                            break;
                    }

                    stack.push(std::move(result));
                };

                void toAtan2()
                {
                    const BasePtr x = pop();
                    const BasePtr y = pop();

                    stack.push(Trigonometric::createAtan2(y, x));
                }

                void toPower()
                {
                    const BasePtr exp = pop();
                    const BasePtr base = pop();

                    stack.push(Power::create(base, exp));
                }

                void flipSign()
                {
                    stack.push(Product::minus(pop()));
                }

                void toProduct()
                {
                    const BasePtr rhs = pop();
                    const BasePtr lhs = pop();

                    stack.push(Product::create(lhs, rhs));
                }

                void toQuotient()
                {
                    const BasePtr rhs = pop();

                    stack.push(Power::oneOver(rhs));

                    toProduct();
                }

                void toSum()
                {
                    const BasePtr rhs = pop();
                    const BasePtr lhs = pop();

                    stack.push(Sum::create(lhs, rhs));
                }

                void toDifference()
                {
                    flipSign();
                    toSum();
                }

                BasePtr pop()
                {
                    const BasePtr topElement = stack.top();

                    stack.pop();

                    return topElement;
                }

                std::stack<BasePtr> stack;
                qi::symbols<char, Fct> functions;
                qi::real_parser<double, qi::strict_ureal_policies<double>> floatingPoint{};
                SymbolParser<Iterator> symbol{};
                BigIntParser<Iterator> integer{};
                Rule expression;
                NoSpaceRule scalar;
                Rule oneArgFunction;
                Rule twoArgFunction;
                Rule function;
                Rule operand;
                Rule posNegOperand;
                Rule factor;
                Rule summand;
        };

        tsym::parser::Result parseAsciiOnly(const std::string& inputStr)
        {
            QiParser<std::string::const_iterator> parser;

            return parser.parse(cbegin(inputStr), cend(inputStr));
        }

        bool isAsciiOnly(const std::string& inputStr)
        {
            return std::all_of(cbegin(inputStr), cend(inputStr), [](auto ch){ return ch >= 0; });
        }
    }
}

tsym::parser::Result tsym::parser::parse(const std::string& inputStr)
{
    if (isAsciiOnly(inputStr))
        return parseAsciiOnly(inputStr);
    else
        return { Undefined::create(), false, false };
}
