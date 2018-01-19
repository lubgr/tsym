#ifndef TSYM_PRINTENGINE_H
#define TSYM_PRINTENGINE_H

#include <ostream>
#include <functional>
#include "baseptr.h"
#include "logging.h"

namespace tsym {
    template<class Engine> class PrintEngine {
        public:
            PrintEngine(std::ostream& stream) : out(stream) {}

            void registerToplevelPrintFunction(std::function<void(Engine&, const BasePtr&)> function)
            {
                printFunction = function;
            }

            Engine& invokePrint(const BasePtr& ptr)
            {
                if (printFunction)
                    printFunction(static_cast<Engine&>(*this), ptr);
                else
                    TSYM_ERROR("No toplevel print function registered");

                return static_cast<Engine&>(*this);
            }

        protected:
            std::ostream& out;

        private:
            std::function<void(Engine&, const BasePtr&)> printFunction;
    };
}

#endif
