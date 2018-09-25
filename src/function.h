#ifndef TSYM_FUNCTION_H
#define TSYM_FUNCTION_H

#include "base.h"
#include "name.h"

namespace tsym {
    class Function : public Base {
        /* Underlying abstract base class for functions to be further specified by inheriting
         * classes. For those, a constructor is provided, otherwise, there are getter to access the
         * name and the argument of the function. */
      public:
        Function(const Function&) = delete;
        Function& operator=(const Function&) = delete;
        Function(Function&&) = delete;
        Function& operator=(Function&&) = delete;

        bool isEqualDifferentBase(const Base& other) const override;
        bool sameType(const Base& other) const override;
        std::string_view typeStr() const override;
        size_t hash() const override;

        bool isConst() const override;
        bool isFunction() const override;
        BasePtr constTerm() const override;
        BasePtr nonConstTerm() const override;
        const Name& name() const override;

      protected:
        Function(const BasePtrList& args, Name&& name);
        ~Function() override = default;

      private:
        const Name functionName;
    };
}

#endif
