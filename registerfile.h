#ifndef _REGISTER_FILE_
#define _REGISTER_FILE_

#include <iostream>
#include <cstdint>
#include <vector>
#include "hex.h"

class registerfile : public hex
{
    public:
        registerfile()
        {
            reset();
        }

        // Initialize register x0 to zero, and all other registers to 0xf0f0f0f0.
        void reset();

        //  Assign register r the given val. If r is zero then do nothing.
        void set(uint32_t r, int32_t val);

        //  Return the value of register r. If r is zero then return zero.
        int32_t get(uint32_t r) const;

        // see documentation
        void dump(const std::string &hdr) const;

    private:
        std::vector<int32_t> regs;
};

#endif