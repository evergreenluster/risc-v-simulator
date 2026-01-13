#ifndef _CPU_SINGLE_HART_
#define _CPU_SINGLE_HART_

#include <iostream>
#include <cstdint>
#include "rv32i_hart.h"

class cpu_single_hart : public rv32i_hart
{
    public:
        // Implement this constructor as shown above in order to pass the memory class instance to the constructor in the base class.
        // (look into constructor logic with inheritance)
        cpu_single_hart(memory &mem) : rv32i_hart(mem) {};

        // see doc
        void run(uint64_t exec_limit);
};

#endif