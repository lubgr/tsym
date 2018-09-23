#ifndef TSYM_ORDER_H
#define TSYM_ORDER_H

#include "baseptr.h"

namespace tsym {
    bool doPermute(const Base& left, const Base& right);
    bool isCorrect(const Base& left, const Base& right);
}

#endif
