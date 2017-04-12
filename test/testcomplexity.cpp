
#include "abc.h"
#include "constant.h"
#include "power.h"
#include "sum.h"
#include "product.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"
#include "logarithm.h"

using namespace tsym;

TEST_GROUP(Complexity)
{
    BasePtr undef;
    BasePtr integer;
    BasePtr fraction;
    BasePtr doubleval;
    BasePtr constant;
    BasePtr symbol;
    BasePtr prod;
    BasePtr power;
    BasePtr sinus;
    BasePtr ln;
    BasePtr undefnumeric;

    void setup()
    {
        integer = Numeric::create(3);
        doubleval= Numeric::create(2.7665454894445454);
        constant = Constant::createPi();
        symbol = Symbol::create("a");
        prod = Product::create(integer,symbol,constant);
        power = Power::create(integer,symbol);
        sinus = Trigonometric::createSin(symbol);
        ln = Logarithm::create(symbol);
    }
};

TEST(Complexity, undefined)
{
    BasePtr undef = Undefined::create();
    unsigned res = undef->complexity();
    CHECK_EQUAL(0,res);
}

TEST(Complexity, integer)
{
    unsigned res = integer->complexity();
    CHECK_EQUAL(1,res);
}

TEST(Complexity, fraction)
{
    BasePtr fraction = Numeric::create(2,3);
    unsigned res = fraction->complexity();
    CHECK_EQUAL(2,res);
}

TEST(Complexity, double)
{
    unsigned res = doubleval->complexity();
    CHECK_EQUAL(3,res);
}

TEST(Complexity, constant)
{
    unsigned res = constant->complexity();
    CHECK_EQUAL(4,res);
}

TEST(Complexity, symbol)
{
    unsigned res = symbol->complexity();
    CHECK_EQUAL(5, res);
}

TEST(Complexity, sum)
{
    BasePtr sum = Sum::create(integer,symbol);
    unsigned res = sum->complexity();
    CHECK_EQUAL(5 + 1 + 5,res );
}

TEST(Complexity, product)
{
    unsigned res = prod->complexity();
    CHECK_EQUAL(5 + 1 + 5 + 4,res );
}

TEST(Complexity, power)
{
    unsigned res = power->complexity();
    CHECK_EQUAL(5 + 1 + 2*5,res );
}

TEST(Complexity, trigonometric)
{
    unsigned res = sinus->complexity();
    CHECK_EQUAL(6 + 5,res );
}

TEST(Complexity, logarithm)
{
    unsigned res = ln->complexity();
    CHECK_EQUAL(6 + 5,res );
}

TEST(Complexity, largeterm)
{
    unsigned expected = 5 + 15 + 5 + 3 + 11 + 16 + 11 + 4;
    std::initializer_list<BasePtr> list = {prod,symbol,doubleval,sinus,power,ln,constant};
    BasePtrList largetermlist(list);
    BasePtr largeterm = Sum::create(largetermlist);
    unsigned res = largeterm->complexity();
    CHECK_EQUAL(expected,res );
}