
# tsym: Tiny Symbolic Library [![Build Status](https://travis-ci.org/lubgr/tsym.svg?branch=develop)](https://travis-ci.org/lubgr/tsym) [![Coverage Status](https://coveralls.io/repos/github/lubgr/tsym/badge.svg?branch=develop)](https://coveralls.io/github/lubgr/tsym?branch=develop) [![BCH compliance](https://bettercodehub.com/edge/badge/lubgr/tsym?branch=develop)](https://bettercodehub.com/results/lubgr/tsym)

This small C++ library intends to provide a simple approach to symbolic algebra. In contrast to
fully-fledged computer algebra systems or libraries (e.g. maxima, GiNaC, sympy), only basic features
are implemented and the scope is limited to real-valued symbols: no complex numbers are taken into
account during expression simplification (this prohibits some simplifications and allows for some).
The main features are:

* automatic simplification `a + b + 2*a = 3*a + b`
* expand expressions `(a + b)^2 = a^2 + 2*a*b + b^2`
* substitute (sub-)expressions `3*a + b = 6 + b` for `a = 2`
* trigonometric functions `sin(pi/4) = 1/sqrt(2)` or `sin^2(a) + cos^2(a) = 1`
* expression normalization via gcd `a/b + 1/(5*b) = 1/5*(1 + 5*a)/b`
* differentiation `d/da 2*a^4 = 8*a^3`
* solution of linear systems of equations, matrix inversion and determinant
* parsing of expressions from strings
* big integer arithmetic (by use of boost multiprecision)

Most algorithms are implemented according to _Cohen, Computer Algebra and Symbolic Computation
[2003]_, modifications are made for the handling of numeric power expressions and when not taking
complex numbers into account. Absent features often implemented in other CAS are e.g. arbitrary
precision arithmetics for floating point numbers, collecting parts of an expression or series
expansion.

Note that this library is under development and _far from being stable_. Breaking changes are to be
expected.

Setup
-----
To compile tsym, you need a compiler supporting
[C++17](https://en.cppreference.com/w/cpp/compiler_support#cpp17), [cmake](https://cmake.org) and
the [boost](https://boost.org) headers (build dependency only, version >= 1.65). The easiest way to
integrate tsym into a project is to leverage cmake in conjunction with git submdodules:
```bash
cd path/to/your/project
git submodule add https://github.com/lubgr/tsym external/tsym
```
In the `CMakeLists.txt` of your application, add
```
add_subdirectory(external/tsym)

target_link_libraries(yourTarget PRIVATE tsym::tsym)
```
and on Linux or MacOS, you should now be able to compile via the usual
```bash
mkdir build && cd build
cmake .. # pass e.g. -D BOOST_ROOT=/non/standard/path
make
```
The configuration step can be tweaked with common cmake options
([CMAKE_BUILD_TYPE](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html),
[BUILD_SHARED_LIBS](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html),
[BOOST_ROOT](https://cmake.org/cmake/help/latest/module/FindBoost.html),
[CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html)
etc.). An alternative is a system-wide installation (`git clone`, `cmake -D [OPTIONS]
[path/to/tsym]`, `make install`). In case of Archlinux, there is also an [AUR
package](https://aur.archlinux.org/packages/tsym-git). When using cmake for the client application,
use `find_package(tsym)` instead of `add_subdirectory`, otherwise pass e.g. `-std=c++17`, `-ltsym`
and `-I`/`-L` to your compiler where appropriate. For compiling unit tests, configure the tsym build
with [BUILD_TESTING](https://cmake.org/cmake/help/latest/module/CTest.html)`=ON`. The test
executable links to the boost test framework, and the appropriate static library must be available.

Usage
-----
There is only one header that needs to be included: `tsym/tsym.h` (don't worry about compile times,
the library is indeed tiny). The library provides a **tsym::Var** class holding all expressions. It
can be instantiated as a symbol or a number, and arithmetic as well as stream operators are
overloaded:
```c++
#include <iostream>
#include "tsym/tsym.h"

int main(int argc, char **argv)
{
    tsym::Var a("a");
    tsym::Var b("b_{subscript}");
    tsym::Var fraction(2, 3);

    std::cout << a + b*fraction << "\n";
}
```
Variable names and subscripts must be ASCII characters or numbers. A variable name can't start with
a number, curly braces around the subscript can be omitted when it's only one character. Big
integers can be constructed by a digit-only string, and implicit constructors are available for
`int` and `double` arguments, such that intuitive calculations are possible:
```c++
tsym::Var largeInt("98938498203948203948203948209384029384092834098");
tsym::Var a("a");
tsym::Var b; /* Initialized to zero by default constructor. */

b = 2*a + a/1.2345 - largeInt;

std::cout << b << "\n";

/* Careful - this will be zero, because 2/3 is evaluated first and as plain integral type: */
b = 2/3*a;
```
Functions for square root, power, logarithm and (inverse) trigonometric maps including `atan2` as
well as Euler and Pi constants are available inside of the tsym namespace:
```c++
tsym::Var minusPiFourth = tsym::pi()/4 - tsym::asin(1);
tsym::Var onePlusPiFourth = tsym::log(tsym::euler()) + tsym::acos(1/tsym::sqrt(2));
tsym::Var eToTheFour = tsym::pow(tsym::euler(), 4);
```
Information about expressions can be queried via free functions accepting a Var instance or member
functions:
```c++
tsym::Var b("b");
tsym::Var c = 2*tsym::Var("a")/b/5;

if (tsym::has(c, b) && c.type() == tsym::Var::Type::PRODUCT)
    std::cout << "c contains " << b << " and is a " << c.type() << "\n";

std::cout << tsym::numerator(c) << ", " << tsym::denominator(c) << "\n"; /* 2*a, 5*b */

tsym::collectSymbols(c); /* Returns a vector containing the symbols a and b. */
tsym::operands(c); /* Returns a vector with a, b^(-1) and the fraction 2/5. */
```
The type() method used in this snippet returns a enum class inside of Var with possible values
`PRODUCT`, `SYMBOL`, `INT`, `FRACTION`, `DOUBLE`, `CONSTANT`, `UNDEFINED`, `FUNCTION`, `SUM` and
`POWER`. Simplification, expansion and differentiation can be done as follows (all Var parameters
are const, thus don't modify the object).
```c++
tsym::Var a = tsym::Var("a");
tsym::Var b = tsym::Var("b");
tsym::Var c = a/b + 1/(5*b);

std::cout << tsym::subst(c, b, tsym::Var(1, 3)) << "\n"; /* 3/5 + 3a */
std::cout << tsym::diff(c, b) << "\n"; /* (-1/5)/b^2 - a/b^2 */

c = c*b;

std::cout << tsym::expand(c) << "\n"; /* 1/5 + a */
```
The construction of expressions can be done by parsing of strings, too, by invocation of a free
function
```c++
const std::optional<tsym::Var> parsedFromString = tsym::parse("a*cos(pi/5) + sqrt(3)*log(euler)");
```
This can go wrong, hence the return value.
```c++
const auto parseAttempt = tsym::parse("a*cos(3!)");

if (!parseAttempt)
    std::cout << "Factorials aren't implemented\n";
```
The parser does very limited error recovery, and partially parsed result are
simply discarded. Three standard operations for matrices are provided as free
functions, i.e., solution of linear systems of equations, matrix inversion and
computing determinants. All three use the LU decomposition and a partial
pivoting scheme and are implemented as function templates that accept any matrix
type providing element access via `(i, j)` or `[i][j]` operators and any vector
type with `(i)` or `[i]` operators. It's fine to use simple container types
(`std::vector<tsym::Var>`, `std::vector<std::vector<tsym::Var>`, plain C-style
arrays, `std::array` and so on. More sophisticated matrix/vector types don't
help the performance that is solely governed by internal simplifications.
```c++
std::vector<std::vector<tsym::Var>> A;
boost::numeric::ublas::matrix<tsym::Var> B;

/* ... fill matrices with values ... */

tsym::determinant(A, A.size()); /* A is modified in place! */
tsym::invert(B, B.size1());
```
The dimension (second parameter in the example above) should be passed as the container index type
(usually some `unsigned` integral type). Instantiating the function with e.g. a literal integer
will probably result in many unwanted sign conversions. When using a type that doesn't support the
standard access operators, a callable entity can be passed to these functions.
```c++
std::array<tsym::Var, 16> C; /* A 4x4 matrix with elements stored contingously. */
std::unique_ptr<tsym::Var[]> rhs(new tsym::Var[4]);
std::unique_ptr<tsym::Var[]> x(new tsym::Var[4]);

/* ... fill matrix and and right hand side with values ... */

tsym::solve(C, rhs, x, std::size_t{4}, [](auto& A, auto i, auto j) -> tsym::Var& { return A[4u*i + j]; });

/* A and rhs have changed now, x contains the solution. */
```
Different vector types for solution and right hand side aren't supported. Last, a sixth functions
argument can be passed to `solve`, specifying the vector access function. When the given coefficient
matrix is singular, all functions will throw an instance of `std::invalid_argument`. To keep the
requirements on vector/matrix types low, no sanity checks are made (client code is hence responsible
for correct dimensions)

Additional notes
----------------
* `Var` objects can be used as keys in `std::unordered_map` containers, as `std::hash` is specified
  for this class.
* Avoid `using namespace tsym` because `sqrt` and `pow` from math.h are in the global namespace.
  `sqrt(2)` will thus be evaluated to a `double`, while `tsym::sqrt(2)` gives the desired result.
* Control over logging output can be implemented by providing a subclass of `tsym::Logger` that
  overrides the debug/info/warning/error/critical methods. An instance of that subclass must then be
  registered to replace the default behavior (print warning, error and critical messages to standard
  output) by `tsym::Logger::setInstance(...);` where the argument is a `std::unique_ptr` to the
  Logger object.
