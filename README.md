
# tsym: Tiny Symbolic Library

This small C++ library intends to provide a simple approach to symbolic algebra. In contrast to
fully-fledged computer algebra systems or libraries (e.g. maxima, GiNaC, sympy), only the most basic
features are implemented and the scope is limited to real-valued symbols: no complex numbers are
taken into account during expression simplification (this prohibits some simplifications and allows
for some). The main features are:

* automatic simplification `a + b + 2*a = 3*a + b`
* expand expressions `(a + b)^2 = a^2 + 2*a*b + b^2`
* substitute (sub-)expressions `3*a + b = 6 + b` for `a = 2`
* trigonometric functions `sin(pi/4) = 1/sqrt(2)` or `sin^2(a) + cos^2(a) = 1`
* expression normalization via gcd `a/b + 1/(5*b) = 1/5*(1 + 5*a)/b`
* differentiation `d/da 2*a^4 = 8*a^3`
* solution of simple linear systems of equations
* parsing of expressions from strings

Most algorithms are implemented according to _Cohen, Computer Algebra and Symbolic Computation
[2003]_, modifications are made for the handling of numeric power expressions and when not taking
complex numbers into account. Absent features that are usually implemented in common CAS are e.g.
arbitrary precision arithmetics, collecting parts of an expression or series expansion.


Installation
------------
To compile tsym, you need the [plic](https://github.com/lubgr/plic) library for logging, flex and
bison (or their non-GPL counterparts), obviously C and C++ compiler recent enough to support C++11
and [scons](http://scons.org) as a build system. For unit tests,
[CppUTest](https://github.com/cpputest/cpputest) is required. tsym should build on all major Linux
distributions by e.g.
```bash
scons CFLAGS="-O2" CXXFLAGS="-march=native -O2" lib
```
where the flag specifications are of course optional. See `scons -h` for details. To install header
files and the shared library:
```bash
scons install PREFIX=/usr/local
```
For all available build options and targets, see `scons -h` for details.

Usage
-----
There is only one header that needs to be included: `tsym/tsym.h`. The library provides a
**tsym::Var** class holding all expressions. It can be instantiated as a symbol or a number, and
arithmetic as well as stream operators are overloaded:
```c++
#include <iostream>
#include "tsym/tsym.h"

int main(int argc, char **argv)
{
    tsym::Var a("a");
    tsym::Var b("b_{subscript}");
    tsym::Var fraction(2, 3);

    std::cout << a + b*fraction << std::endl;
}
```
Implicit constructors are available for `int` and `double` arguments, such that intuitive
calculations are possible:
```c++
tsym::Var a("a");
tsym::Var b; /* Initialized to zero by default constructor. */

b = 2*a + a/1.2345;

/* Careful - this will be zero, because 2/3 is evaluated first and as plain integral type: */
b = 2/3*a;
```
Some mathematical functions and two constants (e and pi) are available inside of the tsym namespace:
```c++
tsym::Var minusPiFourth = tsym::Pi/4 - tsym::asin(1);
tsym::Var onePlusPiFourth = tsym::log(tsym::e) + tsym::acos(1/tsym::sqrt(2));
```
Information about expressions can be queried via methods of Var:
```c++
tsym::Var b("b");
tsym::Var c = 2*tsym::Var("a")/b/5;

if (c.has(b))
    std::cout << "c contains " << b << " and is a " << c.type() << std::endl;

std::cout << c.numerator() << ", " << c.denominator() << std::endl; /* 2*a, 5*b */

c.collectSymbols(); /* Returns a vector containing the symbols a and b. */
c.operands(); /* Returns a vector with a, b^(-1) and the fraction 2/5. */
```
Normalization, expansion and differentiation can be done as follows (all methods are declared const,
thus don't modify the object).
```c++
tsym::Var b = tsym::Var("b");
tsym::Var c = tsym::Var("a")/b + 1/(5*b);

std::cout << c.normal() << std::endl; /* 1/5*(1 + 5*a)/b */
std::cout << c.diff(b) << std::endl; /* (-1/5)/b^2 - a/b^2 */

c = c*b;

std::cout << c.expand() << std::endl; /* 1/5 + a */
```
Finally, a Matrix and a Vector class (simple wrappers around memory management) are provided with
standard operations like addition, multiplication and the like. Linear systems of equations can be
solved by the global function `solve`.
```c++
tsym::Matrix M(2, 2); /* All entries are default by default. */
tsym::Vector rhs(2);
tsym::Vector solution;

M(0, 0) = 1;
M(1, 1) = -tsym::cos(2);

rhs(0) = tsym::Var(3, 5);
rhs(1) = tsym::Var("a");

std::cout << M.det() << std::endl; /* -cos(2) */

tsym::solve(M, rhs, solution);

std::cout << solution << std::endl; /* [ 3/5, -a/cos(2) ] */
```
