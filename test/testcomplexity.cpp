
#include "abc.h"
#include "constant.h"
#include "power.h"
#include "sum.h"
#include "product.h"
#include "trigonometric.h"
#include "tsymtests.h"
#include "undefined.h"

using namespace tsym;

TEST_GROUP(Complexity)
{
    
    BasePtr undef;
    BasePtr integer;
    BasePtr fraction;
    BasePtr doubleval;
    BasePtr constant;
    BasePtr symbol;
    BasePtr sum;
    BasePtr prod;
    BasePtr power;
    BasePtr sinus;

    void setup()
    {
        double a =2.7665454894445454;
        undef = Undefined::create();
        integer = Numeric::create(3);
        fraction = Numeric::create(2,3);
        doubleval= Numeric::create(a);
        constant = Constant::createPi();
        symbol = Symbol::create("a");
        sum = Sum::create(integer,symbol);
        prod = Product::create(integer,symbol,constant);
        power = Power::create(integer,symbol);
        sinus = Trigonometric::createSin(symbol);
    }

};

TEST(Complexity, Undefined)
{
    unsigned int res = undef->complexity();

    CHECK_EQUAL(0,res);
}

TEST(Complexity, Integer)
{
    unsigned int res = integer->complexity();

    CHECK_EQUAL(1,res);
}

TEST(Complexity, Fraction)
{
    unsigned int res = fraction->complexity();

    CHECK_EQUAL(2,res);
}

TEST(Complexity, Double)
{
    unsigned int res = doubleval->complexity();

    CHECK_EQUAL(3,res);
}

TEST(Complexity, Constant)
{
    unsigned int res = constant->complexity();

    CHECK_EQUAL(4,res);
}

TEST(Complexity, Symbol)
{
    unsigned int res = symbol->complexity();

    CHECK_EQUAL(5, res);
}

TEST(Complexity, Sum)
{   
    unsigned int expected = 5+1+5;
    unsigned int res = sum->complexity();
    
    CHECK_EQUAL(expected,res );
}

TEST(Complexity, Product)
{   
    unsigned int expected = 5+1+5+4;
    unsigned int res = prod->complexity();
    
    CHECK_EQUAL(expected,res );
}

TEST(Complexity, Power)
{   
    unsigned int expected = 5+1+2*5;
    unsigned int res = power->complexity();
    
    CHECK_EQUAL(expected,res );
}

TEST(Complexity, Trigonometric)
{   
    unsigned int expected = 6+5;
    unsigned int res = sinus->complexity();
    
    CHECK_EQUAL(expected,res );
}