#ifndef TSYM_BASE_H
#define TSYM_BASE_H

#include <memory>
#include "number.h"
#include "baseptrlist.h"
#include "fraction.h"
#include "name.h"

namespace tsym { class SymbolMap; }

namespace tsym {
    class Base : public std::enable_shared_from_this<const Base> {
        /* Abstract base class for all mathematical classes (Power, Product etc.). References to
         * this class are managed by intrusive reference counting inside of the BasePtr class.
         * Objects of this type will be passed around most of the time.
         *
         * The Base class and its subclasses can be understood as an implementation of the GoF
         * Composite pattern, where all objects are immutable (thus, no adding/removing of
         * children). The subclasses Numeric, Undefined, Symbol and Constant are leafs, Sum,
         * Product, Power and Function are composites, where the latter has only one child.
         *
         * Some methods provided by this interface do make sense only for particular subclasses,
         * e.g. accessing the operands for leafs will return a reference to an empty BasePtr
         * container or the name() methods returns an empty Name for all subclasses except Function,
         * Symbol or Constant. This clutters the interface a bit, but provides easy access to all
         * information without using casts or other runtime informations. */
        public:
            virtual bool isEqualDifferentBase(const BasePtr& other) const = 0;
            virtual bool sameType(const BasePtr& other) const = 0;
            virtual Number numericEval() const = 0;
            virtual Fraction normal(SymbolMap& map) const = 0;
            virtual BasePtr diffWrtSymbol(const BasePtr& symbol) const = 0;
            virtual std::string typeStr() const = 0;
            /* If unclear or zero, the following two methods shall return false: */
            virtual bool isPositive() const = 0;
            virtual bool isNegative() const = 0;
            virtual size_t hash() const = 0;

            virtual bool isZero() const;
            virtual bool isOne() const;
            virtual bool isNumericallyEvaluable() const;
            virtual bool isUndefined() const;
            virtual bool isSymbol() const;
            virtual bool isNumeric() const;
            virtual bool isPower() const;
            virtual bool isNumericPower() const;
            virtual bool isSum() const;
            virtual bool isProduct() const;
            virtual bool isFunction() const;
            virtual bool isConstant() const;

            virtual bool isEqual(const BasePtr& other) const;
            virtual bool isDifferent(const BasePtr& other) const;
            virtual bool has(const BasePtr& other) const;
            /* Returns true for (composites of) Numerics or num. powers, nothing else: */
            virtual bool isConst() const;
            virtual BasePtr numericTerm() const;
            virtual BasePtr nonNumericTerm() const;
            /* For the following two methods, Constant types are treated as variables, only Numerics
             * and numeric Powers are considered constant (see isConst method above): */
            virtual BasePtr constTerm() const;
            virtual BasePtr nonConstTerm() const;
            virtual BasePtr expand() const;
            virtual BasePtr subst(const BasePtr& from, const BasePtr& to) const;
            virtual BasePtr coeff(const BasePtr& variable, int exp) const;
            virtual BasePtr leadingCoeff(const BasePtr& variable) const;
            virtual int degree(const BasePtr& variable) const;
            /* These two methods return clone() and 1 and must be overridden by Power only. */
            virtual BasePtr base() const;
            virtual BasePtr exp() const;
            /* Returns Symbol/Constant/Function name, an empty Name otherwise: */
            virtual const Name& name() const;

            BasePtr clone() const;
            BasePtr normal() const;
            BasePtr diff(const BasePtr& symbol) const;
            const BasePtrList& operands() const;

            Base();
            Base(const BasePtrList& operands);
            virtual ~Base();

        protected:
            bool isEqualByTypeAndOperands(const BasePtr& other) const;
            void setDebugString();

            const BasePtrList ops;

        private:
            BasePtr normalViaCache() const;
            BasePtr normalWithoutCache() const;

#ifdef TSYM_DEBUG_STRINGS
            /* A member to be accessed by a gdb pretty printing plugin. As the class is immutable,
             * it has to be filled with content during initialization only. */
            std::string prettyStr;
#endif
    };
}

#endif
