#ifndef TSYM_NUMTRIGOSIMPL_H
#define TSYM_NUMTRIGOSIMPL_H

#include <vector>
#include <unordered_map>
#include "trigonometric.h"

namespace tsym {
    /* Simplification class for trigonometric functions with arguments, that can be evaluated to a
     * numeric, e.g. Pi/4, sqrt(2), plain Numbers or any combination of these expressions. After
     * setting type and argument, the simplification can be invoked with the method compute(). Then,
     * the result can be accessed via a getter, but one should query beforehand, whether a
     * simplification could be found. Examples of the simplification are:
     *
     * - Sin(Pi/4) = 1/sqrt(2)
     * - Atan(1/sqrt(3)) = Pi/6
     * - Asin/Acos(arg) = Undefined, if arg is out of range
     * - every non-rational Numeric is evaluated to a (double) Numeric
     *
     * Everything else is not simplified. */
    class NumTrigoSimpl {
        public:
            NumTrigoSimpl();

            void setType(Trigonometric::Type type);
            void setArg(const Number& arg);
            void setArg(const BasePtr& arg);

            void compute();
            bool hasSimplifiedResult() const;
            BasePtr get() const;

        private:
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
            const BasePtr *getValue(const std::unordered_map<BasePtr, BasePtr>& table) const;
            const BasePtr *getValueNumEval(const std::unordered_map<BasePtr, BasePtr>& table) const;
            void setResult(const BasePtr& result);
            bool isDoubleNumeric(const BasePtr& ptr) const;
            void compNumericalSin();
            void compNumerically(double (*fct)(double));

            void cos();

            void tan();
            void setTanSign(unsigned quadrant);
            void tanViaSinCos();
            void setSinForTan(BasePtr& result, bool& simplified);
            void setCosForTan(BasePtr& result, bool& simplified);
            void setForTan(Trigonometric::Type type, BasePtr& result, bool& simplified);

            void computeAsinAcosAtan();
            bool isInverseArgOutOfRange() const;
            void prepareAsinAcosAtan();
            void detourAsinAcosAtan();

            void asin();
            const BasePtr *getKey(const std::unordered_map<BasePtr, BasePtr>& table) const;

            void acos();
            void acosFromAsinResult();

            void atan();

            const BasePtr Pi;
            const Number PI;

            Trigonometric::Type type;
            bool isSimplified;
            int sign;
            BasePtr origArg;
            BasePtr arg;
            BasePtr res;
    };
}

#endif
