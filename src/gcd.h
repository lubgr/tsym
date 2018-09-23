#ifndef TSYM_GCD_H
#define TSYM_GCD_H

#include "baseptr.h"
#include "baseptrlist.h"
#include "int.h"

namespace tsym {
    class Number;
}

namespace tsym {
    class Gcd {
        /* Abstract class for implementations of a gcd algorithm for multivariate polynomials u and
         * v. It performs validation of the given expressions and then calls methods to be
         * implemented by concrete subclasses. These simple cases are handled by methods of this
         * class:
         *
         * - invalid input (see polyinfo utilities)
         * - u and v have no common variables
         *
         * And after expansion of the argument:
         * - u = v = 0 or one of them
         * - u = 1 or v = 1
         * - u = v
         * - u and v are both Numerics
         *
         * Implementations of a gcd algorithm thus don't need to check for those cases. Both u and
         * v are passed as expanded polynomials.
         *
         * Note that this class doesn't have state, making it less error prone while dealing with
         * the recursive nature of gcd algorithms. */
      public:
        Gcd() = default;
        Gcd(const Gcd&) = delete;
        Gcd& operator=(const Gcd&) = delete;
        Gcd(Gcd&&) = delete;
        Gcd& operator=(Gcd&&) = delete;
        virtual ~Gcd() = default;

        BasePtr compute(const BasePtr& u, const BasePtr& v) const;
        BasePtr compute(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const;

      private:
        BasePtr computeNumerics(const BasePtr& u, const BasePtr& v) const;
        Int integerGcd(const Int& u, const Int& v) const;
        bool haveCommonSymbol(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const;
        BasePtr gcdViaAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const;
        BasePtr integerContent(const BasePtr& u, const BasePtr& v) const;
        Number integerContent(const BasePtr& poly) const;
        Number integerContentOfSum(const BasePtrList& summands) const;
        BasePtr normalize(const BasePtr& result, const BasePtrList& L) const;
        Number normalizationFactor(const BasePtr& arg, BasePtrList& L) const;

        virtual BasePtr gcdAlgo(const BasePtr& u, const BasePtr& v, const BasePtrList& L) const = 0;
    };
}

#endif
