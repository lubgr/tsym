#ifndef TSYM_POWERSIMPL_H
#define TSYM_POWERSIMPL_H

#include "baseptrlist.h"
#include "number.h"

namespace tsym {
    class PowerSimpl {
        /* Processes the simplification of power expressions. A BasePtrList with 2 items is
         * returned, the first one being the base, the second one the exponent. If the latter may be
         * one, which makes it possible to return products (e.g. sqrt(8) = 2*sqrt(2)) or numerics
         * (e.g. sqrt(4) = 2). For details of the simplification of numeric powers, see the
         * NumPowerSimpl class. */
        public:
            PowerSimpl();

            BasePtrList simplify(const BasePtr& base, const BasePtr& exp);

        private:
            BasePtrList simplifyNumericBase(const BasePtr& base, const BasePtr& exp);
            BasePtrList simplifyNumericPower(const BasePtr& base, const BasePtr& exp);
            BasePtrList simplifyNumericPower(const Number& base, const Number& exp);
            BasePtrList simplifyPowerBase(const BasePtr& base, const BasePtr& exp);
            BasePtrList simplifyProductBase(const BasePtr& base, const BasePtr& exp);
            BasePtrList simplifyConstantBase(const BasePtr& base, const BasePtr& exp);
            bool isBaseEulerConstantAndExpLogarithm(const BasePtr& base, const BasePtr& exp);

            const BasePtr one;
    };
}

#endif
