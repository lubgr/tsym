#ifndef TSYM_NUMTRIGOSIMPL_H
#define TSYM_NUMTRIGOSIMPL_H

#include <optional>
#include <unordered_map>
#include <vector>
#include "constant.h"
#include "number.h"
#include "trigonometric.h"

namespace tsym {
    /* Simplification class for trigonometric functions with arguments, that can be evaluated to a
     * numeric, e.g. Pi/4, sqrt(2), plain Numbers or any combination of these expressions. Examples
     * of the simplification are:
     *
     * - Sin(Pi/4) = 1/sqrt(2)
     * - Atan(1/sqrt(3)) = Pi/6
     * - Asin/Acos(arg) = Undefined, if arg is out of range
     * - every non-rational Numeric is evaluated to a (double) Numeric
     *
     * Everything else is not simplified. */
    class NumTrigoSimpl {
      public:
        explicit NumTrigoSimpl(BasePtr arg);
        NumTrigoSimpl(BasePtr arg, Trigonometric::Type type);

        std::optional<BasePtr> simplify();

      private:
        void resetAndCompute();
        void reset();
        void detour();
        bool isNotAnInverseFct() const;
        void computeSinCosTan();
        bool isArgRationalNonZeroNumeric() const;
        bool isRationalNumeric(const BasePtr& ptr) const;
        void prepareSinCosTan();
        void adjustNumericArg();
        void adjustArgRange();
        void detourSinCosTan();

        void sin();
        unsigned getQuadrant() const;
        void setSinSign(unsigned quadrant);
        void shiftToFirstQuadrant(unsigned quadrant);
        void compShiftedSin();
        void resultTimesSign();
        std::optional<BasePtr> getValue(const std::unordered_map<BasePtr, BasePtr>& table) const;
        std::optional<BasePtr> getValueNumEval(const std::unordered_map<BasePtr, BasePtr>& table) const;
        bool isDoubleNumeric(const BasePtr& ptr) const;
        void compNumericalSin();
        template <class Fct> void compNumerically(Fct&& eval);
        void setTimesSign(const BasePtr& newResult);

        void cos();

        void tan();
        void setTanSign(unsigned quadrant);
        void tanViaSinCos();
        std::optional<BasePtr> computeForTan(Trigonometric::Type type) const;

        void computeAsinAcosAtan();
        bool isInverseArgOutOfRange() const;
        void prepareAsinAcosAtan();
        void detourAsinAcosAtan();

        void asin();
        std::optional<BasePtr> getKey(const std::unordered_map<BasePtr, BasePtr>& table) const;

        void acos();
        void acosFromAsinResult();

        void atan();

        const BasePtr Pi{Constant::createPi()};
        const Number PI{*Pi->numericEval()};

        Trigonometric::Type type;
        int sign = 1;
        BasePtr origArg;
        BasePtr arg;
        std::optional<BasePtr> result = std::nullopt;
    };
}

#endif
