
#include "cpputest.h"
#include "abc.h"
#include "numeric.h"
#include "symbol.h"
#include "power.h"
#include "constant.h"
#include "sum.h"
#include "product.h"

using namespace tsym;

TEST_GROUP(BasePtr) {};

TEST(BasePtr, init)
{
    CHECK_EQUAL(1, ten->getRefCount());
}

TEST(BasePtr, noArgumentCtor)
{
    BasePtr ptr;

    CHECK(ptr->isUndefined());
    /* See the BasePtr default constructor, why the reference count is 2. */
    CHECK_EQUAL(2, ptr->getRefCount());

    ptr = Symbol::create("dummy");

    CHECK(ptr->isSymbol());
    CHECK_EQUAL(1, ptr->getRefCount());
}

TEST(BasePtr, copyCtor)
{
    const BasePtr copy(ten);

    CHECK_EQUAL(2, ten->getRefCount());
}

TEST(BasePtr, pointerAccess)
{
    const Base *base = ten.base();

    POINTERS_EQUAL(base, &*ten);
}

TEST(BasePtr, assignmentOperator)
{
    BasePtr assigned = Symbol::create("dummy");

    assigned = ten;

    CHECK_EQUAL(2, ten->getRefCount());
}

TEST(BasePtr, selfAssignment)
{
    BasePtr dummy = Numeric::create(12345);

    dummy = dummy;

    CHECK_EQUAL(1, dummy->getRefCount());
}

TEST(BasePtr, zeroAndNotZero)
{
    const BasePtr zero = Numeric::create(0);

    CHECK(zero->isZero());
    CHECK(!ten->isZero());
    CHECK(!a->isZero());
}

TEST(BasePtr, oneAndNotOne)
{
    const BasePtr one = Numeric::create(1);

    CHECK(one->isOne());
    CHECK(!ten->isOne());
    CHECK(!a->isOne());
}

TEST(BasePtr, undefinedTypeRequest)
{
    const BasePtr ptr = Undefined::create();

    CHECK(ptr->isUndefined());
}

TEST(BasePtr, symbolTypeRequest)
{
    const Name expected("a");

    CHECK(a->isSymbol());
    CHECK_EQUAL(expected, a->name());
}

TEST(BasePtr, numericTypeRequest)
{
    CHECK(ten->isNumeric());
    CHECK_EQUAL(10, ten->numericEval());
}

TEST(BasePtr, powerTypeRequest)
{
    const BasePtr pow = Power::create(a, ten);

    CHECK(pow->isPower());
    CHECK_EQUAL(ten, pow->exp());
    CHECK_EQUAL(a, pow->base());
}

TEST(BasePtr, sumTypeRequest)
{
    const BasePtr ptr = Sum::create(ten, a);

    CHECK(ptr->isSum());
}

TEST(BasePtr, productTypeRequest)
{
    const BasePtr res = Product::create(ten, a);

    CHECK(res->isProduct());
}

TEST(BasePtr, constantTypeRequest)
{
    const BasePtr res = Constant::createPi();

    CHECK(res->isConstant())
}

TEST(BasePtr, constCheckNumeric)
{
    CHECK(ten->isConst());
}

TEST(BasePtr, constCheckSymbol)
{
    CHECK(!a->isConst());
}

TEST(BasePtr, constCheckUndefined)
{
    const BasePtr u = Undefined::create();

    CHECK(!u->isConst());
}

TEST(BasePtr, constCheckNumericPower)
{
    const BasePtr half = Numeric::create(1, 2);
    const BasePtr pow = Power::sqrt(ten);

    CHECK(pow->isConst());
    CHECK(pow->isNumericPower());
}

TEST(BasePtr, constCheckConstPower)
    /* A power with a true Constant involved (Pi) isn't considered as a const expression, because
     * Constants are treated as Symbols. */
{
    const BasePtr pi = Constant::createPi();
    const BasePtr exp = Numeric::create(4, 7);
    const BasePtr pow = Power::create(pi, exp);

    CHECK(!pow->isConst());
    CHECK(!pow->isNumericPower());
}

TEST(BasePtr, constCheckNonConstPower)
{
    const BasePtr pow1 = Power::create(a, ten);
    const BasePtr pow2 = Power::create(ten, a);

    CHECK(!pow1->isConst());
    CHECK(!pow2->isConst());
}

TEST(BasePtr, constCheckConstSum)
{
    const BasePtr half = Numeric::create(1, 2);
    const BasePtr pow = Power::sqrt(ten);
    const BasePtr sum = Sum::create(ten, pow);

    CHECK(sum->isConst());
}

TEST(BasePtr, constCheckNonConstSum)
{
    BasePtr sum = Sum::create(ten, a);

    CHECK(!sum->isConst());
}

TEST(BasePtr, constCheckConstProduct)
{
    const BasePtr half = Numeric::create(1, 2);
    const BasePtr sqrtTwo = Power::sqrt(two);
    const BasePtr sqrtThree = Power::sqrt(three);
    const BasePtr sqrtFive = Power::sqrt(five);
    BasePtrList factors;
    BasePtr product;

    factors.push_back(half);
    factors.push_back(sqrtTwo);
    factors.push_back(sqrtThree);
    factors.push_back(sqrtFive);

    product = Product::create(factors);

    CHECK(product->isConst());
}

TEST(BasePtr, constCheckNonConstProduct)
{
    const BasePtr product = Product::create(ten, a);

    CHECK(!product->isConst());
}

TEST(BasePtr, numericTermOfNumeric)
{
    CHECK_EQUAL(five, five->numericTerm());
}

TEST(BasePtr, numericTermOfSymbol)
{
    CHECK_EQUAL(one, a->numericTerm());
}

TEST(BasePtr, numericTermOfProductWithNumeric)
{
    const BasePtr p = Product::create(two, Power::create(a, b));

    CHECK_EQUAL(two, p->numericTerm());
}

TEST(BasePtr, numericTermOfProductWithoutNumeric)
{
    const BasePtr p = Product::create(Power::create(a, b), Sum::create(c, d));

    CHECK_EQUAL(one, p->numericTerm());
}

TEST(BasePtr, numericTermOfProductWithConstPower)
{
    const BasePtr p = Product::create(two, Power::create(five, Numeric::create(1, 3)));

    CHECK_EQUAL(two, p->numericTerm());
}

TEST(BasePtr, numericTermOfSum)
{
    const BasePtr s = Sum::create(a, b);

    CHECK_EQUAL(one, s->numericTerm());
}

TEST(BasePtr, nonNumericTermOfNumeric)
{
    CHECK_EQUAL(one, five->nonNumericTerm());
}

TEST(BasePtr, nonNumericTermOfProduct)
{
    const BasePtr pow = Power::create(five, Numeric::create(1, 3));
    const BasePtr p = Product::create(two, pow);

    CHECK_EQUAL(pow, p->nonNumericTerm());
}

TEST(BasePtr, nonNumericTermOfSum)
{
    const BasePtr sum = Sum::create(a, b);

    CHECK_EQUAL(sum, sum->nonNumericTerm());
}

TEST(BasePtr, constTermOfNumeric)
{
    CHECK_EQUAL(five, five->constTerm());
}

TEST(BasePtr, constTermOfNumericPow)
{
    const BasePtr pow = Power::create(four, Numeric::create(1, 5));

    CHECK_EQUAL(pow, pow->constTerm());
}

TEST(BasePtr, constTermOfConstProduct)
{
    const BasePtr pow = Power::create(seven, Numeric::create(1, 5));
    const BasePtr expected = Product::create(three, pow);
    BasePtrList fac;
    BasePtr product;

    fac.push_back(three);
    fac.push_back(pow);
    fac.push_back(Constant::createPi());
    fac.push_back(Power::create(a, b));
    fac.push_back(Power::create(c, d));

    product = Product::create(fac);

    CHECK_EQUAL(expected, product->constTerm());
}

TEST(BasePtr, constTermOfSymbol)
{
    CHECK_EQUAL(one, a->constTerm());
}

TEST(BasePtr, nonConstTermOfNumeric)
{
    CHECK_EQUAL(one, four->nonConstTerm());
}

TEST(BasePtr, nonConstTermOfSymbol)
{
    CHECK_EQUAL(a, a->nonConstTerm());
}

TEST(BasePtr, nonConstTermOfConstProduct)
{
    const BasePtr pow = Power::create(three, Numeric::create(1, 13));
    BasePtr expected;
    BasePtrList fac;
    BasePtr product;

    fac.push_back(Constant::createPi());
    fac.push_back(Power::create(a, b));
    fac.push_back(Power::create(c, d));

    expected = Product::create(fac);

    fac.push_back(four);
    fac.push_back(pow);

    product = Product::create(fac);

    CHECK_EQUAL(expected, product->nonConstTerm());
}

TEST(BasePtr, nonConstTermOfConstSum)
{
    const BasePtr sum = Sum::create(three, Power::sqrt(two));

    CHECK_EQUAL(one, sum->nonConstTerm());
}

TEST(BasePtr, constTermOfSumWithConstant)
{
    const BasePtr pow = Power::create(two, Numeric::create(1, 3));
    const BasePtr sum = Sum::create(Constant::createPi(), pow);

    CHECK_EQUAL(one, sum->constTerm());
}

TEST(BasePtr, constTermOfConstSum)
{
    const BasePtr sum = Sum::create(Power::create(seven, Numeric::create(1, 3)),
            Sum::create(two, Power::sqrt(three)));

    CHECK_EQUAL(sum, sum->constTerm());
}

TEST(BasePtr, nonConstTermOfSum)
{
    const BasePtr sum = Sum::create(Constant::createPi(), a);

    CHECK_EQUAL(sum, sum->nonConstTerm());
}

TEST(BasePtr, nonConstTermOfConstPow)
{
    const BasePtr pow = Power::create(ten, Numeric::create(1, 7));

    CHECK_EQUAL(one, pow->nonConstTerm());
}

TEST(BasePtr, nonConstTermOfPow)
{
    const BasePtr pow = Power::create(a, b);

    CHECK_EQUAL(pow, pow->nonConstTerm());
}
