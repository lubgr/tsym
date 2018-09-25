#ifndef TSYM_BASE_H
#define TSYM_BASE_H

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include "baseptrlist.h"

namespace tsym {
    class SymbolMap;
    class Number;
    struct Fraction;
    struct Name;
}

namespace tsym {
    class Base : public std::enable_shared_from_this<const Base> {
        /* Abstract base class for all mathematical classes (Power, Product etc.). References to
         * this class are managed by reference counting inside of the BasePtr type.
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
         * information without using casts. */
      public:
        Base(const Base&) = delete;
        Base& operator=(const Base&) = delete;
        Base(Base&&) = delete;
        Base& operator=(Base&&) = delete;
        virtual ~Base() = default;

        virtual bool isEqualDifferentBase(const Base& other) const = 0;
        virtual bool sameType(const Base& other) const = 0;
        virtual std::optional<Number> numericEval() const = 0;
        virtual Fraction normal(SymbolMap& map) const = 0;
        virtual BasePtr diffWrtSymbol(const Base& symbol) const = 0;
        virtual std::string_view typeStr() const = 0;
        /* If unclear or zero, the following two methods shall return false: */
        virtual bool isPositive() const = 0;
        virtual bool isNegative() const = 0;
        virtual unsigned complexity() const = 0;
        virtual size_t hash() const = 0;

        virtual bool isUndefined() const;
        virtual bool isSymbol() const;
        virtual bool isNumeric() const;
        virtual bool isPower() const;
        virtual bool isSum() const;
        virtual bool isProduct() const;
        virtual bool isFunction() const;
        virtual bool isConstant() const;

        virtual bool isEqual(const Base& other) const;
        virtual bool isDifferent(const Base& other) const;
        virtual bool has(const Base& other) const;
        /* Returns true for (composites of) Numerics or num. powers, nothing else: */
        virtual bool isConst() const;
        virtual BasePtr numericTerm() const;
        virtual BasePtr nonNumericTerm() const;
        /* For the following two methods, Constant types are treated as variables, only Numerics
         * and numeric Powers are considered constant (see isConst method above): */
        virtual BasePtr constTerm() const;
        virtual BasePtr nonConstTerm() const;
        virtual BasePtr expand() const;
        virtual BasePtr subst(const Base& from, const BasePtr& to) const;
        virtual BasePtr coeff(const Base& variable, int exp) const;
        virtual BasePtr leadingCoeff(const Base& variable) const;
        virtual int degree(const Base& variable) const;
        /* These two methods return clone() and 1 and must be overridden by Power only. */
        virtual BasePtr base() const;
        virtual BasePtr exp() const;

        /* Returns Symbol/Constant/Function name, an empty Name otherwise: */
        virtual const Name& name() const;

        BasePtr clone() const;
        BasePtr normal() const;
        BasePtr diff(const Base& symbol) const;
        const BasePtrList& operands() const;

      protected:
        Base() = default;
        explicit Base(BasePtrList operands);

        bool isEqualByTypeAndOperands(const Base& other) const;
        void setDebugString();

        const BasePtrList ops{};

        /* Empty struct for using make_shared for subclasses that shall not be created directly,
         * but only via their static creation methods. */
        struct CtorKey {};

      private:
        BasePtr normalViaCache() const;
        BasePtr normalWithoutCache() const;

#ifdef TSYM_WITH_DEBUG_STRINGS
        /* A member to be accessed by a gdb pretty printing plugin. As the class is immutable,
         * it has to be filled with content during initialization only. */
        std::string prettyStr;
#endif
    };

    std::ostream& operator<<(std::ostream& stream, const Base& arg);
}

#endif
