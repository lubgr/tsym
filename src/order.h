#ifndef TSYM_ORDER_H
#define TSYM_ORDER_H

#include "baseptr.h"

namespace tsym {
    namespace order {
        bool doPermute(const BasePtr& left, const BasePtr& right);
        bool isCorrect(const BasePtr& left, const BasePtr& right);
    }
}

#endif
