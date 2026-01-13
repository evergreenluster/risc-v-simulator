#include "registerfile.h"
#include <iostream>
#include <iomanip>

// Initialize register x0 to zero, and all other registers to 0xf0f0f0f0.
void registerfile::reset()
{
    regs[0] = 0;

    for (int32_t& reg : regs)
    {
        reg = 0xf0f0f0f0;
    }
}

//  Assign register r the given val. If r is zero then do nothing.
void registerfile::set(uint32_t r, int32_t val)
{
    if (r != 0)
    {
        regs[r] = val;
    }
}

//  Return the value of register r. If r is zero then return zero.
int32_t registerfile::get(uint32_t r) const
{
    return regs[r];
}

// see documentation
void registerfile::dump(const std::string &hdr) const
{
    for (size_t i = 0; i < 32; i++) 
    {
        // start a new line with the address at every 32-byte boundary
        if (i % 32 == 0) 
        {    
            if (hdr != "")
            {
                std::cout << hdr;
            }
            // print the starting address of the current 32-byte line
            std::cout << std::setw(3) << to_hex8(i) << " ";
        }

        // print an extra space at 16-byte midpoint for readability
        if (i % 16 == 0 && i % 32 != 0) 
        {
            std::cout << " ";
        }

        // print byte value in hex
        std::cout << to_hex32(regs[i]) << " ";
    }
}