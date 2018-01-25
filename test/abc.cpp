
#include "abc.h"
#include "symbol.h"
#include "numeric.h"

const tsym::BasePtr a(tsym::Symbol::create("a"));
const tsym::BasePtr b(tsym::Symbol::create("b"));
const tsym::BasePtr c(tsym::Symbol::create("c"));
const tsym::BasePtr d(tsym::Symbol::create("d"));
const tsym::BasePtr e(tsym::Symbol::create("e"));
const tsym::BasePtr f(tsym::Symbol::create("f"));
const tsym::BasePtr g(tsym::Symbol::create("g"));
const tsym::BasePtr h(tsym::Symbol::create("h"));

const tsym::BasePtr zero(tsym::Numeric::zero());
const tsym::BasePtr one(tsym::Numeric::one());
const tsym::BasePtr two(tsym::Numeric::two());
const tsym::BasePtr three = tsym::Numeric::three();
const tsym::BasePtr four = tsym::Numeric::four();
const tsym::BasePtr five = tsym::Numeric::create(5);
const tsym::BasePtr six = tsym::Numeric::create(6);
const tsym::BasePtr seven = tsym::Numeric::create(7);
const tsym::BasePtr eight = tsym::Numeric::create(8);
const tsym::BasePtr nine = tsym::Numeric::create(9);
const tsym::BasePtr ten = tsym::Numeric::create(10);
